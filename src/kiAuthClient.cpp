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

kiAuthClient::kiAuthClient(kiClient* master) {
    setKeys(KEY_Auth_X, KEY_Auth_N);
    setClientInfo(master->getBuildID(), KI_BUILDTYPE, KI_BRANCHID, KI_UUID);

    fMaster = master;
}

kiAuthClient::~kiAuthClient() {
    fMaster = NULL;
}

void kiAuthClient::process() {
    if (!this->isConnected()) {
        return;
    }

    /*
    fMaster->push(this->sendFileSrvIpAddressRequest(1));
    if (!this->isConnected()) {
        return;
    }
    fCondFile.wait();
    g_idle_add(fMaster->auth_file_callback, NULL);

    fMaster->push(this->sendAuthSrvIpAddressRequest(1));
    if (!this->isConnected()) {
        return;
    }
    fCondAuth.wait();
    */
}

