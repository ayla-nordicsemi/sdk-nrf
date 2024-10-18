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

SSF_CLIENT_SERVICE_DEFINE(device_info_srvc, DEVICE_INFO, cbor_encode_device_info_service_req,
			  cbor_decode_device_info_service_rsp);

int ssf_device_info(char *str_in, char *str_out, size_t str_out_size)
{
	int err;

	return 0;
}
/** .. include_endpoint_device_info_source_rst */
