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

#include "kiAuthClient.h"

/* Auth server public keys */
hsUbyte KEY_Auth_N[] = { 0xA7, 0x72, 0xE4, 0x39, 0xE9, 0x8D, 0x2E, 0x13,
                         0x41, 0x08, 0x92, 0x54, 0x29, 0xE6, 0x38, 0x58,
                         0xEF, 0x81, 0xD9, 0x86, 0x7D, 0x77, 0x4E, 0x5E,
                         0x69, 0x1D, 0x91, 0x12, 0x65, 0x12, 0xC8, 0x10,
                         0x1C, 0x73, 0xB8, 0x90, 0x5D, 0x7A, 0x7B, 0x4E,
                         0xB6, 0x7C, 0xF9, 0xFE, 0x11, 0x30, 0xDB, 0xFF,
                         0xED, 0x10, 0xCE, 0x12, 0xA0, 0xF4, 0xFF, 0x32,
                         0xBA, 0x3A, 0x8D, 0x44, 0xF5, 0x62, 0xBA, 0x8B };
hsUbyte KEY_Auth_X[] = { 0xF1, 0x80, 0x28, 0xB1, 0xAD, 0x66, 0xBB, 0xCB,
                         0x6F, 0xCD, 0x53, 0x0F, 0x0F, 0x0A, 0x1A, 0xF4,
                         0xB1, 0x26, 0x3E, 0xB5, 0x03, 0x96, 0xE7, 0xAC,
                         0x1D, 0x58, 0x5F, 0x47, 0x18, 0x9F, 0x08, 0x72,
                         0xA3, 0x51, 0x39, 0x45, 0xF0, 0x8F, 0xD5, 0x36,
                         0x3A, 0x00, 0xD9, 0x17, 0x03, 0x69, 0x93, 0x2A,
                         0x65, 0x21, 0x29, 0x45, 0x04, 0xC8, 0x58, 0xE9,
                         0x50, 0xB6, 0xC9, 0x25, 0x74, 0x80, 0x6D, 0x47 };

static gboolean auth_timeout(gpointer data) {
    kiClient* cli = (kiClient*)data;
    cli->set_error(kiClient::kAuth, kNetErrTimeout);

    return FALSE;
}

static gboolean auth_setactive(gpointer data) {
    kiClient* cli = (kiClient*)data;
    purple_connection_set_state(cli->getConnection(), PURPLE_CONNECTED);

    return FALSE;
}

kiAuthClient::kiAuthClient(kiClient* master)
    : pnAuthClient(master->getResManager()) {
    setKeys(KEY_Auth_X, KEY_Auth_N);
    setClientInfo(master->getBuildID(), KI_BUILDTYPE, KI_BRANCHID, KI_UUID);

    fMaster = master;
    fClientChallenge = (hsUint32)rand();
    fTimeout = 0;
    fPlayerID = master->getKINumber();
}

kiAuthClient::~kiAuthClient() {
    fDisposing = TRUE;

    if (!fPlayerName.empty()) {
        pnVaultPlayerInfoNode* node = new pnVaultPlayerInfoNode();
        node->setNodeIdx(fNodeIDs[kPlayerInfo]);
        node->setAgeInstName(plString());
        node->setOnline(0);
        this->sendVaultNodeSave(fNodeIDs[kPlayerInfo], plUuid(),
                (const pnVaultNode&)*node);
        fCondLogout.wait();
    }
    fMaster = NULL;
    if (fTimeout != 0) {
        g_source_remove(fTimeout);
        fTimeout = 0;
    }
}

void kiAuthClient::setCredentials(plString username, plString password) {
    fUsername = username;
    fPassword = password;
}

void kiAuthClient::process() {
    if (!this->isConnected()) return;
    this->ping();

    this->sendClientRegisterRequest();
    if (!this->isConnected()) return;
    fCondChallenge.wait();

    fMaster->push(this->sendAcctLoginRequest(
                fServerChallenge, fClientChallenge, fUsername, fPassword,
                "", "linux"));
    if (!this->isConnected()) return;
    fCondPlayers.wait();

    if (fAccountUuid.isNull()) return;
    if (fPlayerName.empty()) {
        fMaster->set_error(kiClient::kAuth, kNetErrPlayerNotFound);
        return;
    }

    fMaster->push(this->sendAcctSetPlayerRequest(fPlayerID));
    if (!this->isConnected()) return;
    fCondActive.wait();

    g_idle_add(auth_setactive, fMaster);

    fMaster->push(this->sendVaultFetchNodeRefs(fPlayerID));
    if (!this->isConnected()) return;
}

void kiAuthClient::ping() {
    if (fTimeout == 0) {
        fMaster->push(this->sendPingRequest(0));
        fTimeout = g_timeout_add_seconds(KI_TIMEOUT, auth_timeout, fMaster);
    }
}

void kiAuthClient::onPingReply(hsUint32 transId, hsUint32 pingTimeMs) {
    if (fTimeout != 0) {
        g_source_remove(fTimeout);
        fTimeout = 0;
    }
    fMaster->pop(transId);
}

void kiAuthClient::onServerAddr(hsUint32 address, const plUuid& token) {
    plDebug::Debug("Auth Server address %x; %s", address,
            token.toString().cstr());
}

