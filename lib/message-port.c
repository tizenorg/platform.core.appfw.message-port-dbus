/* vi: set et sw=4 ts=4 cino=t0,(0: */
/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 * This file is part of message-port.
 *
 * Copyright (C) 2013 Intel Corporation.
 *
 * Contact: Amarnath Valluri <amarnath.valluri@linux.intel.com>
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

#include "message-port.h"
#include "msgport-factory.h"
#include "msgport-manager.h"
#include "msgport-utils.h"
#include "common/log.h"

void
_messageport_cb_helper (int id, const char* remote_app_id, const char* remote_port, bool trusted_message, bundle* message, void *userdata)
{
    messageport_message_cb client_cb = (messageport_message_cb)userdata;

    if (client_cb) client_cb(id, remote_app_id, remote_port, trusted_message, message);
}

static int
_messageport_register_port (const char *name, gboolean is_trusted, messageport_message_cb_full cb, void *userdata)
{
    int port_id = 0; /* id of the port created */
    messageport_error_e res;
    MsgPortManager *manager = msgport_factory_get_manager ();

    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;

    res = msgport_manager_register_service (manager, name, is_trusted, cb, userdata, &port_id);

    return port_id > 0 ? port_id : (int)res;
}

static int
_messageport_unregister_port (int local_port_id, gboolean is_trusted)
{
    messageport_error_e res, res_check_trust;
    gboolean is_trusted_out = FALSE;
    MsgPortManager *manager = msgport_factory_get_manager ();
    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;

    res_check_trust = msgport_manager_get_service_is_trusted (manager, local_port_id, &is_trusted_out);
    if (res_check_trust != MESSAGEPORT_ERROR_NONE)
        return (int)res_check_trust;

    if (is_trusted_out == is_trusted)
        return MESSAGEPORT_ERROR_MESSAGEPORT_NOT_FOUND;

    res = msgport_manager_unregister_service (manager, local_port_id);

    return (int)res;
}

static messageport_error_e
_messageport_check_remote_port (const char *app_id, const char *port, gboolean is_trusted, bool *exists)
{
    messageport_error_e res;
    MsgPortManager *manager = msgport_factory_get_manager ();

    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;

    res = msgport_manager_check_remote_service (manager, app_id, port, is_trusted, NULL);

    if (exists) *exists = (bool)(res == MESSAGEPORT_ERROR_NONE);

    return res;
}

static messageport_error_e
_messageport_send_message (const char *app_id, const char *port, gboolean is_trusted, bundle *message)
{
    MsgPortManager *manager = msgport_factory_get_manager ();

    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;
    if (!message) return MESSAGEPORT_ERROR_INVALID_PARAMETER;

    GVariant *v_data = bundle_to_variant_map (message);

    return msgport_manager_send_message (manager, app_id, port, is_trusted, v_data);
}

messageport_error_e
_messageport_send_bidirectional_message (int id, const gchar *remote_app_id, const gchar *remote_port, gboolean is_trusted, bundle *message)
{
    MsgPortManager *manager = msgport_factory_get_manager ();

    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;
    if (!message) return MESSAGEPORT_ERROR_INVALID_PARAMETER;

    GVariant *v_data = bundle_to_variant_map (message);

    return msgport_manager_send_bidirectional_message (manager, id, remote_app_id, remote_port, is_trusted, v_data);
}

/*
 * API
 */

int
messageport_register_local_port (const char* local_port, messageport_message_cb callback)
{
    return _messageport_register_port (local_port, FALSE, _messageport_cb_helper, (void*)callback);
}

int
messageport_register_local_port_full (const char *local_port, messageport_message_cb_full callback, void *userdata)
{
    return _messageport_register_port (local_port, FALSE, callback, userdata);
}

int
messageport_register_trusted_local_port (const char *local_port, messageport_message_cb callback)
{
    return _messageport_register_port (local_port, TRUE, _messageport_cb_helper, (void*)callback);
}

int
messageport_register_trusted_local_port_full (const char *local_port, messageport_message_cb_full callback, void *userdata)
{
    return _messageport_register_port (local_port, TRUE, callback, userdata);
}

int
messageport_unregister_local_port (int local_port_id)
{
    return _messageport_unregister_port (local_port_id, FALSE);
}

int
messageport_unregister_trusted_local_port (int trusted_local_port_id)
{
    return _messageport_unregister_port (trusted_local_port_id, TRUE);
}

messageport_error_e
messageport_check_remote_port (const char *remote_app_id, const char *port_name, bool *exists)
{
    return _messageport_check_remote_port (remote_app_id, port_name, FALSE, exists);
}

messageport_error_e
messageport_check_trusted_remote_port (const char *remote_app_id, const char *port_name, bool *exists)
{
    return _messageport_check_remote_port (remote_app_id, port_name, TRUE, exists);
}

messageport_error_e
messageport_send_message (const char* remote_app_id, const char* remote_port, bundle* message)
{
    return _messageport_send_message (remote_app_id, remote_port, FALSE, message);
}

messageport_error_e
messageport_send_trusted_message (const char* remote_app_id, const char* remote_port, bundle* message)
{
    return _messageport_send_message (remote_app_id, remote_port, TRUE, message);
}

messageport_error_e
messageport_send_bidirectional_message (int id, const char* remote_app_id, const char* remote_port, bundle* data)
{
    return _messageport_send_bidirectional_message (id, remote_app_id, remote_port, FALSE, data);
}

messageport_error_e
messageport_send_bidirectional_trusted_message (int id, const char *remote_app_id, const char *remote_port, bundle *data)
{
    return _messageport_send_bidirectional_message (id, remote_app_id, remote_port, TRUE, data);
}

messageport_error_e
messageport_get_local_port_name (int id, char **name_out)
{
    MsgPortManager *manager = msgport_factory_get_manager ();

    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;

    return msgport_manager_get_service_name (manager, id, name_out);
}

messageport_error_e
messageport_check_trusted_local_port (int id, bool *is_trusted_out)
{
    MsgPortManager *manager = msgport_factory_get_manager ();

    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;

    gboolean is_trusted = FALSE;
    messageport_error_e res = msgport_manager_get_service_is_trusted (
            manager, id, &is_trusted);
    if (is_trusted_out) *is_trusted_out = (bool)is_trusted;

    return res;
}

