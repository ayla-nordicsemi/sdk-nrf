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
#include <zcbor_common.h>

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


struct device_info_service_read_req {
	uint32_t device_info_service_read_req_target;
};

struct device_info_service_write_req {
	uint32_t device_info_service_write_req_target;
	struct zcbor_string device_info_service_write_req_data;
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

struct device_info_service_read_resp {
	uint32_t device_info_service_read_resp_target;
	uint32_t device_info_service_read_resp_status;
	struct zcbor_string device_info_service_read_resp_data;
};

struct device_info_service_write_resp {
	uint32_t device_info_service_write_resp_target;
	uint32_t device_info_service_write_resp_status;
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
