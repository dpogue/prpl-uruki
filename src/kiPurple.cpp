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

GList* ki_status_types(PurpleAccount* account) {
    GList* types = NULL;
	PurpleStatusType* status;
	
	g_return_val_if_fail(account != NULL, NULL);
	
	status = purple_status_type_new(
            PURPLE_STATUS_AVAILABLE, NULL, NULL, TRUE);
	types = g_list_prepend(types, status);
	
	status = purple_status_type_new(
            PURPLE_STATUS_AWAY, NULL, NULL, TRUE);
	types = g_list_prepend(types, status);
	
	status = purple_status_type_new(
            PURPLE_STATUS_OFFLINE, NULL, NULL, TRUE);
	types = g_list_prepend(types, status);
	
	return types;
}

void ki_login(PurpleAccount* account) {
    PurpleConnection* pc = purple_account_get_connection(account);
    kiClient* ki;

    pc->flags |=
        PURPLE_CONNECTION_NO_BGCOLOR |
        PURPLE_CONNECTION_FORMATTING_WBFO | 
        PURPLE_CONNECTION_NO_NEWLINES |
        PURPLE_CONNECTION_NO_FONTSIZE |
        PURPLE_CONNECTION_NO_URLDESC |
        PURPLE_CONNECTION_NO_IMAGES;

    ki = new kiClient(account);
    pc->proto_data = ki;

    ki->connect();
}

void ki_close(PurpleConnection* pc) {
    kiClient* ki = pc->proto_data;
    ki->disconnect();
    delete ki;

    pc->proto_data = NULL;
}

gboolean ki_offline_message(const PurpleBuddy* buddy) {
    return FALSE;
}
