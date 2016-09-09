/* This file is iconvars.h (part of XIcon)
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

#ifndef ICONVARS_H
#define ICONVARS_H

#include "icondata.h"

extern int generic_count;
extern generic_info *generic;

extern int dump_input;

extern rgb_info *pc_palette;
extern rgb_info *mac_palette;
extern rgb_info *bw_palette;

extern char *color_db_file;

#endif /* not ICONVARS_H */
