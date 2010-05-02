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

#include "kiClient_cb.cpp"

kiClient::kiClient(PurpleConnection* pc, PurpleAccount* account) {
    fBuildID = 0;

    fAddresses[kGate] = "184.73.198.22";
    fAddresses[kAuth] = "184.73.198.22";
    fConnectFunc[kGate] = gate_connect;
    fConnectFunc[kFile] = file_connect;
    fConnectFunc[kAuth] = auth_connect;
    /* fConnectFunc[kGame] = game_connect; */

    fConnection = pc;
    fAccount = account;
}

kiClient::~kiClient() {
    int i = 0;
    for (i = 0; i < kNumClients; i++) {
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
    fNetMutex.unlock();
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
	purple_connection_update_progress(fConnection, _("GateKeeper"), 0, 3);

    if (purple_proxy_connect(this, fAccount, fAddresses[kGate], KI_PORT,
                fConnectFunc[kGate], this) == NULL)
    {
        this->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                _("Unable to connect."));
    }
}

void kiClient::disconnect() {
    /* if we are connected to a GameSrv, send some logout message */
    this->killClient(kGame);

    /* if we are connected to the AuthSrv, mark ourselves offline */
    if (fClients[kAuth] != NULL && fClients[kAuth]->isConnected()) {
        this->killClient(kAuth);
    }

    /* Disconnect from the FileSrv */
    if (fClients[kFile] != NULL && fClients[kFile]->isConnected()) {
        this->killClient(kFile);
    }
}

void kiClient::ping() {
    if (fClients[kFile] != NULL && fClients[kFile]->isConnected()) {
        ((kiFileClient*)fClients[kFile])->ping();
    }
    if (fClients[kAuth] != NULL && fClients[kAuth]->isConnected()) {
        ((kiAuthClient*)fClients[kAuth])->ping();
    }
}

void kiClient::update_buddy(pnVaultPlayerInfoNode* node) {
    kiVaultBuddy* buddy = new kiVaultBuddy();

    buddy->fPlayerKI = node->getPlayerId();
    buddy->fPlayerName = node->getPlayerName();
    buddy->fLocation = node->getAgeInstName();
    buddy->fOnline = node->getOnline();

    fBuddies[node->getNodeIdx()] = buddy;

    clo_Buddy* closure = new clo_Buddy();
    closure->client = this;
    closure->buddy = buddy;
    g_idle_add(ki_update_buddy, closure);
}

void kiClient::gate_file_callback() {
	purple_connection_update_progress(fConnection, _("FileSrv"), 1, 3);

    if (purple_proxy_connect(this, fAccount, fAddresses[kFile], KI_PORT,
                fConnectFunc[kFile], this) == NULL)
    {
        this->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                _("Unable to connect."));
    }
}

void kiClient::file_build_callback() {
	purple_connection_update_progress(fConnection, _("AuthSrv"), 2, 3);

    if (purple_proxy_connect(this, fAccount, fAddresses[kAuth], KI_PORT,
                fConnectFunc[kAuth], this) == NULL)
    {
        this->set_error(PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                _("Unable to connect."));
    }
}

void kiClient::set_error(PurpleConnectionError e, const char* msg) {
    purple_connection_error_reason(fConnection, e, msg);
}

void kiClient::set_error(CliType client, ENetError e) {
    /* This MUST call set_error (above) using g_idle_add!!! */
}

void kiClient::setAddress(CliType client, const plString address) {
    switch (client) {
        case kGate:
            fAddresses[kGate] = address;
            return;
        case kAuth:
            fAddresses[kAuth] = address;
            return;
        case kFile:
            fAddresses[kFile] = address;
            return;
        case kGame:
            fAddresses[kGame] = address;
            return;
        default:
            return;
    }
}

void kiClient::initClient(CliType client) {
    switch (client) {
        case kGate:
            fClients[kGate] = new kiGateClient(this);
        case kAuth:
            fClients[kAuth] = new kiAuthClient(this);
        case kFile:
            fClients[kFile] = new kiFileClient(this);
        case kGame:
            fClients[kGame] = NULL; /* new kiGameClient(this) */
        default:
            return;
    }
}

pnClient* kiClient::getClient(CliType client) const {
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

void kiClient::killClient(CliType client) {
    if (client > kGame) {
        return;
    }

    pnClient* cli = fClients[client];
    if (cli != NULL && cli->isConnected()) {
        cli->disconnect();
    }
    delete cli;
    fClients[client] = NULL;
}

void kiClient::requestKey(plString name) {
    plString filename;

    if (!fClients[kFile]->isConnected()) {
        this->set_error(kFile, kNetErrDisconnected);
    }

    if (!name.compareTo("male", TRUE) || !name.compareTo("female", TRUE)) {
        filename = "GlobalAvatars";
    } else {
        filename = "CustomAvatars";
    }
}

const plString kiClient::getUsername() const {
    const char* fullname = purple_account_get_username(fAccount);
    char** usersplits = g_strsplit(fullname, "/", 2);
    char* username = g_utf8_strdown(usersplits[0], -1);

    g_free(usersplits);

    return plString(username);
}

const plString kiClient::getPassword() const {
    return plString(purple_connection_get_password(fConnection));
}

const hsUint32 kiClient::getKINumber() const {
    const char* fullname = purple_account_get_username(fAccount);
    char** usersplits = g_strsplit(fullname, "/", 2);
    hsUint32 kiNumber = plString(usersplits[1]).toInt();
    g_free(usersplits);

    return kiNumber;
}
