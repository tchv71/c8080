/*
 * MegaLZ packer
 * (c) https://github.com/lvd2
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

#include <stdio.h>
#include <stdlib.h>

#include "megalz-types.h"
#include "megalz-hash.h"

// allocate mem for hash (length) and build it from data
// length must be at least 3 bytes, since hash[0] and hash[1] are not valid hashes
UBYTE *build_hash(const UBYTE *data, ULONG length, ULONG prebin_len) {
    UBYTE *hash;

    ULONG i;
    const UBYTE *src;
    UBYTE *dst;
    UBYTE curr, prev, prev2;

    if (!length)
        return NULL;

    hash = (UBYTE *)malloc(length + prebin_len);
    if (!hash)
        return NULL;

    prev = curr = 0;
    i = length + prebin_len;
    src = data - prebin_len;
    dst = hash;

    do {
        prev2 = (UBYTE)((prev >> 1) | (prev << 7));
        prev = (UBYTE)((curr >> 1) | (curr << 7));
        curr = *(src++);

        *(dst++) = curr ^ prev ^ prev2;

    } while (--i);

    return hash + prebin_len;  // negative indices must be used to access prebin hashes
}

// free hash
void destroy_hash(UBYTE *hash, ULONG prebin_len) {
    if (hash)
        free(hash - prebin_len);
}
