/*
 * Generated using zcbor version 0.9.0
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "zcbor_decode.h"
#include "device_info_service_decode.h"
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

static bool decode_device_info_service_read_resp(zcbor_state_t *state, struct device_info_service_read_resp *result);
static bool decode_device_info_service_write_resp(zcbor_state_t *state, struct device_info_service_write_resp *result);
static bool decode_device_info_resp(zcbor_state_t *state, struct device_info_resp *result);
static bool decode_device_info_service_read_req(zcbor_state_t *state, struct device_info_service_read_req *result);
static bool decode_device_info_service_write_req(zcbor_state_t *state, struct device_info_service_write_req *result);
static bool decode_device_info_req(zcbor_state_t *state, struct device_info_req *result);

static bool decode_device_info_service_read_req(
		zcbor_state_t *state, struct device_info_service_read_req *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((((zcbor_uint32_expect(state, (2))))
	&& ((zcbor_uint32_decode(state, (&(*result).device_info_service_read_req_target)))))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_device_info_service_write_req(
		zcbor_state_t *state, struct device_info_service_write_req *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((((zcbor_uint32_expect(state, (2))))
	&& ((zcbor_uint32_decode(state, (&(*result).device_info_service_write_req_target))))
	&& ((zcbor_tstr_decode(state, (&(*result).device_info_service_write_req_data)))))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_device_info_req(
		zcbor_state_t *state, struct device_info_req *result)
{
	zcbor_log("%s\r\n", __func__);
	bool int_res;

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_union_start_code(state) && (int_res = ((((decode_device_info_service_read_req(state, (&(*result).device_info_req_msg_device_info_service_read_req_m)))) && (((*result).device_info_req_msg_choice = device_info_req_msg_device_info_service_read_req_m_c), true))
	|| (zcbor_union_elem_code(state) && (((decode_device_info_service_write_req(state, (&(*result).device_info_req_msg_device_info_service_write_req_m)))) && (((*result).device_info_req_msg_choice = device_info_req_msg_device_info_service_write_req_m_c), true)))), zcbor_union_end_code(state), int_res)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}



int cbor_decode_device_info_req(
		const uint8_t *payload, size_t payload_len,
		struct device_info_req *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_device_info_req, sizeof(states) / sizeof(zcbor_state_t), 1);
}
static bool decode_device_info_service_read_resp(
		zcbor_state_t *state, struct device_info_service_read_resp *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((((zcbor_uint32_expect(state, (3))))
	&& ((zcbor_uint32_decode(state, (&(*result).device_info_service_read_resp_target))))
	&& ((zcbor_uint32_decode(state, (&(*result).device_info_service_read_resp_status))))
	&& ((zcbor_tstr_decode(state, (&(*result).device_info_service_read_resp_data)))))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_device_info_service_write_resp(
		zcbor_state_t *state, struct device_info_service_write_resp *result)
{
	zcbor_log("%s\r\n", __func__);

	bool res = (((((zcbor_uint32_expect(state, (3))))
	&& ((zcbor_uint32_decode(state, (&(*result).device_info_service_write_resp_target))))
	&& ((zcbor_uint32_decode(state, (&(*result).device_info_service_write_resp_status)))))));

	log_result(state, res, __func__);
	return res;
}

static bool decode_device_info_resp(
		zcbor_state_t *state, struct device_info_resp *result)
{
	zcbor_log("%s\r\n", __func__);
	bool int_res;

	bool res = (((zcbor_list_start_decode(state) && ((((zcbor_union_start_code(state) && (int_res = ((((decode_device_info_service_read_resp(state, (&(*result).device_info_resp_msg_device_info_service_read_resp_m)))) && (((*result).device_info_resp_msg_choice = device_info_resp_msg_device_info_service_read_resp_m_c), true))
	|| (zcbor_union_elem_code(state) && (((decode_device_info_service_write_resp(state, (&(*result).device_info_resp_msg_device_info_service_write_resp_m)))) && (((*result).device_info_resp_msg_choice = device_info_resp_msg_device_info_service_write_resp_m_c), true)))), zcbor_union_end_code(state), int_res)))) || (zcbor_list_map_end_force_decode(state), false)) && zcbor_list_end_decode(state))));

	log_result(state, res, __func__);
	return res;
}



int cbor_decode_device_info_resp(
		const uint8_t *payload, size_t payload_len,
		struct device_info_resp *result,
		size_t *payload_len_out)
{
	zcbor_state_t states[4];

	return zcbor_entry_function(payload, payload_len, (void *)result, payload_len_out, states,
		(zcbor_decoder_t *)decode_device_info_resp, sizeof(states) / sizeof(zcbor_state_t), 1);
}
