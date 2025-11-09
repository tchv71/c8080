/*
 * NC shell for CP/M
 * Copyright (c) 2025 Aleksey Morozov aleksey.f.morozov@gmail.com aleksey.f.morozov@yandex.ru
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "nc.h"
#include "tools.h"
#include "colors.h"
#include "windows.h"
#include "files.h"
#include "panel.h"

static void NcDriveChanged(uint8_t drive_user) {
    if (panel_a.drive_user == drive_user)
        NcFindFiles();
    if (panel_b.drive_user == drive_user) {
        PanelSwap();
        NcFindFiles();
        PanelSwap();
    }
    NcDrawScreen();
}

