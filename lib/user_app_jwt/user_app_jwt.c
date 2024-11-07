/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <zephyr/sys/base64.h>

#include <sdfw/sdfw_services/device_info_service.h>

#include <psa/crypto.h>
#include <psa/crypto_extra.h>
#include <psa/nrf_platform_key_ids.h>

#include <cJSON.h>

#include <user_app_jwt.h>

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(user_app_jwt, CONFIG_USER_APP_JWT_LOG_LEVEL);

/* Size of a UUID in words */
#define UUID_BINARY_WORD_SZ (4)

/* String size of a binary word encoded in hexadecimal */
#define BINARY_WORD_STR_SZ  (9)

/* Size of an ECDSA signature */
#define ECDSA_SHA_256_SIG_SZ	(64)

/* Size of an SHA 256 hash */
#define ECDSA_SHA_256_HASH_SZ	(32)

/* Size of a private ECDSA key in raw binary format */
#define ECDSA_PRIVATE_KEY_SZ    (32)

/* Size of a public ECDSA key in raw binary format  */
#define ECDSA_PUBLIC_KEY_SZ		(65)

/* Macro to determine the size of a data encoded in base64 */
#define BASE64_ENCODE_SZ(n)	(((4 * n / 3) + 3) & ~3)

/* Size of ECDSA signature encoded in base64 */
#define B64_SIG_SZ		(BASE64_ENCODE_SZ(ECDSA_SHA_256_SIG_SZ) + 1)

/* Lenght of random bites added to JTI field */
#define JWT_NONCE_BITS            (128)

/* Lenght of JTI field, this should be at least UUIDv4 size + size of board name */
#define JWT_CLAIM_FILED_STR_LENGTH (64)

/* lenght of uint64_t_max written as a string + 1 string termination character */
#define JWT_EXPIRATION_STR_LENGTH (21)

//---------------------------------------------------------------------------//
static uint32_t swap_endian(uint32_t value)
{
    return ((value >> 24) & 0xFF) |
			((value << 8) & 0xFF0000) |
			((value >> 8) & 0xFF00) |
			((value << 24) & 0xFF000000);
}


#if defined(CONFIG_DATE_TIME)
// TODO : user propper date_time_now() API.
//---------------------------------------------------------------------------//
static int date_time_now(uint64_t *exp_ts_s)
{
	/* no time source for the moment, this will always return
		value in seconds since january 1970 assuming today's date is
		Monday, January 1, 2024 12:00:00 AM
		use https://www.epochconverter.com/ to generate a new value */
	
	*exp_ts_s = 1704067200;

	return 0;
}
#endif /* CONFIG_DATE_TIME */

//---------------------------------------------------------------------------//
static void base64_url_format(char *const base64_string)
{
	if (base64_string == NULL) {
		return;
	}

	char *found = NULL;

	/* replace '+' with "-" */
	for (found = base64_string; (found = strchr(found, '+'));) {
		*found = '-';
	}

	/* replace '/' with "_" */
	for (found = base64_string; (found = strchr(found, '/'));) {
		*found = '_';
	}

	/* remove padding '=' */
	found = strchr(base64_string, '=');
	if (found) {
		*found = '\0';
	}
}


