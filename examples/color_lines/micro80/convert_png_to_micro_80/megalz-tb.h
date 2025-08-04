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

#ifndef MHMT_TB_H
#define MHMT_TB_H

#include "megalz-types.h"

extern struct tb_chain *tb_entry[];

// chained two-byter element: pos is position in input file
struct tb_chain {
    OFFSET pos;
    struct tb_chain *next;
};

#define BUNCHSIZE 4096

struct tb_bunch {
    struct tb_bunch *next;
    struct tb_chain bunch[BUNCHSIZE];
};

void init_tb(void);
void free_tb(void);
ULONG add_tb(UWORD index, OFFSET position);
void cutoff_tb_chain(UWORD index, OFFSET position);
ULONG add_bunch_of_tb(void);
struct tb_chain *get_free_tb(void);

#endif
