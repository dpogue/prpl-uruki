/*
 * prpl-uruki - Pidgin KI Chat plugin for Myst Online: Uru Live.
 * Copyright (C) 2010 Darryl Pogue
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "kiPurple.h"

const char* ki_list_icon(PurpleAccount* account, PurpleBuddy* buddy) {
    return "uruki";
}

char* ki_status_text(PurpleBuddy* buddy) {
	const PurplePresence *presence;
	const PurpleStatus *status;
	const char *location;
	char *ret = NULL;

	presence = purple_buddy_get_presence(buddy);
	status = purple_presence_get_active_status(presence);

	location = purple_status_get_attr_string(status, "location");

	if (location != NULL) {
        ret = g_strdup_printf("KI#%s : %s", purple_buddy_get_name(buddy),
                g_markup_escape_text(location, -1));
		purple_util_chrreplace(ret, '\n', ' ');

	} else {
        ret = g_strdup_printf("KI#%s", purple_buddy_get_name(buddy));
    }

	return ret;
}

GList* ki_status_types(PurpleAccount* account) {
    GList* types = NULL;
	PurpleStatusType* status;
	
	g_return_val_if_fail(account != NULL, NULL);
	
    /* Buddy status messages hold location information */
	status = purple_status_type_new_with_attrs(
            PURPLE_STATUS_AVAILABLE,
            "b_online", NULL, FALSE, FALSE, FALSE,
            "location", _("Location"),
            purple_value_new(PURPLE_TYPE_STRING), NULL);
	types = g_list_prepend(types, status);
	
	status = purple_status_type_new_with_attrs(
            PURPLE_STATUS_AWAY,
            "b_away", NULL, FALSE, FALSE, FALSE,
            "location", _("Location"),
            purple_value_new(PURPLE_TYPE_STRING), NULL);
	types = g_list_prepend(types, status);

	status = purple_status_type_new_full(
            PURPLE_STATUS_OFFLINE,
            "b_offline", NULL, FALSE, FALSE, FALSE);
	types = g_list_prepend(types, status);

    /* Our status is simply online or offline */
	status = purple_status_type_new_full(
            PURPLE_STATUS_AVAILABLE,
            "online", NULL, TRUE, TRUE, FALSE);
	types = g_list_prepend(types, status);
	
	status = purple_status_type_new_full(
            PURPLE_STATUS_OFFLINE,
            "offline", NULL, TRUE, TRUE, FALSE);
	types = g_list_prepend(types, status);
	
	return types;
}

void ki_login(PurpleAccount* account) {
    PurpleConnection* pc = purple_account_get_connection(account);
    kiClient* ki;

    pc->flags = (PurpleConnectionFlags)(
        PURPLE_CONNECTION_NO_BGCOLOR |
        PURPLE_CONNECTION_FORMATTING_WBFO | 
        PURPLE_CONNECTION_NO_NEWLINES |
        PURPLE_CONNECTION_NO_FONTSIZE |
        PURPLE_CONNECTION_NO_URLDESC |
        PURPLE_CONNECTION_NO_IMAGES);

    ki = new kiClient(pc, account);
    pc->proto_data = ki;

    ki->connect();
}

void ki_close(PurpleConnection* pc) {
    kiClient* ki = (kiClient*)(pc->proto_data);
    ki->disconnect();
    delete ki;

    pc->proto_data = NULL;
}

int ki_send_pm(PurpleConnection* pc, const char* who,
                const char* message, PurpleMessageFlags flags) {
    /* TODO */
    return 0;
}

void ki_get_info(PurpleConnection* pc, const char* who) {
    /* TODO */
    return;
}

void ki_set_status(PurpleAccount* account, PurpleStatus* status) {
    /* TODO */
}

void ki_add_buddy(PurpleConnection* pc, PurpleBuddy* buddy,
                    PurpleGroup* group) {
    /* TODO */
    return;
}

void ki_add_buddies(PurpleConnection* pc, GList* buddies, GList* groups) {
    /* TODO */
    return;
}

void ki_remove_buddy(PurpleConnection* pc, PurpleBuddy* buddy,
                      PurpleGroup* group) {
    /* TODO */
    return;
}

void ki_ignore(PurpleConnection* pc, const char* name) {
    /* TODO */
    return;
}

void ki_unignore(PurpleConnection* pc, const char* name) {
    /* TODO */
    return;
}

void ki_keepalive(PurpleConnection* pc) {
    kiClient* ki = (kiClient*)(pc->proto_data);
    ki->ping();
}

const char* ki_normalise(const PurpleAccount* account, const char* who) {
    /* TODO */
    return who;
}

gboolean ki_offline_message(const PurpleBuddy* buddy) {
    return FALSE;
}