//---------------------------------------------------------------------------//
static int bytes_to_uuid_str(const uint32_t *uuid_words, const int32_t uuid_byte_len,
							char *uuid_str_out, const int32_t uuid_str_out_size)
{

	if ((NULL == uuid_words) || (uuid_byte_len < UUID_BINARY_WORD_SZ * 4) ||
		(NULL == uuid_str_out) || (uuid_str_out_size < NRF_UUID_V4_STR_LEN)) {
		/* bad parameter */
		return -EINVAL;
	}
	// this will hold 4 integer words in string format (string lenght 8 + 1)
	char temp_str[UUID_BINARY_WORD_SZ][BINARY_WORD_STR_SZ] = {0};

	// Transform integers to strings
	for (int i = 0; i < UUID_BINARY_WORD_SZ; i++) {
		// UUID byte endiannes is little endian first.
		sprintf(temp_str[i], "%08x", swap_endian(uuid_words[i]));
	}

	// UUID string format defined by  ITU-T X.667 | ISO/IEC 9834-8 :
	// <8 char>-<4 char>-<4 char>-<4 char>-<12 char>

	// for some reason, sprintf will not respect the amout of characters to print that is
	// specified in the formatting string. use this cascade of sprintf to overcome this issue.
	// 59fa2510\0 , 8952abc5\0 , a20cd981\0 , 2bf2e936\0
	//'^         ' = "59fa2510"
	sprintf(uuid_str_out, "%8s", temp_str[0]);
	// 59fa2510\0 , 8952abc5\0 , a20cd981\0 , 2bf2e936\0
	//			   '^   ' = "8952"
	sprintf(uuid_str_out + 8, "-%4s", temp_str[1] + 0);
	// 59fa2510\0 , 8952abc5\0 , a20cd981\0 , 2bf2e936\0
	//			       '^   ' = "abc5"
	sprintf(uuid_str_out + 8 + 5, "-%4s", temp_str[1] + 4);
	// 59fa2510\0 , 8952abc5\0 , a20cd981\0 , 2bf2e936\0
	//  			            '^   ' = "a20c"
	sprintf(uuid_str_out + 8 + 5 + 5, "-%4s", temp_str[2] + 0);
	// 59fa2510\0 , 8952abc5\0 , a20cd981\0 , 2bf2e936\0
	//  			                '^   ' + '^         '  = "d9812bf2e936"
	sprintf(uuid_str_out + 8 + 5 + 5 + 5, "-%4s%8s", temp_str[2] + 4, temp_str[3]);
	uuid_str_out[NRF_UUID_V4_STR_LEN] = '\0';

	return 0;
}


//---------------------------------------------------------------------------//
static int crypto_init(void)
{
	psa_status_t status;

	/* Initialize PSA Crypto */
	status = psa_crypto_init();
	if (status != PSA_SUCCESS) {
		LOG_ERR("psa_crypto_init failed! (Error: %d)", status);
		return -1;
	}

	return 0;
}


//---------------------------------------------------------------------------//
static int export_ecdsa_public_key_hash(const uint32_t user_key_id,
										uint8_t *key_hash_out,
										size_t key_hash_buffer_size,
										size_t *key_hash_lenght)
{
	psa_status_t status;
	size_t olen;
	uint8_t pub_key[ECDSA_PUBLIC_KEY_SZ];

	/* Export the public key */
	status = psa_export_public_key(user_key_id, pub_key, sizeof(pub_key), &olen);
	if (status != PSA_SUCCESS) {
		LOG_ERR("psa_export_public_key failed! (Error: %d)", status);
		return -1;
	}

	/* Compute the SHA256 hash of public key */
	status = psa_hash_compute(PSA_ALG_SHA_256,
				  pub_key,
				  sizeof(pub_key),
				  key_hash_out,
				  key_hash_buffer_size,
				  key_hash_lenght);
	if (status != PSA_SUCCESS) {
		LOG_ERR("psa_hash_compute failed! (Error: %d)", status);
		return -1;
	}

	return status;
}


//---------------------------------------------------------------------------//
static int hash_and_sign_message(const uint32_t user_key_id, const uint8_t *input, size_t input_length,
				uint8_t *signature, size_t signature_size, size_t *signature_length)
{
	uint32_t output_len;
	uint8_t msg_hash[ECDSA_SHA_256_HASH_SZ];
	psa_status_t status;

	/* Compute the SHA256 hash*/
	status = psa_hash_compute(PSA_ALG_SHA_256,
				  input,
				  input_length,
				  msg_hash,
				  sizeof(msg_hash),
				  &output_len);
	if (status != PSA_SUCCESS) {
		LOG_ERR("psa_hash_compute failed! (Error: %d)", status);
		return -1;
	}

	/* Sign the hash */
	status = psa_sign_message(user_key_id,
			       PSA_ALG_ECDSA(PSA_ALG_SHA_256),
			       msg_hash,
			       sizeof(msg_hash),
			       signature,
			       signature_size,
			       &output_len);
	if (status != PSA_SUCCESS) {
		LOG_ERR("psa_sign_hash failed! (Error: %d)", status);
		return -1;
	}

	*signature_length = output_len;

	return 0;
}


//---------------------------------------------------------------------------//
static int get_random_bytes(uint8_t *output_buffer, const size_t output_length)
{
	if ((NULL == output_buffer) || (output_length < 4)) {
		/* bad parameter */
		return -EINVAL;
	}

	int err;
	err = crypto_init();
	if (err) {
		LOG_ERR("Failed to initialize PSA Crypto, error: %d", err);
		err = -ENOMEM;
	}

	psa_status_t psa_status = psa_generate_random(output_buffer, output_length);
	if (PSA_SUCCESS != psa_status) {
		LOG_ERR("psa_generate_random failed! = %d", psa_status);
		err = -ENOMEM;
	}

	return err;
}

