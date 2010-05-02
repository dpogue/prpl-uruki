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

static gboolean file_timeout(gpointer data) {
    kiClient* cli = (kiClient*)data;
    cli->set_error(kiClient::kFile, kNetErrTimeout);

    return FALSE;
}

static gboolean file_build_callback(gpointer data) {
    kiClient* client = (kiClient*)data;
    client->file_build_callback();
    return FALSE;
}

kiFileClient::kiFileClient(kiClient* master) {
    setClientInfo(KI_BUILDTYPE, KI_BRANCHID, KI_UUID);

    fMaster = master;
    fTimeout = 0;
}

kiFileClient::~kiFileClient() {
    fMaster = NULL;
    if (fTimeout != 0) {
        g_source_remove(fTimeout);
        fTimeout = 0;
    }
}

void kiFileClient::process() {
    if (!this->isConnected()) return;
    this->ping();

    fMaster->push(this->sendBuildIdRequest());
    if (!this->isConnected()) return;
    fCondBuildId.wait();
    g_idle_add(file_build_callback, fMaster);
}

void kiFileClient::ping() {
    if (fTimeout == 0) {
        this->sendPingRequest(0);
        fTimeout = g_timeout_add_seconds(KI_TIMEOUT, file_timeout, fMaster);
    }
}

void kiFileClient::request(const plString age, const plString page) {
    fPagename = page;

    fMaster->push(this->sendManifestRequest(age, 0));
}

void kiFileClient::onPingReply(hsUint32 pingTimeMs) {
    if (fTimeout != 0) {
        g_source_remove(fTimeout);
        fTimeout = 0;
    }
}

void kiFileClient::onBuildIdReply(hsUint32 transId, ENetError result,
        hsUint32 buildId) {
    fMaster->setBuildID(buildId);
    fMaster->pop(transId);

    fCondBuildId.signal();
}
