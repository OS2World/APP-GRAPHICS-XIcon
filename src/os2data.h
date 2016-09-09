/* This file is os2data.h (part of XIcon)
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

#ifndef OS2DATA_H
#define OS2DATA_H

#include "icondata.h"

typedef struct _os2_icon_hdr { /* actually, an array of bitmaps... */
  USHORT flag;
  ULONG  cbsize;
  ULONG  next;
  USHORT display_width;
  USHORT display_height;
} os2_icon_hdr;

typedef os2_icon_hdr os2_arr_hdr;

typedef struct _os2_file_hdr { /* a file header (not an array) */
  USHORT    flag;
  ULONG     cbsize;
  SHORT     xhotspot;
  SHORT     yhotspot;
  ULONG     bitmap_offset;
} os2_file_hdr;

/* *********************************************** OS/2 1.2 */

typedef struct _os2_12_rgb_info {
  BYTE   blue;
  BYTE   green;
  BYTE   red;
} os2_12_rgb_info;

typedef struct _os2_12_bitmap_info {
  USHORT flag;
  ULONG  cbsize;
  USHORT hotx, hoty;
  ULONG  bitmap_offset;
  ULONG  bitmap_size;
  USHORT width, height, planes, bits_per_pixel;
  os2_12_rgb_info *p;
} os2_12_bitmap_info;

typedef struct _os2_12_icon_info {
  os2_icon_hdr        header;
  os2_12_bitmap_info  andxor_map;
  os2_12_bitmap_info  clr_map;
} os2_12_icon_info;

/* *********************************************** OS/2 2.0 */

typedef struct _os2_20_rgb_info {
  BYTE   blue;
  BYTE   green;
  BYTE   red;
  BYTE   unused;
} os2_20_rgb_info;

typedef struct _os2_20_bitmap_info {
  USHORT flag;
  ULONG  cbsize;
  USHORT hotx;
  USHORT hoty;
  ULONG  bitmap_offset;
  ULONG  length;
  ULONG  width;
  ULONG  height;
  USHORT planes;
  USHORT bits_per_pixel;
  ULONG  compression;
  ULONG  bitmap_size;
  ULONG  devXres;
  ULONG  devYres;
  ULONG  clrUsed;
  ULONG  clrImportant;
  USHORT units;
  USHORT reserved;
  USHORT recording;
  USHORT rendering;
  ULONG  size1;
  ULONG  size2;
  ULONG  color_encoding;
  ULONG  app_identifier;
  os2_20_rgb_info  *p;
} os2_20_bitmap_info;

typedef struct _os2_20_icon_info {
  os2_icon_hdr        header;
  os2_20_bitmap_info  andxor_map;
  os2_20_bitmap_info  clr_map;
} os2_20_icon_info;

#endif /* not OS2DATA_H */