//---------------------------------------------------------------------------//
static int crypto_finish(void)
{
	psa_status_t status;

	/* Purge the key from memory */
	status = psa_purge_key(IAK_APPLICATION_GEN1);
	if (status != PSA_SUCCESS) {
		LOG_ERR("psa_purge_key failed! (Error: %d)", status);
		return -1;
	}

	return 0;
}


//---------------------------------------------------------------------------//
static char *jwt_header_create(const uint32_t alg, const uint32_t keyid)
{
	int err = 0;
	uint8_t pub_key_hash[ECDSA_SHA_256_HASH_SZ];
	char *hdr_str = NULL;
	cJSON *jwt_hdr = cJSON_CreateObject();

	// Type: format: always "JWT"
	if (cJSON_AddStringToObjectCS(jwt_hdr, "typ", "JWT") == NULL) {
		return NULL;
	}

	// Algorithme: format: always "ES256"
	if (JWT_ALG_TYPE_ES256 == alg) {
		if (cJSON_AddStringToObjectCS(jwt_hdr, "alg", "ES256") == NULL) {
			return NULL;
		}
	} else {
		return NULL;
	}

	// Keyid: format: sha256 string
	/* init crypto service */
	err = crypto_init();
	if (err) {
		LOG_ERR("Failed to initialize PSA Crypto, error: %d", err);
		return NULL;
	}

	/* Get kid: sha256 over public key */
	size_t olen;
	err = export_ecdsa_public_key_hash(IAK_APPLICATION_GEN1, pub_key_hash, ECDSA_SHA_256_HASH_SZ, &olen);
	if (err) {
		LOG_ERR("Failed to export public key, error: %d", err);
		return NULL;
	}

	char sha256_str[ECDSA_SHA_256_SIG_SZ + 1] = {0};
	int32_t printed_bytes = 0;
	for (uint32_t i = 0; i < 8; i++) {
		printed_bytes += sprintf((sha256_str + printed_bytes), "%08x", swap_endian((uint32_t)*((uint32_t *)pub_key_hash + i)));
	}
	sha256_str[ECDSA_SHA_256_SIG_SZ] = '\0';

	if (cJSON_AddStringToObjectCS(jwt_hdr, "kid", sha256_str) == NULL) {
		err = -ENOMEM;
	}

	if (err == 0) {
		hdr_str = cJSON_PrintUnformatted(jwt_hdr);
	}

	cJSON_Delete(jwt_hdr);
	jwt_hdr = NULL;

	return hdr_str;
}


//---------------------------------------------------------------------------//
static char *convert_str_to_b64_url(const char *const str)
{
	size_t str_len = strlen(str);
	size_t b64_sz = BASE64_ENCODE_SZ(str_len) + 1;

	char * const b64_out = calloc(b64_sz, 1);

	if (b64_out) {
		int err = base64_encode(b64_out, b64_sz, &b64_sz, str, strlen(str));

		if (err) {
			LOG_ERR("base64_encode failed, error: %d", err);
		}
	}

	/* Convert to base64 URL */
	base64_url_format(b64_out);

	return b64_out;
}


//---------------------------------------------------------------------------//
static char *jwt_header_payload_combine(const char *const hdr, const char *const pay)
{
	/* Allocate buffer for the JWT header and payload to be signed */
	size_t msg_sz = strlen(hdr) + 1 + strlen(pay) + 1;
	char *msg_out = calloc(msg_sz, 1);

	/* Build the base64 URL JWT to sign:
	 * <base64_header>.<base64_payload>
	 */
	if (msg_out) {
		int ret = snprintk(msg_out, msg_sz, "%s.%s", hdr, pay);

		if ((ret < 0) || (ret >= msg_sz)) {
			LOG_ERR("Could not format JWT to be signed");
			free(msg_out);
			msg_out = NULL;
		}
	}

	return msg_out;
}


