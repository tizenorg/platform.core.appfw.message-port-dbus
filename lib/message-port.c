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

#include <stdlib.h>
#include "message_port.h"
#include "msgport-factory.h"
#include "msgport-manager.h"
#include "msgport-utils.h"
#include "common/log.h"

static int
_messageport_register_port (const char *name, gboolean is_trusted, messageport_message_cb cb)
{
    int port_id = 0; /* id of the port created */
    messageport_error_e res;
    MsgPortManager *manager = msgport_factory_get_manager ();

    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;

    res = msgport_manager_register_service (manager, name, is_trusted, cb, &port_id);

    return port_id > 0 ? port_id : (int)res;
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

    GVariant *v_data = bundle_to_variant_map (message);

    return msgport_manager_send_message (manager, app_id, port, is_trusted, v_data);
}

messageport_error_e
_messageport_send_bidirectional_message (int id, const gchar *remote_app_id, const gchar *remote_port, gboolean is_trusted, bundle *message)
{
    MsgPortManager *manager = msgport_factory_get_manager ();

    if (!manager) return MESSAGEPORT_ERROR_IO_ERROR;

    GVariant *v_data = bundle_to_variant_map (message);

    return msgport_manager_send_bidirectional_message (manager, id, remote_app_id, remote_port, is_trusted, v_data);
}

typedef struct {
    message_port_message_cb callback;
    void *user_data;
} message_port_callback_item_s;


static void
do_callback (message_port_message_cb callback, int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *user_data)
{
    if (callback)
    {
        callback(local_port_id, remote_app_id, remote_port, trusted_remote_port, message, user_data);
        bundle_free(message);
    }
    else
    {
        DBG("Ignored");
    }
}

static void
message_dispatcher (int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message)
{
    DBG("A message has been received to specific local port id (%d) from%s remote port (%s):(%s).", local_port_id, trusted_remote_port ? " trusted" : "", remote_app_id, remote_port);
    message_port_callback_item_s *item = g_hash_table_lookup(msgport_listeners, &local_port_id);

    do_callback(item->callback, local_port_id, remote_app_id, remote_port, trusted_remote_port, message, item->user_data);
}

static void
trusted_message_dispatcher (int trusted_local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message)
{
    DBG("A message has been received to specific trusted local port id (%d) from%s remote port (%s):(%s).", trusted_local_port_id, trusted_remote_port ? " trusted" : "", remote_app_id, remote_port);
    message_port_callback_item_s *item = g_hash_table_lookup(msgport_trusted_listeners, &trusted_local_port_id);
    do_callback(item->callback, trusted_local_port_id, remote_app_id, remote_port, trusted_remote_port, message, item->user_data);
}

/*
 * API
 */

int
messageport_register_local_port (const char* local_port, messageport_message_cb callback)
{
    return _messageport_register_port (local_port, FALSE, callback);
}

messageport_error_e
messageport_register_trusted_local_port (const char *local_port, messageport_message_cb callback)
{
    return _messageport_register_port (local_port, TRUE, callback);
}

messageport_error_e
messageport_check_remote_port (const char* remote_app_id, const char *remote_port, bool *exist)
{
    if (remote_app_id == NULL || remote_port == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }
    DBG("Check remote port (%s):(%s).", remote_app_id, remote_port);
    return _messageport_check_remote_port (remote_app_id, remote_port, FALSE, exist);
}

messageport_error_e
messageport_check_trusted_remote_port (const char* remote_app_id, const char *remote_port, bool *exist)
{
    if (remote_app_id == NULL || remote_port == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }
    DBG("Check trusted remote port (%s):(%s).", remote_app_id, remote_port);
    return _messageport_check_remote_port (remote_app_id, remote_port, TRUE, exist);
}

messageport_error_e
messageport_send_message (const char* remote_app_id, const char* remote_port, bundle* message)
{
    if (remote_app_id == NULL || remote_port == NULL || message == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }
    DBG("Send a message to the remote port (%s):(%s).", remote_app_id, remote_port);
    return _messageport_send_message(remote_app_id, remote_port, FALSE, message);
}

