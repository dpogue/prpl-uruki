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

#ifndef _KI_PURPLE_H
#define _KI_PURPLE_H

#include "kiDefines.h"
#include "kiClient.h"

#ifdef __cplusplus
extern "C" {
#endif

const char* ki_list_icon(PurpleAccount* account, PurpleBuddy* buddy);

char* ki_status_text(PurpleBuddy* buddy);

GList* ki_status_types(PurpleAccount* account);

void ki_login(PurpleAccount* account);

void ki_close(PurpleConnection* pc);

int ki_send_pm(PurpleConnection* pc, const char* who,
                const char* message, PurpleMessageFlags flags);
                
void ki_get_info(PurpleConnection* pc, const char* who);

void ki_set_status(PurpleAccount* account, PurpleStatus* status);

void ki_add_buddy(PurpleConnection* pc, PurpleBuddy* buddy,
                    PurpleGroup* group);

void ki_add_buddies(PurpleConnection* pc, GList* buddies, GList* groups);

void ki_remove_buddy(PurpleConnection* pc, PurpleBuddy* buddy,
                      PurpleGroup* group);

void ki_ignore(PurpleConnection* pc, const char* name);

void ki_unignore(PurpleConnection* pc, const char* name);

void ki_keepalive(PurpleConnection* pc);

const char* ki_normalise(const PurpleAccount* account, const char* who);

gboolean ki_offline_message(const PurpleBuddy* buddy);

#ifdef __cplusplus
}
#endif

#endif
