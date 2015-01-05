/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <map>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <message-port.h>
#include "message_port_internal.h"
#include "message_port_log.h"
#include "message_port.h"

struct message_port_callback_item_s {
	message_port_message_cb callback;
	void *user_data;
};
typedef std::map<int, message_port_callback_item_s> ListenerMap;

static ListenerMap __listeners;
static ListenerMap __trusted_listeners;
static pthread_mutex_t __mutex = PTHREAD_MUTEX_INITIALIZER;


static void do_callback(message_port_message_cb callback, int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message, void *user_data)
{
	if (callback)
	{
		callback(local_port_id, remote_app_id, remote_port, trusted_remote_port, message, user_data);
		bundle_free(message);
	}
	else
	{
		_LOGI("Ignored");
	}
}

static void message_dispatcher(int local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message)
{
	_SECURE_LOGI("A message has been received to specific local port id (%d) from%s remote port (%s):(%s).", local_port_id, trusted_remote_port ? " trusted" : "", remote_app_id, remote_port);
	do_callback(__listeners[local_port_id].callback, local_port_id, remote_app_id, remote_port, trusted_remote_port, message, __listeners[local_port_id].user_data);
}

static void trusted_message_dispatcher(int trusted_local_port_id, const char *remote_app_id, const char *remote_port, bool trusted_remote_port, bundle *message)
{
	_SECURE_LOGI("A message has been received to specific trusted local port id (%d) from%s remote port (%s):(%s).", trusted_local_port_id, trusted_remote_port ? " trusted" : "", remote_app_id, remote_port);
	do_callback(__trusted_listeners[trusted_local_port_id].callback, trusted_local_port_id, remote_app_id, remote_port, trusted_remote_port, message, __trusted_listeners[trusted_local_port_id].user_data);
}

int message_port_register_local_port(const char *local_port, message_port_message_cb callback, void *user_data)
{
	if (local_port == NULL || callback == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}

	int local_port_id = messageport_register_local_port(local_port, message_dispatcher);
	if (local_port_id > 0)
	{
		_SECURE_LOGI("Register local port ID (%d).", local_port_id);
		pthread_mutex_lock(&__mutex);
		__listeners[local_port_id].callback = callback;
		__listeners[local_port_id].user_data = user_data;
		pthread_mutex_unlock(&__mutex);
	}
	return convert_to_tizen_error((messageport_error_e)local_port_id);
}

int message_port_register_trusted_local_port(const char *local_port, message_port_trusted_message_cb callback, void *user_data)
{
	if (local_port == NULL || callback == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}

	int trusted_local_port_id = messageport_register_trusted_local_port(local_port, trusted_message_dispatcher);
	if (trusted_local_port_id > 0)
	{
		_SECURE_LOGI("Register trusted local port ID (%d).", trusted_local_port_id);
		pthread_mutex_lock(&__mutex);
		__trusted_listeners[trusted_local_port_id].callback = callback;
		__trusted_listeners[trusted_local_port_id].user_data = user_data;
		pthread_mutex_unlock(&__mutex);
	}
	return convert_to_tizen_error((messageport_error_e)trusted_local_port_id);
}

int message_port_unregister_local_port(int local_port_id)
{
	if (local_port_id <= 0)
	{
		_LOGE("[MESSAGEPORT_ERROR_INVALID_PARAMETER] Neither 0 nor negative value is allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	else if (__listeners[local_port_id].callback == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_PORT_NOT_FOUND] The local port ID (%d) is not registered.", local_port_id);
		return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;
	}

	_SECURE_LOGI("Unregister local port ID (%d).", local_port_id);
	char *local_port_name = NULL;
	int res = messageport_get_local_port_name(local_port_id, &local_port_name);
	if (res == MESSAGE_PORT_ERROR_NONE && local_port_name != NULL)
	{
		pthread_mutex_lock(&__mutex);
		messageport_register_local_port(local_port_name, NULL);
		__listeners.erase(local_port_id);
		pthread_mutex_unlock(&__mutex);

		free(local_port_name);
	}
	return convert_to_tizen_error((messageport_error_e)res);
}

