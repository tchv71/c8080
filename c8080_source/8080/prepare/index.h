/*
 * c8080 compiler
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

#pragma once

#include "../../prepare/prepare.h"

bool Prepare8080Const(Prepare &, CNodePtr &node);
bool Prepare8080SetOperators(Prepare &, CNodePtr &node);
bool Prepare8080Fast8BitCompare(Prepare &, CNodePtr &node);
bool Prepare8080Fast8BitMath(Prepare &, CNodePtr &node);
bool Prepare8080UnsignedCompareG0ToE(Prepare &, CNodePtr &node);
bool Prepare8080CompareGToGe(Prepare &, CNodePtr &node);
bool Prepare8080Sub16ToAdd16(Prepare &, CNodePtr &node);
bool Prepare8080IncDec(Prepare &, CNodePtr &node);
bool Prepare8080Fast8Math(Prepare &, CNodePtr &node);
bool Prepare8080SecondConstArg(Prepare &, CNodePtr &node);
bool Prepare8080DoubleConvert(Prepare &, CNodePtr &node);
bool Prepare8080ConstCond(Prepare &, CNodePtr &node);
