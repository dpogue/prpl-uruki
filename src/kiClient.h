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

#ifndef _KI_CLIENT_H
#define _KI_CLIENT_H

#include "kiDefines.h"
#include <list>

class kiClient {
public:
    enum ServType {
        kGate,
        kAuth,
        kFile,
        kGame,
        kNumServers
    };

    kiClient(PurpleAccount* account);
    virtual ~kiClient();

    void push(hsUint32 transID);
    void pop(hsUint32 transID);

    void connect();
    void disconnect();

    gboolean gate_file_callback(gpointer data);
    gboolean file_build_callback(gpointer data);

private:
    std::list<hsUint32> fTransactions;
    hsMutex fNetMutex;

    PurpleAccount* fAccount;
    plString fGateAddr;
    plString fAuthAddr;
    plString fFileAddr;
    plString fGameAddr;
    pnClient* fClients[kNumServers];
    PurpleProxyConnectFunction fConnectFuncs[kNumServers];
    hsUint32 fBuildID;

public:
    void setAddress(ServType server, const plString address);

    pnClient* getClient(ServType client);
    void killClient(ServType client);

    hsUint32 getBuildID() const { return fBuildID; }
    void setBuildID(const hsUint32 buildID) { fBuildID = buildID; }
};

#endif
