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

#ifndef _KI_FILECLIENT_H
#define _KI_FILECLIENT_H

#include "kiHeaders.h"
#include "kiClient.h"
#include "file/pnFileClient.h"

class kiFileClient : public pnFileClient {
public:
    kiFileClient(kiClient* master);
    virtual ~kiFileClient();

    void process();

    //virtual void onPingReply(hsUint32 transId, hsUint32 pingTimeMs);
    virtual void onBuildIdReply(hsUint32 transId, ENetError result,
            hsUint32 buildId);

private:
    kiClient* fMaster;
    hsThreadCondition fCondBuildId;
};

#endif
