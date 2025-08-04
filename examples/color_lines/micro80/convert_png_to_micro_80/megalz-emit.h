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

#ifndef MHMT_EMIT_H
#define MHMT_EMIT_H

#include "megalz-types.h"
#include "megalz-lz.h"
#include "megalz-optimal.h"

#define EMIT_FILEBUF_SIZE 4096 /* file buffer */

#define EMIT_BYTEBUF_SIZE 256 /* !! MUST BE 2^N !! */

ULONG emit_megalz(struct optchain *optch, ULONG actual_len);
ULONG emit_hrum(struct optchain *optch, ULONG actual_len);
ULONG emit_hrust(struct optchain *optch, ULONG actual_len);
ULONG emit_zx7(struct optchain *optch, ULONG actual_len);

#define EMIT_FILE_INIT (-1)
#define EMIT_FILE_FINISH (0)
// what should emit_file do: init channel, add bytes or finish (flush) channel to disk
//
ULONG emit_file(const UBYTE *bytes, LONG length);

#define EMIT_BITS_INIT (-1)
#define EMIT_BITS_FINISH (0)
// length>0 or one of two special cases
ULONG emit_bits(ULONG msb_aligned_bits, LONG length);

ULONG emit_bits_flush(ULONG bits);

#define EMIT_BYTE_INIT 0
#define EMIT_BYTE_FLUSH 1
#define EMIT_BYTE_ADD 2
//
ULONG emit_byte(UBYTE byte, ULONG operation);

// ULONG emit_begin(void);
// ULONG emit_end(void);

#endif
