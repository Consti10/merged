// Copyright 2020 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <glib.h>
#include <dbus-c++/dbus.h>

#include "json-c/json.h"
#include "storage_manager.h"
#include "storage_manager_proxy.h"
#include "dbus_connection.h"

#define STORAGEMANAGER_DBUSSEND(FUNC) \
    char *ret = NULL; \
    dbus_mutex_lock(); \
    try { \
        DBus::Connection conn = get_dbus_conn(); \
        DBusStorageManager* storage_manage_proxy_ = new DBusStorageManager(conn, STORAGEMANAGER_PATH, STORAGEMANAGER, STORAGEMANAGER_INTERFACE); \
        auto config = storage_manage_proxy_->FUNC(json); \
        ret = g_strdup(config.c_str()); \
        delete storage_manage_proxy_; \
    } catch (DBus::Error err) { \
        printf("DBus::Error - %s\n", err.what()); \
    } \
    dbus_mutex_unlock(); \
    return ret;

char *dbus_storage_manager_diskformat(char *json)
{
    STORAGEMANAGER_DBUSSEND(DiskFormat);
}

char *dbus_storage_manager_get_media_path(char *json)
{
    STORAGEMANAGER_DBUSSEND(GetMediaPath);
}

char *dbus_storage_manager_get_disks_status(char *json)
{
    STORAGEMANAGER_DBUSSEND(GetDisksStatus);
}

char *dbus_storage_manager_get_filelist(char *json)
{
    STORAGEMANAGER_DBUSSEND(GetFileList);
}

extern "C" char *storage_manager_diskformat(char *mountpath, char *type)
{
    char *ret;
    json_object *j_cfg = json_object_new_object();

    json_object_object_add(j_cfg, "sMountPath", json_object_new_string(mountpath));
    json_object_object_add(j_cfg, "sType", json_object_new_string(type));

    ret = dbus_storage_manager_diskformat((char *)json_object_to_json_string(j_cfg));

    json_object_put(j_cfg);

    return ret;
}

extern "C" char *storage_manager_get_media_path(void)
{
    return dbus_storage_manager_get_media_path((char *)"");
}

extern "C" char *storage_manager_get_disks_status(char *mountpath)
{
    char *ret = NULL;
    json_object *j_cfg = json_object_new_object();

    json_object_object_add(j_cfg, "sMountPath", json_object_new_string(mountpath));

    ret = dbus_storage_manager_get_disks_status((char *)json_object_to_json_string(j_cfg));

    json_object_put(j_cfg);

    return ret;
}

extern "C" char *storage_manager_get_filelist_id(int rootid, int pathid, int order, char *limit)
{
    char *ret;
    json_object *j_cfg = json_object_new_object();

    json_object_object_add(j_cfg, "rootid", json_object_new_int(rootid));
    json_object_object_add(j_cfg, "pathid", json_object_new_int(pathid));
    json_object_object_add(j_cfg, "order", json_object_new_int(order));
    if (limit)
        json_object_object_add(j_cfg, "limit", json_object_new_string(limit));

    ret = dbus_storage_manager_get_filelist((char *)json_object_to_json_string(j_cfg));

    json_object_put(j_cfg);

    return ret;
}

extern "C" char *storage_manager_get_filelist_path(char *path, int *StartTime, int *EndTime, int order, char *limit)
{
    char *ret;
    json_object *j_cfg = json_object_new_object();

    json_object_object_add(j_cfg, "path", json_object_new_string(path));
    json_object_object_add(j_cfg, "order", json_object_new_int(order));
    if (StartTime)
        json_object_object_add(j_cfg, "starttime", json_object_new_int(*StartTime));
    if (EndTime)
        json_object_object_add(j_cfg, "endtime", json_object_new_int(*EndTime));
    if (limit)
        json_object_object_add(j_cfg, "limit", json_object_new_string(limit));

    ret = dbus_storage_manager_get_filelist((char *)json_object_to_json_string(j_cfg));

    json_object_put(j_cfg);

    return ret;
}