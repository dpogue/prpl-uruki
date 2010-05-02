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

static void gate_connect(gpointer data, gint fd, const gchar* error) {
    kiClient* client = (kiClient*)data;
    gchar* tmp;
    ENetError err;

    if (fd < 0) {
        tmp = g_strdup_printf(_("Unable to connect: %s"), error);
        client->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    client->initClient(kiClient::kGate);

    if ((err = client->getClient(kiClient::kGate)->connect(fd))
            != kNetSuccess) {
        client->set_error(kiClient::kGate, err);
        return;
    }

    ((kiGateClient*)client->getClient(kiClient::kGate))->process();

    if (!client->getClient(kiClient::kGate)->isConnected()) {
        tmp = g_strdup_printf(_("Network Error: %s"), "GateKeeperSrv");
        client->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    } else {
        client->killClient(kiClient::kGate);
    }
}

static void file_connect(gpointer data, gint fd, const gchar* error) {
    kiClient* client = (kiClient*)data;
    gchar* tmp;
    ENetError err;

    if (fd < 0) {
        tmp = g_strdup_printf(_("Unable to connect: %s"), error);
        client->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    client->initClient(kiClient::kFile);

    if ((err = client->getClient(kiClient::kFile)->connect(fd))
            != kNetSuccess) {
        client->set_error(kiClient::kFile, err);
        return;
    }

    ((kiFileClient*)client->getClient(kiClient::kFile))->process();

    if (!client->getClient(kiClient::kFile)->isConnected()) {
        tmp = g_strdup_printf(_("Network Error: %s"), "FileSrv");
        client->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }
}

static void auth_connect(gpointer data, gint fd, const gchar* error) {
    kiClient* client = (kiClient*)data;
    gchar* tmp;
    ENetError err;

    if (fd < 0) {
        tmp = g_strdup_printf(_("Unable to connect: %s"), error);
        client->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    client->initClient(kiClient::kAuth);

    ((kiAuthClient*)client->getClient(kiClient::kAuth))->setCredentials(
        client->getUsername(), client->getPassword());

    if ((err = client->getClient(kiClient::kAuth)->connect(fd))
            != kNetSuccess) {
        client->set_error(kiClient::kAuth, err);
        return;
    }

    ((kiAuthClient*)client->getClient(kiClient::kAuth))->process();

    if (!client->getClient(kiClient::kAuth)->isConnected()) {
        tmp = g_strdup_printf(_("Network Error: %s"), "AuthSrv");
        client->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }
}

struct clo_Buddy {
    kiClient* client;
    kiVaultBuddy* buddy;
};

static gboolean ki_update_buddy(gpointer data) {
    clo_Buddy* closure = (clo_Buddy*)data;
    kiClient* client = closure->client;
    kiVaultBuddy* buddy = closure->buddy;
    PurpleBuddy* pbuddy = NULL;

    char* id = g_strdup_printf("%d", buddy->fPlayerKI);
    if ((pbuddy = purple_find_buddy(client->getAccount(), id)) == NULL) {
        PurpleGroup* g = purple_find_group(_("Buddies"));
        pbuddy = purple_buddy_new(client->getAccount(), id,
                                    NULL);
        
        if (!g) {
            g = purple_group_new(_("Buddies"));
            purple_blist_add_group(g, NULL);
        }

        purple_blist_add_buddy(pbuddy, NULL, g, NULL);
    }
	purple_buddy_set_protocol_data(pbuddy, buddy);
    serv_got_alias(client->getConnection(), id, buddy->fPlayerName.cstr());

    if (buddy->fOnline) {
        purple_prpl_got_user_status(client->getAccount(), id,
                "b_online", "location", buddy->fLocation.cstr(), NULL);
    } else {
        purple_prpl_got_user_status(client->getAccount(), id,
                "b_offline", NULL);
    }

    delete closure;

    return FALSE;
}
