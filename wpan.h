/*
 * netifd - network interface daemon
 * Copyright (C) 2013 Felix Fietkau <nbd@openwrt.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#ifndef __NETIFD_WPAN_H
#define __NETIFD_WPAN_H

#include <libubox/utils.h>
#include "interface.h"

extern struct vlist_tree wpan_devices;
extern struct avl_tree wpan_drivers;

struct wpan_driver {
	struct avl_node node;

	const char *name;
	const char *script;

	struct {
		char *buf;
		struct uci_blob_param_list *config;
	} device, interface;
};

struct wpan_device {
	struct vlist_node node;

	struct wpan_driver *drv;
	struct vlist_tree interfaces;
	char *name;

	struct netifd_process script_task;
	struct uloop_timeout timeout;
	struct uloop_timeout poll;

	struct list_head script_proc;
	struct uloop_fd script_proc_fd;
	struct uloop_timeout script_check;

	struct ubus_request_data *kill_request;

	struct blob_attr *prev_config;
	struct blob_attr *config;
	struct blob_attr *data;

	bool config_autostart;
	bool autostart;
	bool disabled;

	enum interface_state state;
	enum interface_config_state config_state;
	bool cancel;
	int retry;

	int vif_idx;
};

struct wpan_interface {
	struct vlist_node node;
	const char *section;
	char *name;

	struct wpan_device *wdev;

	struct blob_attr *config;
	struct blob_attr *data;

	const char *ifname;
	struct blob_attr *network;
	bool isolate;
	bool ap_mode;
};

struct wpan_process {
	struct list_head list;

	const char *exe;
	int pid;

	bool required;
};

void wpan_device_create(struct wpan_driver *drv, const char *name, struct blob_attr *data);
void wpan_device_set_up(struct wpan_device *wdev);
void wpan_device_set_down(struct wpan_device *wdev);
void wpan_device_status(struct wpan_device *wdev, struct blob_buf *b);
void wpan_device_get_validate(struct wpan_device *wdev, struct blob_buf *b);
void wpan_interface_create(struct wpan_device *wdev, struct blob_attr *data, const char *section);
int wpan_device_notify(struct wpan_device *wdev, struct blob_attr *data,
			   struct ubus_request_data *req);

void wpan_start_pending(void);
void wpan_init(void);

#endif
