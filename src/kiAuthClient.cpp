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
hsUbyte KEY_Auth_N[] = { 0x2D, 0x17, 0x19, 0x42, 0xEB, 0x71, 0x8F, 0x91,
                         0x29, 0x7C, 0x61, 0x88, 0x43, 0x75, 0xE5, 0xEE,
                         0x72, 0xFE, 0x45, 0x1B, 0x43, 0xC3, 0x8E, 0xB9,
                         0x47, 0x5E, 0x03, 0xC8, 0x0C, 0x78, 0xB7, 0xE4,
                         0x4D, 0x31, 0x5B, 0xCB, 0x66, 0xC2, 0x54, 0x1A,
                         0x0A, 0x61, 0x11, 0x57, 0x38, 0x66, 0x9B, 0x34,
                         0x6B, 0xAB, 0x6D, 0x12, 0x12, 0x38, 0x87, 0xC5,
                         0x3F, 0x20, 0xBE, 0x97, 0xA3, 0xA6, 0x56, 0x8F };
hsUbyte KEY_Auth_X[] = { 0x21, 0xCF, 0x1D, 0xC6, 0x08, 0xC0, 0x23, 0xAD,
                         0x53, 0x36, 0xCE, 0x61, 0x25, 0xDD, 0xB9, 0x55,
                         0x05, 0xC1, 0xBB, 0x8F, 0xF4, 0x0D, 0x59, 0xF9,
                         0x20, 0x27, 0x9A, 0xEE, 0xFB, 0x23, 0x5B, 0xEB,
                         0xE5, 0xEC, 0x01, 0x55, 0x2E, 0xD5, 0x64, 0xEF,
                         0xEA, 0x43, 0xB1, 0x9E, 0xB9, 0x8C, 0x75, 0x3F,
                         0xDA, 0xB2, 0xBB, 0xB3, 0x6A, 0x3D, 0xCD, 0xBC,
                         0xFA, 0x6F, 0x03, 0xF3, 0x55, 0xD8, 0xE9, 0x1B };

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

void kiAuthClient::addBuddy(hsUint32 playerId) {
    hsUint32 transId;
    pnVaultPlayerInfoNode* vnode = new pnVaultPlayerInfoNode();
    vnode->setPlayerId(playerId);

    transId = fMaster->push(this->sendVaultNodeFind((pnVaultNode&)*vnode));
    fTransAddBuddy.insert(transId);
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
            hsUint32 bidx = fNodeIDs[kBuddyList];
            std::list<hsUint32>::iterator i;
            for (i = fRefs[bidx].begin(); i != fRefs[bidx].end(); i++) {
                if (*i == vnode->getNodeIdx()) {
                    fMaster->update_buddy(vnode);
                }
            }
        }
    }
    fVaultMutex.unlock();
}

void kiAuthClient::onVaultNodeChanged(hsUint32 nodeId,
        const plUuid& revisionId) {
    /*if (fNodeIDs[kBuddyList] == nodeId) {
        fMaster->push(this->sendVaultNodeFetch(nodeId));
        return;
    }
    if (fNodeIDs[kBuddyList] != 0) {
        hsUint32 bidx = fNodeIDs[kBuddyList];
        std::list<hsUint32>::iterator i;
        for (i = fRefs[fNodeIDs[bidx]].begin();
                i != fRefs[fNodeIDs[bidx]].end(); i++) {
            if (nodeId == *i) {
                fMaster->push(this->sendVaultNodeFetch(*i));
                return;
            }
        }
    }
    if (fNodeIDs[kIgnoreList] != 0) {*/
        /* TODO */
    //}
    fMaster->push(this->sendVaultNodeFetch(nodeId));
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

void kiAuthClient::onVaultNodeFindReply(hsUint32 transId, ENetError result,
        size_t count, const hsUint32* nodes) {
    fMaster->pop(transId);

    if (result != kNetSuccess) {
        fMaster->set_error(kiClient::kAuth, result);
        return;
    }

    if (fTransAddBuddy.find(transId) != fTransAddBuddy.end()) {
        fTransAddBuddy.erase(fTransAddBuddy.find(transId));
        for (size_t i = 0; i < count; i++) {
            fMaster->push(this->sendVaultNodeAdd(fNodeIDs[kBuddyList],
                        nodes[i], fPlayerID));
        }
    }
}

void kiAuthClient::onVaultNodeAdded(hsUint32 parent, hsUint32 child,
        hsUint32 owner) {

    fVaultMutex.lock();
    fRefs[parent].push_back(child);
    fVaultMutex.unlock();

    if (parent == fNodeIDs[kBuddyList]) {
        fMaster->push(this->sendVaultNodeFetch(child));
    }
}
