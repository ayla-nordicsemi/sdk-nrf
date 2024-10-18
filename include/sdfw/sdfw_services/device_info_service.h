/*
 * Copyright (c) 2024 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#ifndef DEVICE_INFO_SERVICE_H__
#define DEVICE_INFO_SERVICE_H__

#include <stddef.h>

#include <sdfw/sdfw_services/ssf_errno.h>

/** .. include_startingpoint_device_info_header_rst */
/**
 * @brief       todo.
 *
 * @param[out]  str_out Local buffer for copying the UUID c string into.
 * @param[in]   str_out_size Size of local buffer.
 *
 * @return      0 on success, otherwise a negative errno.
 */
int ssf_device_info(char *str_out, size_t str_out_size);
/** .. include_endpoint_device_info_header_rst */

#endif /* DEVICE_INFO_SERVICE_H__ */
