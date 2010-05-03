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

#ifndef _KI_AUTHCLIENT_H
#define _KI_AUTHCLIENT_H

#include "kiHeaders.h"
#include "kiClient.h"
#include <auth/pnAuthClient.h>
#include <auth/pnVaultNode.h>
#include <auth/pnVaultNodeTypes.hpp>

class kiAuthClient : public pnAuthClient {
private:
    enum Nodes {
        kBuddyList,
        kIgnoreList,
        kHoodOwners,
        kNumNodes
    };

public:
    kiAuthClient(kiClient* master);
    virtual ~kiAuthClient();

    void setCredentials(plString username, plString password);
    void process();
    void ping();

    virtual void onPingReply(hsUint32 transId, hsUint32 pingTimeMs);
    virtual void onServerAddr(hsUint32 address, const plUuid& token);
    virtual void onClientRegisterReply(hsUint32 serverChallenge);
    virtual void onAcctLoginReply(hsUint32 transId, ENetError result,
                    const plUuid& acctUuid, hsUint32 acctFlags,
                    hsUint32 billingType, const hsUint32* encryptionKey);
    virtual void onAcctPlayerInfo(hsUint32 transId, hsUint32 playerId,
                    const plString& playerName, const plString& avatarModel,
                    hsUint32 explorer);
    virtual void onAcctSetPlayerReply(hsUint32 transId, ENetError result);
    virtual void onVaultNodeRefsFetched(hsUint32 transId, ENetError result,
                    size_t count, const pnVaultNodeRef* refs);
    virtual void onVaultNodeFetched(hsUint32 transId, ENetError result,
                    const pnVaultNode& node);
    virtual void onVaultNodeChanged(hsUint32 nodeId, const plUuid& revisionId);
    virtual void onVaultSaveNodeReply(hsUint32 transId, ENetError result);

private:
    kiClient* fMaster;
    hsMutex fVaultMutex;
    hsThreadCondition fCondChallenge;
    hsThreadCondition fCondPlayers;
    hsThreadCondition fCondActive;
    guint fTimeout;

    hsUint32 fServerChallenge;
    hsUint32 fClientChallenge;
    plString fUsername;
    plString fPassword;
    hsUint32 fPlayerID;
    plString fPlayerName;
    plString fPlayerModel;
    plUuid fAccountUuid;
    std::map<hsUint32, std::list<hsUint32> > fRefs;
    pnVaultPlayerInfoNode* fSelf;
    hsUint32 fNodeIDs[kNumNodes];
};

#endif

