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

/* GateKeeper server public keys */
hsUbyte KEY_Gate_N[] = { 0x8D, 0xFA, 0x35, 0xE6, 0xF8, 0x7A, 0x50, 0x50,
                         0xAB, 0x25, 0x4B, 0x81, 0xD1, 0xD7, 0x7A, 0x23,
                         0xA3, 0x9A, 0x21, 0x0F, 0x34, 0xAB, 0x66, 0x2E,
                         0x16, 0x98, 0x55, 0xB6, 0xFC, 0x49, 0xD5, 0x50,
                         0xDC, 0xB8, 0x4C, 0x4D, 0xC7, 0xDB, 0xF1, 0x1C,
                         0x15, 0x4C, 0x55, 0xF5, 0x92, 0x0D, 0x6A, 0xEC,
                         0x60, 0xBC, 0x55, 0xFA, 0x29, 0x2F, 0x6F, 0xC3,
                         0xD7, 0x21, 0x80, 0xA3, 0x6B, 0x44, 0x23, 0xB5 };
hsUbyte KEY_Gate_X[] = { 0xB3, 0x88, 0xFF, 0x0B, 0x90, 0x70, 0x2B, 0x2E,
                         0x07, 0xBC, 0x62, 0x98, 0x83, 0x9D, 0x0F, 0x05,
                         0x39, 0xFA, 0x35, 0x39, 0xA9, 0xF3, 0xB3, 0xFC,
                         0xCD, 0x5E, 0xA9, 0xA6, 0x61, 0x0F, 0x9B, 0x38,
                         0x0F, 0x9C, 0xBE, 0xA0, 0xBE, 0x6F, 0x7F, 0xE4,
                         0x7C, 0xCB, 0xC4, 0x09, 0x6C, 0x8D, 0xCE, 0x47,
                         0x68, 0x82, 0x32, 0xC5, 0x89, 0x94, 0xF9, 0xCA,
                         0x69, 0x69, 0xD0, 0x60, 0x19, 0xB7, 0xF3, 0x1A };

static gboolean gate_file_callback(gpointer data) {
    kiClient* client = (kiClient*)data;
    return client->gate_file_callback();
}

kiGateClient::kiGateClient(kiClient* master) {
    setKeys(KEY_Gate_X, KEY_Gate_N);
    setClientInfo(master->getBuildID(), KI_BUILDTYPE, KI_BRANCHID, KI_UUID);

    fMaster = master;
}

kiGateClient::~kiGateClient() {
    fMaster = NULL;
}

void kiGateClient::process() {
    if (!this->isConnected()) {
        return;
    }

    fMaster->push(this->sendFileSrvIpAddressRequest(1));
    if (!this->isConnected()) {
        return;
    }
    fCondFile.wait();
    g_idle_add(gate_file_callback, fMaster);

    fMaster->push(this->sendAuthSrvIpAddressRequest());
    if (!this->isConnected()) {
        return;
    }
    fCondAuth.wait();
}

void kiGateClient::onFileSrvIpAddressReply(hsUint32 transId,
        const plString& addr) {
    fMaster->setAddress(kiClient::kFile, addr);
    fMaster->pop(transId);

    fCondFile.signal();
}

/* This shouldn't happen, since requesting the Auth IP disconnects us */
void kiGateClient::onAuthSrvIpAddressReply(hsUint32 transId,
        const plString& addr) {
    fMaster->setAddress(kiClient::kAuth, addr);
    fMaster->pop(transId);
    
    fCondAuth.signal();
}
