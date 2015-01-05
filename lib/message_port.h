/* vi: set et sw=4 ts=4 cino=t0,(0: */
/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of message-port.
 *
 * Copyright (c) 2015 Samsung Electronics
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#ifndef __TIZEN_APPFW_MESSAGE_PORT_H__
#define __TIZEN_APPFW_MESSAGE_PORT_H__

#ifdef __GNUC__
#    ifndef EXPORT_API
#        define EXPORT_API __attribute__((visibility("default")))
#    endif
#else
#    define EXPORT_API
#endif

#include <stdbool.h>
#include <bundle.h>
#include <message-port.h>
#include <tizen_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/* For Tizen 2.3 Native API compatibility*/
typedef enum
{
    MESSAGE_PORT_ERROR_NONE = MESSAGEPORT_ERROR_NONE,                                     /**< Successful */
    MESSAGE_PORT_ERROR_IO_ERROR = MESSAGEPORT_ERROR_IO_ERROR,                             /**< Internal I/O error */
    MESSAGE_PORT_ERROR_OUT_OF_MEMORY = MESSAGEPORT_ERROR_OUT_OF_MEMORY,                   /**< Out of memory */
    MESSAGE_PORT_ERROR_INVALID_PARAMETER = MESSAGEPORT_ERROR_INVALID_PARAMETER,           /**< Invalid parameter */
    MESSAGE_PORT_ERROR_PORT_NOT_FOUND = MESSAGEPORT_ERROR_MESSAGEPORT_NOT_FOUND,          /**< The message port of the remote application is not found */
    MESSAGE_PORT_ERROR_CERTIFICATE_NOT_MATCH = MESSAGEPORT_ERROR_CERTIFICATE_NOT_MATCH,   /**< The remote application is not signed with the same certificate */
    MESSAGE_PORT_ERROR_MAX_EXCEEDED = MESSAGEPORT_ERROR_MAX_EXCEEDED,                     /**< The size of the message has exceeded the maximum limit */
    MESSAGE_PORT_ERROR_RESOURCE_UNAVAILABLE = MESSAGEPORT_ERROR_RESOURCE_UNAVAILABLE      /**< Resource is temporarily unavailable */
} message_port_error_e;

typedef void (*message_port_message_cb)(int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *messagem, void *user_data);
typedef void (*message_port_trusted_message_cb)(int trusted_local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *user_data);

EXPORT_API int
message_port_register_local_port (const char *local_port, message_port_message_cb callback, void *user_data);

EXPORT_API int
message_port_register_trusted_local_port (const char *trusted_local_port, message_port_trusted_message_cb callback, void *user_data);

EXPORT_API int
message_port_unregister_local_port (int local_port_id);

EXPORT_API int
message_port_unregister_trusted_local_port (int trusted_local_port_id);

EXPORT_API int
message_port_check_remote_port (const char *remote_app_id, const char *remote_port, bool *exist);

EXPORT_API int
message_port_check_trusted_remote_port (const char *remote_app_id, const char *remote_port, bool *exist);

EXPORT_API int
message_port_send_message (const char *remote_app_id, const char *remote_port, bundle *message);

EXPORT_API int
message_port_send_trusted_message (const char *remote_app_id, const char *remote_port, bundle *message);

EXPORT_API int
message_port_send_message_with_local_port (const char *remote_app_id, const char *remote_port, bundle *message, int local_port_id);

EXPORT_API int
message_port_send_trusted_message_with_local_port (const char *remote_app_id, const char *remote_port, bundle *message, int local_port_id);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_MESSAGE_PORT_H__ */
