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

#include <stdlib.h>
#include "message_port.h"

EXPORT_API int
message_port_register_local_port (const char *local_port, message_port_message_cb callback, void *user_data)
{
    return messageport_register_local_port_full (local_port, callback, user_data);
}

EXPORT_API int
message_port_register_trusted_local_port (const char *trusted_local_port, message_port_trusted_message_cb callback, void *user_data)
{
    return messageport_register_trusted_local_port_full (trusted_local_port, callback, user_data);
}

EXPORT_API int
message_port_unregister_local_port (int local_port_id)
{
    return messageport_unregister_local_port (local_port_id);
}

EXPORT_API int
message_port_unregister_trusted_local_port (int trusted_local_port_id)
{
    return messageport_unregister_trusted_local_port (trusted_local_port_id);
}

EXPORT_API int
message_port_check_remote_port (const char *remote_app_id, const char *remote_port, bool *exist)
{
    return messageport_check_remote_port (remote_app_id, remote_port, exist);
}

EXPORT_API int
message_port_check_trusted_remote_port (const char *remote_app_id, const char *remote_port, bool *exist)
{
    return messageport_check_trusted_remote_port (remote_app_id, remote_port, exist);
}

EXPORT_API int
message_port_send_message (const char *remote_app_id, const char *remote_port, bundle *message)
{
    return messageport_send_message (remote_app_id, remote_port, message);
}

EXPORT_API int
message_port_send_trusted_message( const char *remote_app_id, const char *remote_port, bundle *message)
{
    return messageport_send_trusted_message (remote_app_id, remote_port, message);
}

EXPORT_API int
message_port_send_message_with_local_port (const char *remote_app_id, const char *remote_port, bundle *message, int local_port_id)
{
    return messageport_send_bidirectional_message (local_port_id, remote_app_id, remote_port, message);
}

EXPORT_API int
message_port_send_trusted_message_with_local_port (const char *remote_app_id, const char *remote_port, bundle *message, int local_port_id)
{
    return messageport_send_bidirectional_trusted_message (local_port_id, remote_app_id, remote_port, message);
}


