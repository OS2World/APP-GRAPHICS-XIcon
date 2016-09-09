/* This file is generic.c (part of XIcon)
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

#include <stdio.h>
#include "iconvars.h"
#include "icondata.h"

bitmap_info init_empty_map (int width, int height, int bits)
{
  bitmap_info m;
  BYTE *ch;
  int count;

  m.width = width;
  m.height = height;
  m.bits_per_pixel = bits;
  m.bytewidth = ((width*bits)+7) / 8;
  m.size = m.bytewidth * m.height;
  m.num_colors = 2 << (bits - 1);
  m.palette = NULL;
  m.map = (BYTE *) malloc (m.size);

  for (count = 0, ch = m.map; count < m.size; count++, ch++)
    *ch = 0;
  
  return m;
}

bitmap_info init_generic_map()
{
  bitmap_info m;

  m.width = 0;
  m.height = 0;
  m.bits_per_pixel = 0;
  m.size = 0;
  m.bytewidth = 0;
  m.num_colors = 0;
  m.palette = NULL;
  m.map = NULL;

  return m;
}

void init_generic (int count)
{
  generic[count].and_map = init_generic_map();
  generic[count].xor_map = init_generic_map();
  generic[count].clr_map = init_generic_map();
}

