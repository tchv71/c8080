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

#ifndef MHMT_HASH_H
#define MHMT_HASH_H

#include "megalz-types.h"

UBYTE *build_hash(const UBYTE *data, ULONG length, ULONG prebin_len);
void destroy_hash(UBYTE *hash, ULONG prebin_len);

#endif
