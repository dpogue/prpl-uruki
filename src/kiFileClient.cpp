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

#include "kiFileClient.h"

static gboolean file_build_callback(gpointer data) {
    kiClient* client = (kiClient*)data;
    return client->file_build_callback();
}

kiFileClient::kiFileClient(kiClient* master) {
    setClientInfo(KI_BUILDTYPE, KI_BRANCHID, KI_UUID);

    fMaster = master;
}

kiFileClient::~kiFileClient() {
    fMaster = NULL;
}

void kiFileClient::process() {
    if (!this->isConnected()) {
        return;
    }

    fMaster->push(this->sendBuildIdRequest());
    if (!this->isConnected()) {
        return;
    }
    fCondBuildId.wait();
    g_idle_add(file_build_callback, fMaster);
}

void kiFileClient::onBuildIdReply(hsUint32 transId, ENetError result,
        hsUint32 buildId) {
    fMaster->setBuildID(buildId);
    fMaster->pop(transId);

    fCondBuildId.signal();
}
