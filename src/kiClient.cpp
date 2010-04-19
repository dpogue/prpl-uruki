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
}

kiClient::kiClient(PurpleAccount* account) {
    fGateAddr = "184.73.198.22";
    fAuthAddr = "184.73.198.22";
    fBuildID = 0;

    fClients[kGate] = new kiGateClient(this);
    // Other clients

    fConnectFunc[kGate] = gate_connect;
    // Other callbacks

    fAccount = account;
}

void push(hsUint32 transID) {
    fNetMutex.lock();
    fTransactions.push_back(transID);
    fnetMutex.unlock();
}

void pop(hsUint32 transID) {
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

void connect() {
    if (purple_proxy_connect(this, this->fAccount, this->fGateAddr,
             14617, this->fConnectFunc[kGate], this) == NULL) {
        purple_connection_error_reason(
                purple_account_get_connection(this->fAccount),
                PURPLE_CONNECTION_ERROR_NETWORK_ERROR,
                _("Unable to connect"));
        return;
    }
}

gboolean kiClient::gate_callback(gpointer data) {
    /* Do FileClient connect */
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
