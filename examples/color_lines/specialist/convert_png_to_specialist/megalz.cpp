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

#include "megalz.h"
#include "megalz-globals.h"
#include "megalz-pack.h"
#include <stdexcept>

void PackMegalz(std::vector<uint8_t> &data) {
    init_globals();
    wrk.prelen = 0;
    wrk.inlen = data.size();
    wrk.indata = (const UBYTE *)data.data();
    wrk.indata_raw = (const UBYTE *)data.data();
    if (!pack())
        throw std::runtime_error("Can't pack");
    data.assign(wrk.output, wrk.output + wrk.output_pos);
}
