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

#ifndef MHMT_PACK_H
#define MHMT_PACK_H

#include "megalz-types.h"
#include "megalz-lz.h"

//#define MAX_CODES_SIZE 3860 // max num of codes is 3857, plus stopcode, plus some extra bytes
#define MAX_CODES_SIZE 65540

// this structure exists in array for each input byte and used to build optimal code chain
struct packinfo {
    struct lzcode code;     // code of jumping here from previous position
    ULONG price;            // bitprice (bitlength) of the best chain of lzcodes going up to this point:
                            // initialized as 0xFFFFFFFF (maximum price)
    ULONG _just_a_filler_;  // to make structure 8 bytes long (true for 32bit machines)
};

ULONG pack(void);

#endif
