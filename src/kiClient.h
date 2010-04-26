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

#include "kiHeaders.h"
#include <list>

struct kiVaultBuddy {
    hsUint32 fPlayerKI;
    plString fPlayerName;
    plString fLocation;
    hsInt32 fOnline;
};

class kiClient {
public:
    enum ServType {
        kGate,
        kAuth,
        kFile,
        kGame,
        kNumServers
    };

    kiClient(PurpleConnection* pc, PurpleAccount* account);
    virtual ~kiClient();

    void push(hsUint32 transID);
    void pop(hsUint32 transID);

    void connect();
    void disconnect();

    void gate_file_callback();
    void file_build_callback();

private:
    std::list<hsUint32> fTransactions;
    hsMutex fNetMutex;

    PurpleAccount* fAccount;
    PurpleConnection* fConnection;
    std::map<hsUint32, kiVaultBuddy> fBuddies;
    plString fGateAddr;
    plString fAuthAddr;
    plString fFileAddr;
    plString fGameAddr;
    pnClient* fClients[kNumServers];
    PurpleProxyConnectFunction fConnectFunc[kNumServers];
    hsUint32 fBuildID;

public:
    void setAddress(ServType server, const plString address);

    void initClient(ServType client);
    pnClient* getClient(ServType client);
    void killClient(ServType client);

    hsUint32 getBuildID() const { return fBuildID; }
    void setBuildID(const hsUint32 buildID) { fBuildID = buildID; }

    PurpleAccount* getAccount() const { return fAccount; }
    PurpleConnection* getConnection() const { return fConnection; }

    const plString getUsername() const;
    const plString getPassword() const;
    const hsUint32 getKINumber() const;
};

#endif
