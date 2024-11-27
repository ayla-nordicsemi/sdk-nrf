/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

/*
 * Generated using zcbor version 0.9.0
 * https://github.com/NordicSemiconductor/zcbor
 * Generated with a --default-max-qty of 3
 */

#ifndef DEVICE_INFO_SERVICE_TYPES_H__
#define DEVICE_INFO_SERVICE_TYPES_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Which value for --default-max-qty this file was created with.
 *
 *  The define is used in the other generated file to do a build-time
 *  compatibility check.
 *
 *  See `zcbor --help` for more information about --default-max-qty
 */
#define DEFAULT_MAX_QTY 3

struct device_info_target_r {
	enum {
		device_info_target_UUID_c = 0,
		device_info_target_TYPE_c = 1,
		device_info_target_TESTIMPRINT_c = 2,
		device_info_target_PARTNO_c = 3,
		device_info_target_HWREVISION_c = 4,
		device_info_target_PRODUCTIONREVISION_c = 5,
	} device_info_target_choice;
};

struct device_info_status_r {
	enum {
		device_info_status_SUCCESS_c = 0,
		device_info_status_INTERNAL_ERROR_c = 16781313,
		device_info_status_UNPROGRAMMED_c = 16781314,
	} device_info_status_choice;
};


struct device_info_service_read_req {
	struct device_info_target_r device_info_service_read_req_target;
};

struct device_info_service_read_resp {
	struct device_info_target_r device_info_service_read_resp_target;
	struct device_info_status_r device_info_service_read_resp_status;
	uint32_t device_info_service_read_resp_data_uint[8];
	size_t device_info_service_read_resp_data_uint_count;
};

struct device_info_service_write_req {
	struct device_info_target_r device_info_service_write_req_target;
	uint32_t device_info_service_write_req_data_uint[8];
	size_t device_info_service_write_req_data_uint_count;
};

struct device_info_service_write_resp {
	struct device_info_target_r device_info_service_write_resp_target;
	struct device_info_status_r device_info_service_write_resp_status;
};

struct device_info_req {
	union {
		struct device_info_service_read_req device_info_req_msg_device_info_service_read_req_m;
		struct device_info_service_write_req device_info_req_msg_device_info_service_write_req_m;
	};
	enum {
		device_info_req_msg_device_info_service_read_req_m_c,
		device_info_req_msg_device_info_service_write_req_m_c,
	} device_info_req_msg_choice;
};

struct device_info_resp {
	union {
		struct device_info_service_read_resp device_info_resp_msg_device_info_service_read_resp_m;
		struct device_info_service_write_resp device_info_resp_msg_device_info_service_write_resp_m;
	};
	enum {
		device_info_resp_msg_device_info_service_read_resp_m_c,
		device_info_resp_msg_device_info_service_write_resp_m_c,
	} device_info_resp_msg_choice;
};

#ifdef __cplusplus
}
#endif

#endif /* DEVICE_INFO_SERVICE_TYPES_H__ */