//---------------------------------------------------------------------------//
static char *jwt_payload_create(const char *const sub, const char *const aud, uint64_t exp)
{
	int err = 0;
	char *pay_str = NULL;
	cJSON *jwt_pay = cJSON_CreateObject();

	// Issued At: format: time in seconds as integer
	uint64_t issue_time = 0;
#if defined(CONFIG_DATE_TIME)
	err = date_time_now(&issue_time);
	if (!err) {
		LOG_WARN("Unable to get a valid timestamp, using value 0s");
		issue_time = 0;
	}
#endif /* CONFIG_DATE_TIME */
	if (cJSON_AddNumberToObjectCS(jwt_pay, "iat", issue_time) == NULL) {
		err = -ENOMEM;
	}

	// Json Token ID: format: <hardware_id>.<16-random_bytes>
	uint32_t nonce_words[JWT_NONCE_BITS/32] = {0};
	err = get_random_bytes((uint8_t *)nonce_words, JWT_NONCE_BITS/8);
	if (err) {
		LOG_ERR("get_random_bytes failed! = %d", err);
		err = -ENOMEM;
	} else {
		char nonce_uuid_str[NRF_UUID_V4_STR_LEN] = {0};
		bytes_to_uuid_str(nonce_words, UUID_BINARY_WORD_SZ * sizeof(uint32_t), nonce_uuid_str, NRF_UUID_V4_STR_LEN);

		char jti_str[JWT_CLAIM_FILED_STR_LENGTH] = {0};
		int jti_str_length = sprintf(jti_str, "%s.%s", CONFIG_BOARD, nonce_uuid_str);
		jti_str[jti_str_length + 1] = '\0';
		if (cJSON_AddStringToObjectCS(jwt_pay, "jti", jti_str) == NULL) {
			err = -ENOMEM;
		}
	}

	// Issuer: format: <hardware_id>.<UUID>
	char iss_str[JWT_CLAIM_FILED_STR_LENGTH] = {0};
	int iss_str_length = sprintf(iss_str, "%s.%s", CONFIG_BOARD, sub);
	iss_str[iss_str_length + 1] = '\0';
	if (cJSON_AddStringToObjectCS(jwt_pay, "iss", iss_str) == NULL) {
		err = -ENOMEM;
	}

	// Subject: format: "user_defined_string"
	if (sub && (cJSON_AddStringToObjectCS(jwt_pay, "sub", sub) == NULL)) {
		err = -ENOMEM;
	}

	// Audiance: format: "user_defined_string"
	if (aud && (cJSON_AddStringToObjectCS(jwt_pay, "aud", aud) == NULL)) {
		err = -ENOMEM;
	}

	// Expiration: format: time in seconds as integer + expiration
	if (exp > 0) {
		/* Add expiration (exp) claim */
		if (cJSON_AddNumberToObjectCS(jwt_pay, "exp", exp) == NULL) {
			err = -ENOMEM;
		}
	}

	if (err == 0) {
		pay_str = cJSON_PrintUnformatted(jwt_pay);
	}

	cJSON_Delete(jwt_pay);
	jwt_pay = NULL;

	return pay_str;
}


//---------------------------------------------------------------------------//
static char *unsigned_jwt_create(struct app_jwt_data *const jwt)
{
	uint64_t exp_ts_s = 0;

	char *hdr_str, *pay_str;
	char *hdr_b64, *pay_b64;
	char *unsigned_jwt;

	/* Create the header */
	hdr_str = jwt_header_create(jwt->alg, jwt->key);
	if (!hdr_str) {
		LOG_ERR("Failed to create JWT JSON payload");
		return NULL;
	}

	/* Convert header JSON string to base64 URL */
	hdr_b64 = convert_str_to_b64_url(hdr_str);

	cJSON_free(hdr_str);
	hdr_str = NULL;

	if (!hdr_b64) {
		LOG_ERR("Failed to convert header string to base64");
		return NULL;
	}

	/* Get expiration time stamp */
	if (jwt->exp_delta_s > 0) {
#if defined(CONFIG_TIME_DATE)
		int err = date_time_now(&exp_ts_s);
		if (!err) {
			LOG_WARN("Unable to get a valid timestamp, using value 0s");
			exp_ts_s = 0;
		}
#endif /* CONFIG_TIME_DATE */
		exp_ts_s += jwt->exp_delta_s;
	}

	/* Create the payload */
	pay_str = jwt_payload_create(jwt->subject, jwt->audience, exp_ts_s);
	if (!pay_str) {
		LOG_ERR("Failed to create JWT JSON payload");
		return NULL;
	}

	/* Convert payload JSON string to base64 URL */
	pay_b64 = convert_str_to_b64_url(pay_str);

	cJSON_free(pay_str);
	pay_str = NULL;

	if (!pay_b64) {
		LOG_ERR("Failed to convert payload string to base64");
		return NULL;
	}

	/* Create the base64 URL data to be signed */
	unsigned_jwt = jwt_header_payload_combine(hdr_b64, pay_b64);

	free(pay_b64);
	pay_b64 = NULL;

	return unsigned_jwt;
}

