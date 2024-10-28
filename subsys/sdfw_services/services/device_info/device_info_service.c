/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** .. include_startingpoint_device_info_source_rst */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <sdfw/sdfw_services/device_info_service.h>

#include "device_info_service_decode.h"
#include "device_info_service_encode.h"
#include "device_info_service_types.h"
#include <sdfw/sdfw_services/ssf_client.h>
#include "ssf_client_os.h"

#define UUID_BYTES_LENGTH   16UL

SSF_CLIENT_LOG_REGISTER(device_info_service, CONFIG_SSF_DEVICE_INFO_SERVICE_LOG_LEVEL);

SSF_CLIENT_SERVICE_DEFINE(device_info_srvc, DEVICE_INFO, cbor_encode_device_info_req,
			  cbor_decode_device_info_resp);


int ssf_device_info_get_uuid(uint32_t* uuid_words, size_t uuid_words_count)
{
	int err = -1; /* initialize with negative value for error */

	if ((NULL != uuid_words) && (uuid_words_count >= UUID_BYTES_LENGTH)) {
		// valid parameters
		int ret = -1; /* initialize with negative value for error */
		const uint8_t *rsp_pkt; /* For freeing response packet after copying rsp_str. */

		struct device_info_service_read_req uuid_read_req = {
				.device_info_service_read_req_target.device_info_target_choice = device_info_target_UUID_c,
			};

		struct device_info_req read_req = {
				.device_info_req_msg_choice = device_info_req_msg_device_info_service_read_req_m_c,
				.device_info_req_msg_device_info_service_read_req_m = uuid_read_req
		};

		struct device_info_resp read_resp;

		err = ssf_client_send_request(&device_info_srvc, &read_req, &read_resp, &rsp_pkt);
		if (err != 0) {
			return err;
		}

		struct device_info_service_read_resp uuid_read_resp = read_resp.device_info_resp_msg_device_info_service_read_resp_m;
		if (read_resp.device_info_resp_msg_choice == device_info_resp_msg_device_info_service_read_resp_m_c) {
			if(device_info_target_UUID_c == uuid_read_resp.device_info_service_read_resp_target.device_info_target_choice) {
				ret = read_resp.device_info_resp_msg_device_info_service_read_resp_m.device_info_service_read_resp_status.device_info_status_choice;
				if (device_info_status_SUCCESS_c != ret) {
					ssf_client_decode_done(rsp_pkt);
					return ret;
				}
			} else {
				// not a read UUID response
				ssf_client_decode_done(rsp_pkt);
				return device_info_status_INTERNAL_ERROR_c;
			}

		} else {
			// not a read response
			ssf_client_decode_done(rsp_pkt);
			return device_info_status_INTERNAL_ERROR_c;
		}

		size_t uuid_words_len = uuid_read_resp.device_info_service_read_resp_data_uint_count;
		memcpy(uuid_words, uuid_read_resp.device_info_service_read_resp_data_uint, uuid_words_len * sizeof(uint32_t));

		ssf_client_decode_done(rsp_pkt);
	} else {
		// invalid parameters
		err = -1;

	}
	return err;
}

/** .. include_endpoint_device_info_source_rst */
