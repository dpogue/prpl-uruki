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

kiAuthClient::kiAuthClient(kiClient* master) {
    setKeys(KEY_Auth_X, KEY_Auth_N);
    setClientInfo(master->getBuildID(), KI_BUILDTYPE, KI_BRANCHID, KI_UUID);

    fMaster = master;
    fClientChallenge = (hsUint32)rand();
}

kiAuthClient::~kiAuthClient() {
    fMaster = NULL;
}

void kiAuthClient::setCredentials(plString username, plString password) {
    fUsername = username;
    fPassword = password;
}

void kiAuthClient::process() {
    if (!this->isConnected()) {
        return;
    }

    this->sendClientRegisterRequest();
    if (!this->isConnected()) {
        return;
    }
    fCondChallenge.wait();

    fMaster->push(this->sendAcctLoginRequest(
                fServerChallenge, fClientChallenge, fUsername, fPassword));
    if (!this->isConnected()) {
        return;
    }
}

void kiAuthClient::onServerAddr(hsUint32 address, const plUuid& token) {
    plDebug::Debug("Auth Server address %x; %s", address,
            token.toString().cstr());
}

void kiAuthClient::onClientRegisterReply(hsUint32 serverChallenge) {
    fServerChallenge = serverChallenge;
    
    fCondChallenge.signal();
}
