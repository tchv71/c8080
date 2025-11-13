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

#include "dir.h"
#include <string.h>
#include <c8080/getbitposition.h>

void DirInfoReset(struct DirInfo *self, uint8_t drive) {
    self->drive = drive;
    memset(self->items, 0, sizeof(self->items));
}

uint8_t DirInfoAdd(struct DirInfo *self, struct FCB *fcb) {
    const uint8_t dir_index = CpmGetAttrib(fcb->name83 - 3) & 0x0F;
    if (dir_index == 0)
        return 0xFF;  // Это не папка
    struct DirInfoItem *di = &self->items[dir_index - 1];
    di->parent = fcb->drive & 0x0F;
    CpmConvertFromName83(di->name, fcb->name83);
    return dir_index;
}

void DirInfoMakePath(struct DirInfo *self, char *out, uint8_t out_size, uint8_t dir_index) {
    *out = 'A' + self->drive;
    out++;
    *out = ':';
    out++;
    *out = 0;

    if (dir_index == 0) {
        *out = '\\';
        out++;
        *out = 0;
        return;
    }

    char *result = out + (out_size - 3);
    *result = 0;

    do {
        struct DirInfoItem *di = &self->items[dir_index - 1];
        dir_index = di->parent;
        if (di->name[0] == 0)
            return;  // Unknown folder
        const size_t name_size = strlen(di->name);
        result -= name_size;
        if (result <= out)
            return;  // Overflow
        memcpy(result, di->name, name_size);
        result--;
        *result = '\\';
    } while (dir_index != 0);

    strcpy(out, result);
}

uint8_t DirAllocate(void) {
    uint16_t bitmap = 1;
    struct FCB f;
    f.drive = '?';
    struct FCB *x = CpmSearchFirst(DEFAULT_DMA, &f);
    while (x != NULL) {
        if (x->drive < 0x10) {
            const uint8_t dir_index = CpmGetAttrib(x->name83 - 3) & 0x0F;
            if (dir_index != 0)
                bitmap |= 1 << dir_index;
        }
        x = CpmSearchNext();
    }
    return GetZeroBitPosition16(bitmap);
}
