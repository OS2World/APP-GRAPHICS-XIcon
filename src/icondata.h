/* This file is icondata.h (part of XIcon)
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

#ifndef ICONDATA_H
#define ICONDATA_H

#ifndef SKIPTYPES
typedef unsigned short int USHORT;
typedef unsigned char      BYTE;
typedef unsigned long int  ULONG;
typedef short int          SHORT;
typedef int                LONG;
#endif /* not SKIPTYPES */

enum iconFileType { UnkFile, MacIcon, OS2Icon12, OS2Icon20, WinIcon, XBMicon };

typedef struct _rgb_info {
  BYTE blue;
  BYTE green;
  BYTE red;
} rgb_info;

typedef struct _bitmap_info {
  ULONG width, height;
  ULONG bits_per_pixel;
  ULONG size;
  ULONG bytewidth;
  ULONG num_colors;
  rgb_info *palette;
  BYTE  *map;
} bitmap_info;

typedef struct _generic_info {
  bitmap_info and_map;
  bitmap_info xor_map;
  bitmap_info clr_map;
} generic_info;

#endif /* not ICONDATA_H */
