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

#include "kiUtils.h"

plString GetFancyAgeName(plString agename) {
    if ((agename == "Personal") || (agename == "philRelto")) {
        return "Relto";
    } else if (agename.startsWith("GuildPub-")) {
        return plString::Format("Guild of %s Pub",
               agename.afterLast('-'));
    } else if (agename == "Cleft") {
        return "D'ni-Riltagamin";
    } else if ((agename == "Neighborhood02") || (agename == "Kirel")) {
        return "D'ni-Kirel";
    } else if (agename == "ErcanaCitySilo") {
        return "D'ni-Ashem'en";
    } else if ((agename == "city") || (agename == "BaronCityOffice") ||
            (agename == "spyroom") || (agename == "Ae'gura")) {
        return "D'ni-Ae'gura";
    } else if ((agename == "Garrison") || (agename == "Gahreesen")) {
        return "Gahreesen";
    } else if (agename == "Garden") {
        return "Eder Kemo";
    } else if (agename == "Gira") {
        return "Eder Gira";
    } else if (agename == "EderDelin") {
        return "Eder Delin";
    } else if (agename == "EderTsogal") {
        return "Eder Tsogal";
    } else if (agename == "AvatarCustomization") {
        return "Relto";
    } else if ((agename == "GreatZero") || (agename == "Great Zero")) {
        return "D'ni-Rezeero";
    } else if ((agename == "Descent") || (agename == "Shaft")) {
        return "D'ni-Tiwah";
    } else if (agename == "Kveer") {
        return "D'ni-K'veer";
    } else if (agename == "Neighborhood") {
        return "D'ni-Neighbourhood";
    } else if (agename == "BahroCave") {
        return "D'ni-Rudenna";
    } else if (agename == "AhnonayCathedral") {
        return "Ahnonay Cathedral";
    } else {
        return agename;
    }
}
