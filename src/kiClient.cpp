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

#include "kiClient.h"
#include "kiGateClient.h"
#include "kiFileClient.h"
#include "kiAuthClient.h"

static void gate_connect(gpointer data, gint fd, const gchar* error) {
    kiClient* client = data;
    gchar* tmp;
    ENetError err;

    if (fd < 0) {
        tmp = g_strdup_printf(_("Unable to connect: %s"), error);
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    if ((err = client->getClient(kGate)->connect(fd)) != kNetSuccess) {
        tmp = g_strdup_printf(_("Unable to connect: %s"),
                GetNetErrorString(err));
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    client->getClient(kGate)->process();

    if (!client->getClient(kGate)->isConnected()) {
        tmp = g_strdup_printf(_("Network Error: %s"), "GateKeeperSrv");
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    } else {
        client->killClient(kGate);
    }
}

static void file_connect(gpointer data, gint fd, const gchar* error) {
    kiClient* client = data;
    gchar* tmp;
    ENetError err;

    if (fd < 0) {
        tmp = g_strdup_printf(_("Unable to connect: %s"), error);
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    if ((err = client->getClient(kFile)->connect(fd)) != kNetSuccess) {
        tmp = g_strdup_printf(_("Unable to connect: %s"),
                GetNetErrorString(err));
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    client->getClient(kFile)->process();

    if (!client->getClient(kFile)->isConnected()) {
        tmp = g_strdup_printf(_("Network Error: %s"), "FileSrv");
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }
}

static void auth_connect(gpointer data, gint fd, const gchar* error) {
    kiClient* client = data;
    gchar* tmp;
    ENetError err;

    if (fd < 0) {
        tmp = g_strdup_printf(_("Unable to connect: %s"), error);
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    if ((err = client->getClient(kAuth)->connect(fd)) != kNetSuccess) {
        tmp = g_strdup_printf(_("Unable to connect: %s"),
                GetNetErrorString(err));
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }

    client->getClient(kAuth)->process();

    if (!client->getClient(kAuth)->isConnected()) {
        tmp = g_strdup_printf(_("Network Error: %s"), "AuthSrv");
        purple_connection_error_reason(
                purple_account_get_connection(client->getAccount()),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR, tmp);
        g_free(tmp);
        return;
    }
}

kiClient::kiClient(PurpleConnection* pc, PurpleAccount* account) {
    fGateAddr = "184.73.198.22";
    fAuthAddr = "184.73.198.22";
    fBuildID = 0;

    fClients[kGate] = new kiGateClient(this);
    fClients[kFile] = new kiFileClient(this);
    fClients[kAuth] = new kiAuthClient(this);
    fClients[kGame] = NULL; /* new kiGameClient(this); */

    fConnectFunc[kGate] = gate_connect;
    fConnectFunc[kFile] = file_connect;
    fConnectFunc[kAuth] = auth_connect;
    /* fConnectFunc[kGame] = game_connect; */

    fConnection = pc;
    fAccount = account;
}

kiClient::~kiClient() {
    int i = 0;
    for (i = 0; i < kNumServers; i++) {
        pnClient* cli = fClients[i];
        if (cli == NULL) {
            continue;
        }

        if (cli->isConnected()) {
            cli->disconnect();
        }
        delete cli;
        fClients[i] = NULL;
    }

    fAccount = NULL;
    fConnection = NULL;
}

void kiClient::push(hsUint32 transID) {
    fNetMutex.lock();
    fTransactions.push_back(transID);
    fnetMutex.unlock();
}

void kiClient::pop(hsUint32 transID) {
    fNetMutex.lock(); 
    std::list<hsUint32>::iterator i;
    for (i = fTransactions.begin(); i != fTransactions.end(); i++) {
        if (*i == transID) {
            fTransactions.erase(i);
            break;
        }
    }
    fNetMutex.unlock();
}

void kiClient::connect() {
    if (purple_proxy_connect(this, this->fAccount, this->fGateAddr,
             14617, this->fConnectFunc[kGate], this) == NULL) {
        purple_connection_error_reason(
                purple_account_get_connection(this->fAccount),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                _("Unable to connect"));
        return;
    }
}

void kiClient::disconnect() {
    /* if we are connected to a GameSrv, send some logout message */
    this->killClient(kGame);

    /* if we are connected to the AuthSrv, mark ourselves offline */
    if (fClients[kAuth] != NULL && fCLient[kAuth]->isConnected()) {
        this->killClient(kAuth);
    }

    /* Disconnect from the FileSrv */
    if (fClients[kFile] != NULL && fClients[kFile]->isConnected()) {
        this->killClient(kFile);
    }
}

gboolean kiClient::gate_file_callback(gpointer data) {
    if (purple_proxy_connect(this, this->fAccount, this->fFileAddr,
             14617, this->fConnectFunc[kFile], this) == NULL) {
        purple_connection_error_reason(
                purple_account_get_connection(this->fAccount),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                _("Unable to connect"));
        return FALSE;
    }
    return TRUE;
}

gboolean kiClient::file_build_callback(gpointer data) {
    if (purple_proxy_connect(this, this->fAccount, this->fAuthAddr,
             14617, this->fConnectFunc[kAuth], this) == NULL) {
        purple_connection_error_reason(
                purple_account_get_connection(this->fAccount),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                _("Unable to connect"));
        return FALSE;
    }
    return TRUE;
}

void kiClient::setAddress(ServType server, const plString address) {
    switch (server) {
        case kGate:
            fGateAddr = address;
            return;
        case kAuth:
            fAuthAddr = address;
            return;
        case kFile:
            fFileAddr = address;
            return;
        case kGame:
            fGameAddr = address;
            return;
        default:
            return;
    }
}

pnClient* kiClient::getClient(ServType client) {
    switch (client) {
        case kGate:
            return fClients[kGate];
        case kAuth:
            return fClients[kAuth];
        case kFile:
            return fClients[kFile];
        case kGame:
            return fClients[kGame];
        default:
            return NULL;
    }
}

void kiClient::killClient(ServType client) {
    if (client > kGame) {
        return;
    }

    pnClient* cli = fClients[client];
    if (cli != NULL && cli->isConnected()) {
        cli->disconnect();
    }
    delete cli;
    fClient[client] = NULL;
}

const char* kiClient::getUsername() const {
    const char* fullname = purple_account_get_username(fAccount);
    char** usersplits = g_strsplit(fullname, "/", 2);
    char* username = g_utf8_strdown(usersplits[0], -1);

    g_free(usersplits);

    return username;
}

const char* kiClient::getPassword() const {
    return purple_connection_get_password(fConnection);
}

const hsUint32 kiClient::getKINumber() const {
    const char* fullname = purple_account_get_username(fAccount);
    char** usersplits = g_strsplit(fullname, "/", 2);
    hsUint32 kiNumber = plString(usersplits[1]).toInt();
    g_free(usersplits);

    return kiNumber;
}