void kiAuthClient::onClientRegisterReply(hsUint32 serverChallenge) {
    fServerChallenge = serverChallenge;
    
    fCondChallenge.signal();
}

void kiAuthClient::onAcctLoginReply(hsUint32 transId, ENetError result,
        const plUuid& acctUuid, hsUint32 acctFlags, hsUint32 billingType,
        const hsUint32* encryptionKey) {
    fMaster->pop(transId);

    if (result != kNetSuccess) {
        fCondPlayers.signal();
        fMaster->set_error(kiClient::kAuth, result);
        return;
    }

    fAccountUuid = acctUuid;

    fCondPlayers.signal();
}

void kiAuthClient::onAcctPlayerInfo(hsUint32 transId, hsUint32 playerId,
        const plString& playerName, const plString& avatarModel,
        hsUint32 explorer) {
    if (explorer && playerId == fPlayerID) {
        fPlayerName = playerName;
        fPlayerModel = avatarModel;
    }
}

void kiAuthClient::onAcctSetPlayerReply(hsUint32 transId, ENetError result) {
    fMaster->pop(transId);
    fCondActive.signal();

    if (result != kNetSuccess) {
        fMaster->set_error(kiClient::kAuth, result);
    }
}

void kiAuthClient::onVaultNodeRefsFetched(hsUint32 transId,
        ENetError result, size_t count, const pnVaultNodeRef* refs) {
    fMaster->pop(transId);

    if (result != kNetSuccess) {
        fMaster->set_error(kiClient::kAuth, result);
        return;
    }

    fVaultMutex.lock();
    for (size_t i = 0; i < count; i++) {
        fRefs[refs[i].fParent].push_back(refs[i].fChild);
    }

    /* Fetch all direct children of the PlayerNode */
    std::list<hsUint32>::iterator i;
    for (i = fRefs[fPlayerID].begin(); i != fRefs[fPlayerID].end(); i++) {
        fMaster->push(this->sendVaultNodeFetch(*i));
    }
    fVaultMutex.unlock();
}

void kiAuthClient::onVaultNodeFetched(hsUint32 transId, ENetError result,
        const pnVaultNode& node) {
    fMaster->pop(transId);

    if (result != kNetSuccess) {
        fMaster->set_error(kiClient::kAuth, result);
        return;
    }

    fVaultMutex.lock();
    pnVaultNode tnode = node;
    if (pnVaultPlayerInfoListNode::Convert(&tnode) != NULL) {
        pnVaultPlayerInfoListNode* vnode;
        vnode = pnVaultPlayerInfoListNode::Convert(&tnode);
        hsUint32 idx = vnode->getNodeIdx();
        std::list<hsUint32>::iterator i;

        switch (vnode->getFolderType()) {
            case (plVault::kBuddyListFolder):
                fNodeIDs[kBuddyList] = idx;
                for (i = fRefs[idx].begin(); i != fRefs[idx].end(); i++) {
                    fMaster->push(this->sendVaultNodeFetch(*i));
                }
                break;
            case (plVault::kIgnoreListFolder):
                fNodeIDs[kIgnoreList] = idx;
                /* TODO */
                break;
            default:
                break;
        }
    } else if (pnVaultPlayerInfoNode::Convert(&tnode) != NULL) {
        pnVaultPlayerInfoNode* vnode;
        vnode = pnVaultPlayerInfoNode::Convert(&tnode);

        if (vnode->getPlayerId() == fPlayerID) {
            fNodeIDs[kPlayerInfo] = vnode->getNodeIdx();
            vnode->setOnline(1);
            vnode->setAgeInstName("D'ni-Riltagamin");
            fMaster->push(this->sendVaultNodeSave(vnode->getNodeIdx(), 
                        plUuid(), (const pnVaultNode&)*vnode));
        } else {
            /* Hacks to only add buddies -_- */
            hsUint32 bl = fNodeIDs[kBuddyList];
            std::list<hsUint32>::iterator i;
            for (i = fRefs[bl].begin(); i != fRefs[bl].end(); i++) {
                if (vnode->getNodeIdx() == *i) {
                    fMaster->update_buddy(vnode);
                }
            }
        }
    }
    fVaultMutex.unlock();
}

void kiAuthClient::onVaultNodeChanged(hsUint32 nodeId,
        const plUuid& revisionId) {
    if (fNodeIDs[kBuddyList] != 0) {
        std::list<hsUint32>::iterator i;
        for (i = fRefs[fNodeIDs[kBuddyList]].begin();
             i != fRefs[fNodeIDs[kBuddyList]].end();
             i++)
        {
            if (nodeId == *i) {
                fMaster->push(this->sendVaultNodeFetch(*i));
                return;
            }
        }
    }
    if (fNodeIDs[kIgnoreList] != 0) {
        /* TODO */
    }
}

void kiAuthClient::onVaultSaveNodeReply(hsUint32 transId,
        ENetError result) {
    fMaster->pop(transId);

    if (result != kNetSuccess) {
        fMaster->set_error(kiClient::kAuth, result);
    }

    if (fDisposing) {
        fCondLogout.signal();
    }
}