//---------------------------------------------------------------------------//
static int jwt_signature_get(const uint32_t user_key_id, const int sec_tag, const char *const jwt,
			     char *const sig_buf, size_t sig_sz)
{
	int err;
	size_t o_len;
	uint8_t sig_raw[ECDSA_SHA_256_SIG_SZ];

	/* Use Application IAK key for signing the JWT */
	// ignore the provided key_id and sec_tag
    (void) sec_tag;
	(void) user_key_id;
	err = crypto_init();
	if (err) {
		LOG_ERR("Failed to initialize PSA Crypto, error: %d", err);
		return -ENOMEM;
	}

	err = hash_and_sign_message(IAK_APPLICATION_GEN1, jwt, strlen(jwt), sig_raw, ECDSA_SHA_256_SIG_SZ, &o_len);
	if (err) {
		LOG_ERR("Failed to sign message : %d", err);
		return -EACCES;
	}

	err = crypto_finish();
	if (err) {
		LOG_ERR("Failed to sign message : %d", err);
		return -ENOMEM;
	}

	/* Convert signature to base64 URL */
	err = base64_encode(sig_buf, sig_sz, &o_len, sig_raw, sizeof(sig_raw));
	if (err) {
		LOG_ERR("base64_encode failed, error: %d", err);
		return -EIO;
	}

	base64_url_format(sig_buf);
	return 0;
}


//---------------------------------------------------------------------------//
static int jwt_signature_append(const char *const unsigned_jwt, const char *const sig,
				char *const jwt_buf, size_t jwt_sz)
{
	int err = 0;

	/* Get the size of the final, signed JWT: +1 for '.' and null-terminator */
	size_t final_sz = strlen(unsigned_jwt) + 1 + strlen(sig) + 1;

	if (final_sz > jwt_sz) {
		/* Provided buffer is too small */
		err = -E2BIG;
	}

	/* JWT final form:
	 * <base64_header>.<base64_payload>.<base64_signature>
	 */
	if (err == 0) {
		int ret = snprintk(jwt_buf, jwt_sz, "%s.%s", unsigned_jwt, sig);

		if ((ret < 0) || (ret >= jwt_sz)) {
			err = -ETXTBSY;
		}
	}

	return err;
}


//---------------------------------------------------------------------------//
int user_app_jwt_generate(struct app_jwt_data *const jwt)
{
	if (NULL == jwt) {
		return -EINVAL;
	}

	if ((jwt->jwt_buf) && (jwt->jwt_sz == 0)) {
		return -EMSGSIZE;
	}

	int err = 0;
	char *unsigned_jwt;
	uint8_t jwt_sig[B64_SIG_SZ];

	/* Create the JWT to be signed */
	unsigned_jwt = unsigned_jwt_create(jwt);
	if (!unsigned_jwt) {
		LOG_ERR("Failed to create JWT to be signed");
		return -EIO;
	}

	/* Get the signature of the unsigned JWT */
	err = jwt_signature_get(jwt->key, jwt->sec_tag, unsigned_jwt, jwt_sig, sizeof(jwt_sig));
	if (err) {
		LOG_ERR("Failed to get JWT signature, error: %d", err);
		free(unsigned_jwt);
		return -ENOEXEC;
	}

	/* Append the signature, creating the complete JWT */
	err = jwt_signature_append(unsigned_jwt, jwt_sig, jwt->jwt_buf, jwt->jwt_sz);

	free(unsigned_jwt);

	return err;
}


//---------------------------------------------------------------------------//
int user_app_jwt_get_uuid(char *uuid_buffer, const size_t uuid_buffer_size)
{
	if ((NULL == uuid_buffer) || (uuid_buffer_size < NRF_UUID_V4_STR_LEN)) {
		/* bad parameter */
		return -EINVAL;
	}

	uint32_t uuid_words[UUID_BINARY_WORD_SZ];
	if (0 != ssf_device_info_get_uuid(uuid_words, UUID_BINARY_WORD_SZ * sizeof(uint32_t))) {
		/* couldn't read data */
		return -ENXIO;
	}

	return bytes_to_uuid_str(uuid_words, UUID_BINARY_WORD_SZ * sizeof(uint32_t), uuid_buffer, uuid_buffer_size);
}