int message_port_unregister_trusted_local_port(int trusted_local_port_id)
{
	if (trusted_local_port_id <= 0)
	{
		_LOGE("[MESSAGEPORT_ERROR_INVALID_PARAMETER] Neither 0 nor negative value is allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	else if (__trusted_listeners[trusted_local_port_id].callback == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_PORT_NOT_FOUND] The trusted local port ID (%d) is not registered.", trusted_local_port_id);
		return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;
	}

	_SECURE_LOGI("Unregister local port ID (%d).", trusted_local_port_id);
	char *local_port_name = NULL;
	int res = messageport_get_local_port_name(trusted_local_port_id, &local_port_name);
	if (res == MESSAGE_PORT_ERROR_NONE && local_port_name != NULL)
	{
		pthread_mutex_lock(&__mutex);
		messageport_register_trusted_local_port(local_port_name, NULL);
		__trusted_listeners.erase(trusted_local_port_id);
		pthread_mutex_unlock(&__mutex);

		free(local_port_name);
	}
	return convert_to_tizen_error((messageport_error_e)res);
}

int message_port_check_remote_port(const char* remote_app_id, const char *remote_port, bool* exist)
{
	if (remote_app_id == NULL || remote_port == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	_SECURE_LOGI("Check remote port (%s):(%s).", remote_app_id, remote_port);
	return convert_to_tizen_error((messageport_error_e)messageport_check_remote_port(remote_app_id, remote_port, exist));
}

int message_port_check_trusted_remote_port(const char* remote_app_id, const char *remote_port, bool *exist)
{
	if (remote_app_id == NULL || remote_port == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	_SECURE_LOGI("Check trusted remote port (%s):(%s).", remote_app_id, remote_port);
	return convert_to_tizen_error((messageport_error_e)messageport_check_trusted_remote_port(remote_app_id, remote_port, exist));
}

int message_port_send_message(const char *remote_app_id, const char *remote_port, bundle *message)
{
	if (remote_app_id == NULL || remote_port == NULL || message == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	_SECURE_LOGI("Send a message to the remote port (%s):(%s).", remote_app_id, remote_port);
	return convert_to_tizen_error((messageport_error_e)messageport_send_message(remote_app_id, remote_port, message));
}

int message_port_send_trusted_message(const char *remote_app_id, const char *remote_port, bundle *message)
{
	if (remote_app_id == NULL || remote_port == NULL || message == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	_SECURE_LOGI("Send a trusted message to the remote port (%s):(%s).", remote_app_id, remote_port);
	return convert_to_tizen_error((messageport_error_e)messageport_send_trusted_message(remote_app_id, remote_port, message));
}

int message_port_send_message_with_local_port(const char *remote_app_id, const char *remote_port, bundle *message, int local_port_id)
{
	if (remote_app_id == NULL || remote_port == NULL || message == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	else if (local_port_id <= 0)
	{
		_LOGE("[MESSAGEPORT_ERROR_INVALID_PARAMETER] Neither 0 nor negative value is allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	else if (__listeners[local_port_id].callback == NULL && __trusted_listeners[local_port_id].callback == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_PORT_NOT_FOUND] The local port ID (%d) is not registered.", local_port_id);
		return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;
	}

	_SECURE_LOGI("Send a message to (%s):(%s) and listen at the local port ID (%d).", remote_app_id, remote_port, local_port_id);
	return convert_to_tizen_error((messageport_error_e)messageport_send_bidirectional_message(local_port_id, remote_app_id, remote_port, message));
}

int message_port_send_trusted_message_with_local_port(const char* remote_app_id, const char *remote_port, bundle* message, int local_port_id)
{
	if (remote_app_id == NULL || remote_port == NULL || message == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_INVALID_PARAMETER] NULL value is not allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	else if (local_port_id <= 0)
	{
		_LOGE("[MESSAGEPORT_ERROR_INVALID_PARAMETER] Neither 0 nor negative value is allowed.");
		return MESSAGE_PORT_ERROR_INVALID_PARAMETER;
	}
	else if (__listeners[local_port_id].callback == NULL && __trusted_listeners[local_port_id].callback == NULL)
	{
		_LOGE("[MESSAGE_PORT_ERROR_PORT_NOT_FOUND] The local port ID (%d) is not registered.", local_port_id);
		return MESSAGE_PORT_ERROR_PORT_NOT_FOUND;
	}

	_SECURE_LOGI("Send a trusted message to (%s):(%s) and listen at the local port ID (%d).", remote_app_id, remote_port, local_port_id);
	return convert_to_tizen_error((messageport_error_e)messageport_send_bidirectional_trusted_message(local_port_id, remote_app_id, remote_port, message));
}

