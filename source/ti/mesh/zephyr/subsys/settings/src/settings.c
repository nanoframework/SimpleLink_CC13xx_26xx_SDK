/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 * Copyright (c) 2015 Runtime Inc
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifdef CONFIG_BT_SETTINGS
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <kernel.h>

#include "settings/settings.h"
#include <zephyr/types.h>

#include "common/log.h"
LOG_MODULE_REGISTER(settings, CONFIG_SETTINGS_LOG_LEVEL);

K_MUTEX_DEFINE(settings_lock);

extern const struct settings_handler_static settings_handler_bt_mesh;

void settings_store_init(void);

void settings_init(void)
{
	settings_store_init();
}

int settings_name_steq(const char *name, const char *key, const char **next)
{
	if (next) {
		*next = NULL;
	}

	if ((!name) || (!key)) {
		return 0;
	}

	/* name might come from flash directly, in flash the name would end
	 * with '=' or '\0' depending how storage is done. Flash reading is
	 * limited to what can be read
	 */

	while ((*key != '\0') && (*key == *name) &&
	       (*name != '\0') && (*name != SETTINGS_NAME_END)) {
		key++;
		name++;
	}

	if (*key != '\0') {
		return 0;
	}

	if (*name == SETTINGS_NAME_SEPARATOR) {
		if (next) {
			*next = name + 1;
		}
		return 1;
	}

	if ((*name == SETTINGS_NAME_END) || (*name == '\0')) {
		return 1;
	}

	return 0;
}

int settings_name_next(const char *name, const char **next)
{
	int rc = 0;

	if (next) {
		*next = NULL;
	}

	if (!name) {
		return 0;
	}

	/* name might come from flash directly, in flash the name would end
	 * with '=' or '\0' depending how storage is done. Flash reading is
	 * limited to what can be read
	 */
	while ((*name != '\0') && (*name != SETTINGS_NAME_END) &&
	       (*name != SETTINGS_NAME_SEPARATOR)) {
		rc++;
		name++;
	}

	if (*name == SETTINGS_NAME_SEPARATOR) {
		if (next) {
			*next = name + 1;
		}
		return rc;
	}

	return rc;
}

struct settings_handler_static *settings_parse_and_lookup(const char *name,
							const char **next)
{
	struct settings_handler_static *bestmatch;
#if defined(__IAR_SYSTEMS_ICC__)
	struct settings_handler_static *ch = (struct settings_handler_static *) &settings_handler_bt_mesh;
#elif defined(__TI_COMPILER_VERSION__)
#pragma diag_suppress 145
	struct settings_handler_static *ch = &settings_handler_bt_mesh;
#elif defined(__clang__)
#pragma clang diagnostic ignored "-Wincompatible-pointer-types-discards-qualifiers"
    struct settings_handler_static *ch = &settings_handler_bt_mesh;
#endif /* defined(__IAR_SYSTEMS_ICC__) */        
	const char *tmpnext;

	bestmatch = NULL;
	if (next) {
		*next = NULL;
	}

	{
		if (!settings_name_steq(name, ch->name, &tmpnext)) {
			return bestmatch;
		}
		if (!bestmatch) {
			bestmatch = ch;
			if (next) {
				*next = tmpnext;
			}
			return bestmatch;
		}
		if (settings_name_steq(ch->name, bestmatch->name, NULL)) {
			bestmatch = ch;
			if (next) {
				*next = tmpnext;
			}
		}
	}

	return bestmatch;
}

int settings_call_set_handler(const char *name,
			      size_t len,
			      settings_read_cb read_cb,
			      void *read_cb_arg,
			      const struct settings_load_arg *load_arg)
{
	int rc;
	const char *name_key = name;

	if (load_arg && load_arg->subtree &&
	    !settings_name_steq(name, load_arg->subtree, &name_key)) {
		return 0;
	}

	if (load_arg && load_arg->cb) {
		rc = load_arg->cb(name_key, len, read_cb, read_cb_arg,
				  load_arg->param);
	} else {
		struct settings_handler_static *ch;

		ch = settings_parse_and_lookup(name, &name_key);
		if (!ch) {
			return 0;
		}

		rc = ch->h_set(name_key, len, read_cb, read_cb_arg);

		if (rc != 0) {
			LOG_ERR("set-value failure. key: %s error(%d)",
				log_strdup(name), rc);
			/* Ignoring the error */
			rc = 0;
		} else {
			LOG_DBG("set-value OK. key: %s",
				log_strdup(name));
		}
	}
	return rc;
}

int settings_commit(void)
{
	return settings_commit_subtree(NULL);
}


int settings_commit_subtree(const char *subtree)
{
	int rc;
	int rc2;

	rc = 0;
	const struct settings_handler_static *ch = &settings_handler_bt_mesh;

	if (ch->h_commit) {
		rc2 = ch->h_commit();
		if (!rc) {
			rc = rc2;
		}
	}

	return rc;
}
#endif //CONFIG_BT_SETTINGS
