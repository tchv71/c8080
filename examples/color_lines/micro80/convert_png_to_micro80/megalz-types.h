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

#ifndef MHMT_TYPES_H

#define MHMT_TYPES_H

#ifdef _AMIGA
#include <exec/types.h>
typedef signed int OFFSET;
#else
#include <sys/types.h>
#define MHMT_OWNTYPES
#endif

#ifdef MHMT_OWNTYPES
typedef signed char BYTE;
typedef unsigned char UBYTE;

typedef signed short int WORD;
typedef unsigned short int UWORD;

typedef signed int LONG;
typedef unsigned int ULONG;

typedef off_t OFFSET;  // this will be 32bit signed in 32bit systems and 64bit signed in 64bit linux, don't care of
                       // other 64bits yet...
#endif

#endif
