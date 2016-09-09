/* This file is macread.h (part of XIcon)
 *
 * Copyright (C) 1993 by Norman Walsh
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 ************************************************************************/

#ifndef MACREAD_H
#define MACREAD_H

#include "macdata.h"

void read_mac_file();
RsrcHdrStruct read_mac_header();
RsrcMapStruct read_mac_map();
RsrcTypeStruct read_mac_type();
RsrcRefStruct read_mac_ref();
enum MacResourceTypes MacResType(RsrcTypeStruct);

#endif /* not MACREAD_H */
