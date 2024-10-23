/*
 * Generated using zcbor version 0.9.0
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_encode.h"
#include "device_info_service_encode.h"
#include "zcbor_print.h"

#if DEFAULT_MAX_QTY != 3
#error "The type file was generated with a different default_max_qty than this file"
#endif

#define log_result(state, result, func) do { \
	if (!result) { \
		zcbor_trace_file(state); \
		zcbor_log("%s error: %s\r\n", func, zcbor_error_str(zcbor_peek_error(state))); \
	} else { \
		zcbor_log("%s success\r\n", func); \
	} \
} while(0)

static bool encode_device_info_service_read_resp(zcbor_state_t *state, const struct device_info_service_read_resp *input);
static bool encode_device_info_service_write_resp(zcbor_state_t *state, const struct device_info_service_write_resp *input);
static bool encode_device_info_resp(zcbor_state_t *state, const struct device_info_resp *input);
static bool encode_device_info_service_read_req(zcbor_state_t *state, const struct device_info_service_read_req *input);
static bool encode_device_info_service_write_req(zcbor_state_t *state, const struct device_info_service_write_req *input);
static bool encode_device_info_req(zcbor_state_t *state, const struct device_info_req *input);

static bool encode_device_info_service_read_req(
		zcbor_state_t *state, const struct device_info_service_read_req *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((((zcbor_uint32_put(state, (2))))
	&& ((zcbor_uint32_encode(state, (&(*input).device_info_service_read_req_target)))))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_device_info_service_write_req(
		zcbor_state_t *state, const struct device_info_service_write_req *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((((zcbor_uint32_put(state, (2))))
	&& ((zcbor_uint32_encode(state, (&(*input).device_info_service_write_req_target))))
	&& ((zcbor_tstr_encode(state, (&(*input).device_info_service_write_req_data)))))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_device_info_req(
		zcbor_state_t *state, const struct device_info_req *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 3) && ((((((*input).device_info_req_msg_choice == device_info_req_msg_device_info_service_read_req_m_c) ? ((encode_device_info_service_read_req(state, (&(*input).device_info_req_msg_device_info_service_read_req_m))))
	: (((*input).device_info_req_msg_choice == device_info_req_msg_device_info_service_write_req_m_c) ? ((encode_device_info_service_write_req(state, (&(*input).device_info_req_msg_device_info_service_write_req_m))))
	: false)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 3))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_device_info_req(
		uint8_t *payload, size_t payload_len,
		const struct device_info_req *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_device_info_req, sizeof(states) / sizeof(zcbor_state_t), 1);
}

static bool encode_device_info_service_read_resp(
		zcbor_state_t *state, const struct device_info_service_read_resp *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((((zcbor_uint32_put(state, (3))))
	&& ((zcbor_uint32_encode(state, (&(*input).device_info_service_read_resp_target))))
	&& ((zcbor_uint32_encode(state, (&(*input).device_info_service_read_resp_status))))
	&& ((zcbor_tstr_encode(state, (&(*input).device_info_service_read_resp_data)))))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_device_info_service_write_resp(
		zcbor_state_t *state, const struct device_info_service_write_resp *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((((zcbor_uint32_put(state, (3))))
	&& ((zcbor_uint32_encode(state, (&(*input).device_info_service_write_resp_target))))
	&& ((zcbor_uint32_encode(state, (&(*input).device_info_service_write_resp_status)))))));

	log_result(state, res, __func__);
	return res;
}

static bool encode_device_info_resp(
		zcbor_state_t *state, const struct device_info_resp *input)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((zcbor_list_start_encode(state, 4) && ((((((*input).device_info_resp_msg_choice == device_info_resp_msg_device_info_service_read_resp_m_c) ? ((encode_device_info_service_read_resp(state, (&(*input).device_info_resp_msg_device_info_service_read_resp_m))))
	: (((*input).device_info_resp_msg_choice == device_info_resp_msg_device_info_service_write_resp_m_c) ? ((encode_device_info_service_write_resp(state, (&(*input).device_info_resp_msg_device_info_service_write_resp_m))))
	: false)))) || (zcbor_list_map_end_force_encode(state), false)) && zcbor_list_end_encode(state, 4))));

	log_result(state, res, __func__);
	return res;
}



int cbor_encode_device_info_resp(
		uint8_t *payload, size_t payload_len,
		const struct device_info_resp *input,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	return zcbor_entry_function(payload, payload_len, (void *)input, payload_len_out, states,
		(zcbor_decoder_t *)encode_device_info_resp, sizeof(states) / sizeof(zcbor_state_t), 1);
}
