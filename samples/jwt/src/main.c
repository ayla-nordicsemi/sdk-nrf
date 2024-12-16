/*
 * Copyright (c) 2024 Nordic Semiconductor ASA.
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <app_jwt.h>

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(jwt_sample, CONFIG_JWT_SAMPLE_LOG_LEVEL);

#define JWT_AUDIENCE_STR "JSON web token for demonstration"

static int jwt_generate(uint32_t exp_time_s, char *out_buffer, const size_t out_buffer_size)
{
	if (!out_buffer || !out_buffer_size) {
		return -EINVAL;
	}

	int err = -EINVAL;

	struct app_jwt_data jwt = {.sec_tag = 0,
				   .key_type = 0,
				   .alg = JWT_ALG_TYPE_ES256,
				   .add_keyid_to_header = true,
				   .add_timestamp = true,
				   .jwt_buf = out_buffer,
				   .jwt_sz = out_buffer_size};

	if (exp_time_s > APP_JWT_VALID_TIME_S_MAX) {
		jwt.validity_s = APP_JWT_VALID_TIME_S_MAX;
	} else if (exp_time_s == 0) {
		jwt.validity_s = APP_JWT_VALID_TIME_S_DEF;
	} else {
		jwt.validity_s = exp_time_s;
	}

	/* Subject: format: "user_defined_string" , we use uuid as subject */
	char device_uuid_str[APP_JWT_UUID_V4_STR_LEN] = {0};

	if (0 != app_jwt_get_uuid(device_uuid_str, APP_JWT_UUID_V4_STR_LEN)) {
		return -ENXIO;
	}

	jwt.subject = device_uuid_str;

	/* Issuer: format: <hardware_id>.<sub> */
	char token_issuer_str[APP_JWT_CLAIM_MAX_SIZE] = {0};

	snprintf(token_issuer_str, APP_JWT_CLAIM_MAX_SIZE, "%s.%s", CONFIG_BOARD, jwt.subject);
	token_issuer_str[APP_JWT_CLAIM_MAX_SIZE - 1] = '\0';

	jwt.issuer = token_issuer_str;

	/* Json Token ID: format: <hardware_id>.<16-random_bytes> */
	char json_token_id_str[APP_JWT_CLAIM_MAX_SIZE] = {0};

	snprintf(json_token_id_str, APP_JWT_CLAIM_MAX_SIZE, "%s.%s", CONFIG_BOARD,
		"fe02589ac0875fcb3097ce92afdb210e");
	json_token_id_str[APP_JWT_CLAIM_MAX_SIZE - 1] = '\0';

	jwt.json_token_id = json_token_id_str;

	/* Audience: format: "user_defined_string" */
	char audience_str[APP_JWT_CLAIM_MAX_SIZE] = {0};

	snprintf(audience_str, APP_JWT_CLAIM_MAX_SIZE, "%s", JWT_AUDIENCE_STR);
	audience_str[APP_JWT_CLAIM_MAX_SIZE - 1] = '\0';

	jwt.audience = audience_str;

	err = app_jwt_generate(&jwt);

	return err;
}

int main(void)
{
	LOG_INF("Application JWT sample (%s)", CONFIG_BOARD);

	char jwt_str[APP_JWT_STR_MAX_LEN] = {0};

	int ret = jwt_generate(APP_JWT_VALID_TIME_S_MAX, jwt_str, APP_JWT_STR_MAX_LEN);

	if (ret == 0) {
		LOG_INF("JWT(%d) :", strlen(jwt_str));
		LOG_INF("%s", jwt_str);

	} else {
		LOG_ERR("jwt_generate error : %d", ret);
	}

	return 0;
}
