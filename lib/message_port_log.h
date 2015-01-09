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

#ifndef __TIZEN_APPFW_MESSAGE_PORT_LOG_H__
#define __TIZEN_APPFW_MESSAGE_PORT_LOG_H__

#include <dlog.h>

#undef LOG_TAG
#define LOG_TAG "MESSAGE_PORT"

#define _LOGE(fmt, arg...) LOGE(fmt, ##arg)
#define _LOGI(fmt, arg...) LOGI(fmt, ##arg)

#define _SECURE_LOGE(fmt, arg...) SECURE_LOGE(fmt, ##arg)
#define _SECURE_LOGI(fmt, arg...) SECURE_LOGI(fmt, ##arg)

#endif /* __TIZEN_APPFW_MESSAGE_PORT_LOG_H__ */
