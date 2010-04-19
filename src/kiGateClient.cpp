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

#include "kiGateClient.h"

kiGateClient::kiGateClient(kiClient* master) {
    setKeys(KEY_Gate_X, KEY_Gate_N);
    setClientInfo(master->getBuildID(), KI_BUILDTYPE, KI_BRANCHID, KI_UUID);

    fMaster = master;
}

kiGateClient::~kiGateClient() {
    fMaster = NULL;
}

void kiGateClient::process() {
    if (this->isConnected()) {
        fMaster->push(this->sendFileSrvIpAddressRequest(1));
        fCondFile.wait();
    }

    g_idle_add(fMaster->gate_callback, NULL);
}

void kiGateClient::onFileSrvIpAddressReply(hsUint32 transId,
        const plString& addr) {
    fMaster->setAddress(kFile, addr);
    fMaster->pop(transId);

    fCondFile.signal();
}

/* This shouldn't happen, since requesting the Auth IP disconnects us */
void kiGateClient::onAuthSrvIpAddressReply(hsUint32 transId,
        const plString& addr) {
    fMaster->setAddress(kAuth, addr);
    fMaster->pop(transId);
    
    fCondAuth.signal();
}
