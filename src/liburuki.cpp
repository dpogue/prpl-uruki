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

#include "kiHeaders.h"
#include "kiPurple.h"

PurplePluginProtocolInfo prpl_info = {
    (PurpleProtocolOptions)0,               /* options (none) */
    NULL,                                   /* user_splits */
    NULL,                                   /* protocol_options */
    {                                       /* icon_spec (none) */
        NULL,
        0,
        0,
        0,
        0,
        0,
        (PurpleIconScaleRules)0
    },                                      /* end icon_spec */
    ki_list_icon,                           /* list_icon */
    NULL,                                   /* list_emblem */
    ki_status_text,                         /* status_text */
    NULL,                                   /* tooltip_text */
    ki_status_types,                        /* status_types */
    NULL,                                   /* blist_node_menu */
    NULL,                                   /* chat_info */
    NULL,                                   /* chat_info_defaults */
    ki_login,                               /* login */
    ki_close,                               /* close */
    ki_send_pm,                             /* send_im */
    NULL,                                   /* set_info */
    NULL,                                   /* send_typing */
    ki_get_info,                            /* get_info */
    ki_set_status,                          /* set_status */
    NULL,                                   /* set_idle */
    NULL,                                   /* change_passwd */
    ki_add_buddy,                           /* add_buddy */
    NULL,                                   /* add_buddies */
    ki_remove_buddy,                        /* remove_buddy */
    NULL,                                   /* remove_buddies */
    NULL,                                   /* add_permit */
    ki_ignore,                              /* add_deny */
    NULL,                                   /* rem_permit */
    ki_unignore,                            /* rem_deny */
    NULL,                                   /* set_permit_deny */
    NULL,                                   /* join_chat */
    NULL,                                   /* reject_chat */
    NULL,                                   /* get_chat_name */
    NULL,                                   /* chat_invite */
    NULL,                                   /* chat_leave */
    NULL,                                   /* chat_whisper */
    NULL,                                   /* chat_send */
    ki_keepalive,                           /* keepalive */
    NULL,                                   /* register_user */
    NULL,                                   /* get_cb_info - deprecated */
    NULL,                                   /* get_cb_away - deprecated */
    NULL,                                   /* alias_buddy */
    NULL,                                   /* group_buddy */
    NULL,                                   /* rename_group */
    NULL,                                   /* buddy_free */
    NULL,                                   /* convo_closed */
    ki_normalise,                           /* normalize */
    NULL,                                   /* set_buddy_icon */
    NULL,                                   /* remove_group */
    NULL,                                   /* get_cb_real_name */
    NULL,                                   /* set_chat_topic */
    NULL,                                   /* find_blist_chat */
    NULL,                                   /* roomlist_get_list */
    NULL,                                   /* roomlist_cancel */
    NULL,                                   /* roomlist_expand_category */
    NULL,                                   /* can_receive_file */
    NULL,                                   /* send_file */
    NULL,                                   /* new_xfer */
    ki_offline_message,                     /* offline_message */
    (PurpleWhiteboardPrplOps*)NULL,         /* whiteboard_prpl_ops */
    NULL,                                   /* send_raw */
    NULL,                                   /* roomlist_room_serialize */
    NULL,                                   /* unregister_user */
    NULL,                                   /* send_attention */
    NULL,                                   /* get_attention_types */
    sizeof(PurplePluginProtocolInfo),       /* structure size */
    NULL,                                   /* get_account_text_table */
    NULL,                                   /* initiate_media */
    NULL,                                   /* get_media_caps */
    NULL,                                   /* get_moods */
    NULL,                                   /* set_public_alias */
    NULL                                    /* get_public_alias */
};

static PurplePluginInfo info =
{
	PURPLE_PLUGIN_MAGIC,
	PURPLE_MAJOR_VERSION,
	PURPLE_MINOR_VERSION,
	PURPLE_PLUGIN_PROTOCOL,       /* plugin type */
	NULL,                         /* UI requirements */
	0,                            /* Flags */
	NULL,                         /* dependencies */
	PURPLE_PRIORITY_DEFAULT,      /* priority */
	g_strdup(KI_IDNAME),          /* prpl id */
	g_strdup("Uru KI Network"),   /* prpl name */
	g_strdup(KI_VERSION),         /* version */
	
    /* Summary */
	_("Uru KI Protocol Plugin"),
    /* Description */
	_("Connect to the cavern while you're on the surface."),
    /* Author */
	g_strdup(KI_AUTHOR),
    /* Homepage */
	_("http://mystonline.com/"),

	NULL,                         /* load */
	NULL,                         /* unload */
	NULL,                         /* destroy */
	NULL,                         /* UI info */
	&prpl_info,                   /* extra info */
	NULL,                         /* prefs info */
	NULL,                         /* actions */
	NULL,                         /* padding */
	NULL,                         /* padding */
	NULL,                         /* padding */
	NULL                          /* padding */
};

static void _init_plugin(PurplePlugin *plugin)
{
#ifdef DEBUG
    plDebug::Init(plDebug::kDLAll);
#endif

	PurpleAccountUserSplit *split;

	split = purple_account_user_split_new(_("KI Number"), "", '/');
	prpl_info.user_splits = g_list_append(prpl_info.user_splits, split);

}

#ifdef __cplusplus
extern "C" {
#endif
PURPLE_INIT_PLUGIN(uruki, _init_plugin, info);
#ifdef __cplusplus
}
#endif
