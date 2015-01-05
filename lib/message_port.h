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
#	ifndef EXPORT_API
#		define EXPORT_API __attribute__((visibility("default")))
#	endif
#else
#	define EXPORT_API
#endif

#include <stdbool.h>
#include <bundle.h>
#include <message-port.h>
#include <tizen_error.h>

#ifdef __cplusplus
extern "C" {
#endif

/* For Tizen 2.3 Native API compatibility*/
#define message_port_error_e messageport_error_e
#define MESSAGE_PORT_ERROR_NONE MESSAGEPORT_ERROR_NONE
#define MESSAGE_PORT_ERROR_IO_ERROR MESSAGEPORT_ERROR_IO_ERROR
#define MESSAGE_PORT_ERROR_OUT_OF_MEMORY MESSAGEPORT_ERROR_OUT_OF_MEMORY
#define MESSAGE_PORT_ERROR_INVALID_PARAMETER MESSAGEPORT_ERROR_INVALID_PARAMETER
#define MESSAGE_PORT_ERROR_PORT_NOT_FOUND MESSAGEPORT_ERROR_MESSAGEPORT_NOT_FOUND
#define MESSAGE_PORT_ERROR_CERTIFICATE_NOT_MATCH MESSAGEPORT_ERROR_CERTIFICATE_NOT_MATCH
#define MESSAGE_PORT_ERROR_MAX_EXCEEDED MESSAGEPORT_ERROR_MAX_EXCEEDED
#define MESSAGE_PORT_ERROR_RESOURCE_UNAVAILABLE MESSAGEPORT_ERROR_RESOURCE_UNAVAILABLE

#define message_port_trusted_message_cb messageport_message_cb
#define message_port_register_local_port messageport_register_local_port_with_user_data
#define message_port_register_trusted_local_port messageport_register_trusted_local_port_with_user_data
#define message_port_check_remote_port messageport_check_remote_port
#define message_port_check_trusted_remote_port messageport_check_trusted_remote_port
#define message_port_send_message messageport_send_message
#define message_port_send_trusted_message messageport_send_trusted_message

/**
 * @brief Called when a message is received.
 * @details The function is called when a message is received from the remote application.
 *
 * @since_tizen 2.3
 *
 * @remarks		 You can keep @a message using bundle_dup(). @n
 *				@a remote_port will be set only if the remote application sends a message with its port information using message_port_send_message_with_local_port(), otherwise it is @c NULL. @n
 *				When message is sent from remote application by message_port_send_message_with_local_port() in bidirectional communication, trusted_remote_port is used to check whether remote port is trusted port or not.
 *				This callback is called only in the main thread.
 *
 * @param[in] local_port_id		The local message port ID returned by message_port_register_local_port()
 * @param[in] remote_app_id		The ID of the remote application that sent this message
 * @param[in] remote_port		The name of the remote message port
 * @param[in]	trusted_remote_port		If @c true the remote port is a trusted port, otherwise if @c false it is not
 * @param[in] message			The message passed from the remote application
 * @param[in] user_data			The user data passed from the register function
 * @pre			Either message_port_send_message() or message_port_send_message_with_local_port() from the remote application will invoke this function if you register it using message_port_register_local_port().
 * @see			message_port_register_local_port()
 * @see			message_port_unregister_local_port()
 * @see			message_port_send_message()
 * @see			message_port_send_message_with_local_port()
 */
typedef void (*message_port_message_cb)(int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *messagem, void *user_data);
/**
 * @brief Unregisters the local message port.
 * @details This method unregisters the callback function with the specified local port ID.
 *
 * @since_tizen 2.3
 *
 * @param[in] local_port_id		The local message port ID
 * @return		0 on success,
 *				otherwise a negative error value
 * @retval		#MESSAGE_PORT_ERROR_NONE				Successful
 * @retval		#MESSAGE_PORT_ERROR_INVALID_PARAMETER	The specified @a local_port_id is not positive
 * @retval		#MESSAGE_PORT_ERROR_PORT_NOT_FOUND		The specified @a local_port_id cannot be found
 * @retval		#MESSAGE_PORT_ERROR_OUT_OF_MEMORY		Out of memory
 * @retval		#MESSAGE_PORT_ERROR_IO_ERROR			Internal I/O error
 * @see			message_port_register_local_port()
 */
EXPORT_API int message_port_unregister_local_port(int local_port_id);
/**
 * @brief		Registers the trusted local message port.
 * @details		This method unregisters the callback function with the specified local port ID. @n
 *				It allows communications only if the applications are signed with the same certificate which is uniquely assigned to the developer.
 *
 * @since_tizen 2.3
 *
 * @param[in] trusted_local_port_id		The trusted local message port ID
 * @return		@c 0 on success,
 *				otherwise a negative error value
 * @retval		#MESSAGE_PORT_ERROR_NONE				Successful
 * @retval		#MESSAGE_PORT_ERROR_INVALID_PARAMETER	The specified @a trusted_local_port_id is not positive
 * @retval		#MESSAGE_PORT_ERROR_PORT_NOT_FOUND		The specified @a trusted_local_port_id cannot be found
 * @retval		#MESSAGE_PORT_ERROR_OUT_OF_MEMORY		Out of memory
 * @retval		#MESSAGE_PORT_ERROR_IO_ERROR			Internal I/O error
 * @see			message_port_register_trusted_local_port()
 */
EXPORT_API int message_port_unregister_trusted_local_port(int trusted_local_port_id);

/**
 * @brief		Sends a message with local port information to the message port of a remote application.
 * @details		This method is used for bidirectional communication.
 *
 * @since_tizen 2.3
 *
 * @remarks		You must releas @a message using bundle_free() after sending the message.
 * @param[in]	remote_app_id		The ID of the remote application
 * @param[in]	remote_port			The name of the remote message port
 * @param[in]	message				The message to be passed to the remote application, the recommended message size is under 4KB
 * @param[in]	local_port_id		The message port ID returned by message_port_register_local_port() or message_port_register_trusted_local_port()
 * @return		@c 0 on success,
 *				otherwise a negative error value
 * @retval		#MESSAGE_PORT_ERROR_NONE					Successful
 * @retval		#MESSAGE_PORT_ERROR_INVALID_PARAMETER		The specified @a remote_app_id, @a remote_port or @a message is @c NULL and
													The specified @a local_port_id is not positive
 * @retval		#MESSAGE_PORT_ERROR_OUT_OF_MEMORY			Out of memory
 * @retval		#MESSAGE_PORT_ERROR_PORT_NOT_FOUND			The port of the local or remote application cannot be found
 * @retval		#MESSAGE_PORT_ERROR_MAX_EXCEEDED			The size of the message has exceeded the maximum limit
 * @retval		#MESSAGE_PORT_ERROR_IO_ERROR				Internal I/O error
 * @post		It invokes message_port_message_cb() on the remote application.
 * @see			message_port_message_cb()
 * @see			message_port_register_local_port()
 * @see			message_port_unregister_local_port()
 *
 * @code
 * #include <message_port.h>
 *
 * static void
 * message_port_receive_cb(int local_port_id, const char *remote_app_id, const char *remote_port, bundle *message)
 * {
 * }
 *
 * int main(int argc, char *argv[])
 * {
 *   bundle *b = bundle_create();
 *   bundle_add(b, "key1", "value1");
 *   bundle_add(b, "key2", "value2");
 *
 *   int local_port_id = message_port_register_local_port("HelloPort", message_port_receive_cb);
 *
 *   int ret = message_port_send_message_with_local_port("0123456789.BasicApp", "BasicAppPort", b, local_port_id);
 *
 *   bundle_free(b);
 * }
 * @endcode
 */
EXPORT_API int message_port_send_message_with_local_port(const char *remote_app_id, const char *remote_port, bundle *message, int local_port_id);

/**
 * @brief		Sends a trusted message with local port information to the message port of a remote application.
 * @details		This method is used for bidirectional communication. @n
 *				It allows communications only if the applications are signed with the same certificate which is uniquely assigned to the developer.
 *
 * @since_tizen 2.3
 *
 * @remarks		You muse release @a message using bundle_free() after sending the message.
 * @param[in]	remote_app_id		The ID of the remote application
 * @param[in]	remote_port			The name of the remote message port
 * @param[in]	message				The message to be passed to the remote application, the recommended message size is under 4KB
 * @param[in]	local_port_id		The message port ID returned by message_port_register_local_port() or message_port_register_trusted_local_port()
 * @return		0 on success,
 *				otherwise a negative error value
 * @retval		#MESSAGE_PORT_ERROR_NONE					Successful
 * @retval		#MESSAGE_PORT_ERROR_INVALID_PARAMETER		The specified @a remote_app_id, @a remote_port or @a message is @c NULL and
													specified @a local_port_id is not positive
 * @retval		#MESSAGE_PORT_ERROR_OUT_OF_MEMORY			Out of memory
 * @retval		#MESSAGE_PORT_ERROR_PORT_NOT_FOUND			The port of the local or remote application cannot be found.
 * @retval		#MESSAGE_PORT_ERROR_CERTIFICATE_NOT_MATCH	The remote application is not signed with the same certificate.
 * @retval		#MESSAGE_PORT_ERROR_MAX_EXCEEDED			The size of the message has exceeded the maximum limit.
 * @retval		#MESSAGE_PORT_ERROR_IO_ERROR				Internal I/O error
 * @post		It invokes message_port_trusted_message_cb() on the remote application.
 * @see			message_port_trusted_message_cb()
 * @see			message_port_register_trusted_local_port()
 * @see			message_port_unregister_trusted_local_port()
 */
EXPORT_API int message_port_send_trusted_message_with_local_port(const char *remote_app_id, const char *remote_port, bundle *message, int local_port_id);

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_APPFW_MESSAGE_PORT_H__ */