messageport_error_e
messageport_send_trusted_message (const char* remote_app_id, const char* remote_port, bundle* message)
{
    if (remote_app_id == NULL || remote_port == NULL || message == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }
    DBG("Send a trusted message to the remote port (%s):(%s).", remote_app_id, remote_port);
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

/* Tizen 2.3 native API */
int
message_port_register_local_port_with_user_data (const char* local_port, message_port_message_cb callback, void *user_data)
{
    if (local_port == NULL || callback == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }

    int local_port_id = _messageport_register_port (local_port, FALSE, message_dispatcher);
    if (local_port_id > 0)
    {
        DBG("Register local port ID (%d).", local_port_id);
        pthread_mutex_lock(&msgport_mutex);
        message_port_callback_item_s *item = malloc(sizeof(message_port_callback_item_s));
        g_hash_table_insert(msgport_listeners, (gpointer)&local_port_id, (gpointer)item);
        item->callback = callback;
        item->user_data = user_data;
        pthread_mutex_unlock(&msgport_mutex);
    }
    return local_port_id;
}

messageport_error_e
message_port_register_trusted_local_port_with_user_data (const char *local_port, message_port_message_cb callback, void *user_data)
{
    if (local_port == NULL || callback == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }

    int trusted_local_port_id = _messageport_register_port (local_port, TRUE, trusted_message_dispatcher);
    if (trusted_local_port_id > 0)
    {
        DBG("Register trusted local port ID (%d).", trusted_local_port_id);
        pthread_mutex_lock(&msgport_mutex);
        message_port_callback_item_s *item = malloc(sizeof(message_port_callback_item_s));
        g_hash_table_insert(msgport_trusted_listeners, (gpointer)&trusted_local_port_id, (gpointer)item);
        item->callback = callback;
        item->user_data = user_data;
        pthread_mutex_unlock(&msgport_mutex);
    }
    return trusted_local_port_id;
}

int
message_port_unregister_local_port (int local_port_id)
{
    if (local_port_id <= 0)
    {
        ERR("[MESSAGEPORT_ERROR_INVALID_PARAMETER] Neither 0 nor negative value is allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }

    message_port_callback_item_s *item = g_hash_table_lookup(msgport_listeners, &local_port_id);
    if (!item)
        return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;

    if (item->callback == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_PORT_NOT_FOUND] The local port ID (%d) is not registered.", local_port_id);
        return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;
    }

    DBG("Unregister local port ID (%d).", local_port_id);
    char *local_port_name = NULL;
    int res = messageport_get_local_port_name(local_port_id, &local_port_name);
    if (res == MESSAGE_PORT_ERROR_NONE && local_port_name != NULL)
    {
        pthread_mutex_lock(&msgport_mutex);
        messageport_register_local_port(local_port_name, NULL);
        g_hash_table_remove(msgport_listeners, (gpointer)&local_port_id);
        free(item);
        pthread_mutex_unlock(&msgport_mutex);

        g_free(local_port_name);
    }
    return res;
}

int
message_port_unregister_trusted_local_port (int trusted_local_port_id)
{
    if (trusted_local_port_id <= 0)
    {
        ERR("[MESSAGEPORT_ERROR_INVALID_PARAMETER] Neither 0 nor negative value is allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }

    message_port_callback_item_s *item = g_hash_table_lookup(msgport_trusted_listeners, &trusted_local_port_id);
    if (!item)
        return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;

    if (item->callback == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_PORT_NOT_FOUND] The trusted local port ID (%d) is not registered.", trusted_local_port_id);
        return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;
    }

    DBG("Unregister local port ID (%d).", trusted_local_port_id);
    char *local_port_name = NULL;
    int res = messageport_get_local_port_name(trusted_local_port_id, &local_port_name);
    if (res == MESSAGE_PORT_ERROR_NONE && local_port_name != NULL)
    {
        pthread_mutex_lock(&msgport_mutex);
        messageport_register_trusted_local_port(local_port_name, NULL);
        g_hash_table_remove(msgport_trusted_listeners, (gpointer)&trusted_local_port_id);
        free(item);
        pthread_mutex_unlock(&msgport_mutex);

        g_free(local_port_name);
    }
    return res;
}

int
message_port_send_message_with_local_port (const char *remote_app_id, const char *remote_port, bundle *message, int local_port_id)
{
    if (remote_app_id == NULL || remote_port == NULL || message == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }
    else if (local_port_id <= 0)
    {
        ERR("[MESSAGEPORT_ERROR_INVALID_PARAMETER] Neither 0 nor negative value is allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }

    message_port_callback_item_s *item = g_hash_table_lookup(msgport_listeners, &local_port_id);
    message_port_callback_item_s *trusted_item = g_hash_table_lookup(msgport_trusted_listeners, &local_port_id);

    if (!item && !trusted_item)
    {
        ERR("[MESSAGE_PORT_ERROR_PORT_NOT_FOUND] The local port ID (%d) is not registered.", local_port_id);
        return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;
    }

    DBG("Send a message to (%s):(%s) and listen at the local port ID (%d).", remote_app_id, remote_port, local_port_id);
    return messageport_send_bidirectional_message(local_port_id, remote_app_id, remote_port, message);
}

int
message_port_send_trusted_message_with_local_port (const char* remote_app_id, const char *remote_port, bundle* message, int local_port_id)
{
    if (remote_app_id == NULL || remote_port == NULL || message == NULL)
    {
        ERR("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }
    else if (local_port_id <= 0)
    {
        ERR("[MESSAGEPORT_ERROR_INVALID_PARAMETER] Neither 0 nor negative value is allowed.");
        return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
    }

    message_port_callback_item_s *item = g_hash_table_lookup(msgport_listeners, &local_port_id);
    message_port_callback_item_s *trusted_item = g_hash_table_lookup(msgport_trusted_listeners, &local_port_id);

    if (!item && !trusted_item)
    {
        ERR("[MESSAGE_PORT_ERROR_PORT_NOT_FOUND] The local port ID (%d) is not registered.", local_port_id);
        return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;
    }

    DBG("Send a trusted message to (%s):(%s) and listen at the local port ID (%d).", remote_app_id, remote_port, local_port_id);
    return messageport_send_bidirectional_trusted_message(local_port_id, remote_app_id, remote_port, message);
}

