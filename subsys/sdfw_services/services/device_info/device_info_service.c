/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/** .. include_startingpoint_echo_source_rst */
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <sdfw/sdfw_services/device_info_service.h>

#include "device_info_service_decode.h"
#include "device_info_service_encode.h"
#include "device_info_service_types.h"
#include <sdfw/sdfw_services/ssf_client.h>
#include "ssf_client_os.h"

SSF_CLIENT_LOG_REGISTER(device_info_service, CONFIG_SSF_DEVICE_INFO_SERVICE_LOG_LEVEL);

SSF_CLIENT_SERVICE_DEFINE(device_info_srvc, DEVICE_INFO, cbor_encode_device_info_req,
			  cbor_decode_device_info_resp);

enum {
	UUID = 0,
	HWVERSION,
} device_info_target;

enum {
	SUCCESS = 0,
	INTERNAL_ERROR = 0x1001001,
	UNPROGRAMMED = 0x1001002,
} device_info_status;


int ssf_device_info_get_uuid(char *str_out, size_t str_out_size)
{
	int err = -1; /* initialize with negative value for error */
	int ret = -1; /* initialize with negative value for error */
	const uint8_t *rsp_pkt; /* For freeing response packet after copying rsp_str. */

	struct device_info_service_read_req uuid_read_req = { 
			.device_info_service_read_req_target = UUID,
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
		if(UUID == uuid_read_resp.device_info_service_read_resp_target) {
			ret = read_resp.device_info_resp_msg_device_info_service_read_resp_m.device_info_service_read_resp_status;
			if (SUCCESS != ret) {
				ssf_client_decode_done(rsp_pkt);
				return ret;
			}
		} else {
			// not a read UUID response
			ssf_client_decode_done(rsp_pkt);
			return INTERNAL_ERROR;
		}

	} else {
		// not a read response
		ssf_client_decode_done(rsp_pkt);
		return INTERNAL_ERROR;
	}


	const uint8_t *uuid_str;
	size_t uuid_str_len;

	uuid_str = uuid_read_resp.device_info_service_read_resp_data.value;
	uuid_str_len = uuid_read_resp.device_info_service_read_resp_data.len;
	memcpy(str_out, uuid_str, SSF_CLIENT_MIN(str_out_size, uuid_str_len));

	ssf_client_decode_done(rsp_pkt);

	return 0;
}

/** .. include_endpoint_device_info_source_rst */
