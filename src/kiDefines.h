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

#ifndef _KI_DEFINES_H
#define _KI_DEFINES_H

/* libpurple uses glib */
#include <glib.h>

#define GETTEXT_PACKAGE "prpl-uruki"
#ifdef ENABLE_NLS
#	ifdef _WIN32
#		include <win32dep.h>
#	endif
#	include <glib/gi18n-lib.h>
#else
#	define _(a) a
#endif

/* libHSPlasma headers */
#include "Debug/plDebug.h"
#include "pnClient.h"

/* libpurple headers */
#include <account.h>
#include <accountopt.h>
#include <blist.h>
#include <connection.h>
#include <plugin.h>
#include <proxy.h>
#include <prpl.h>
#include <status.h>
#include <util.h>
#include <version.h>

/* Other defines */
#define KI_VERSION "0.01"
#define KI_UUID plUuid("ea489821-6c35-4bd0-9dae-bb17c585e680")
#define KI_BRANCHID 1
#define KI_BUILDTYPE 50
#define KI_PORT 14617

#endif
