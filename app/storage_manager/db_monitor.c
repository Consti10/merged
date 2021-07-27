#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <inttypes.h>

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>

#include "json-c/json.h"
#include "db_monitor.h"
#include "manage.h"
#include "dbserver.h"
#include "dbus_signal.h"

#include "log.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "db_monitor.c"

void signal_storage_datachanged(void *user_data)
{
    db_data_changed(user_data);
}

static int get_storage_config(void)
{
    char *json_str = dbserver_get_storage_config();

    if (json_str) {
        add_db_storage_config(json_str);
        g_free(json_str);
        return 0;
    }

    return -1;
}

static int get_meida_folder(void)
{
    char *json_str = dbserver_get_storage_media_folder();

    if (json_str) {
        add_db_media_folder(json_str);
        g_free(json_str);
        return 0;
    }

    return -1;
}

static int get_disk_path(void)
{
    char *json_str = dbserver_get_storage_disk_path(NULL);

    if (json_str) {
        add_db_disk_path(json_str);
        g_free(json_str);
        return 0;
    }

    return -1;
}

void db_monitor_init(void)
{
    disable_loop();
    while (get_storage_config() != 0) {
        LOG_INFO("dbserver_get_storage_config, wait dbserver.\n");
        usleep(50000);
    }
    while (get_meida_folder() != 0) {
        LOG_INFO("dbserver_get_meida_folder, wait dbserver.\n");
        usleep(50000);
    }

    while (get_disk_path() != 0) {
        LOG_INFO("dbserver_get_disk_path, wait dbserver.\n");
        usleep(50000);
    }
    dbus_monitor_signal_registered(DBSERVER_STORAGE_INTERFACE, DS_SIGNAL_DATACHANGED, &signal_storage_datachanged);
}