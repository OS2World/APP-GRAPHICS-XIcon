/* This file is windata.h (part of XIcon)
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

#ifndef WINDATA_H
#define WINDATA_H

#include "icondata.h"

typedef struct _win_rgb_info {
  BYTE   blue;
  BYTE   green;
  BYTE   red;
  BYTE   unused;
} win_rgb_info;

typedef struct _WinIconDirEntry {
  BYTE   bWidth;
  BYTE   bHeight;
  BYTE   bColorCount;
  BYTE   bReserved;
  USHORT wPlanes;
  USHORT wBitCount;
  ULONG  dwBytesInRes;
  ULONG  dwImageOffset;
} WinIconDirEntry;

typedef struct _WinIconHeader {
  USHORT idReserved;
  USHORT idType;
  USHORT idCount;
} WinIconHeader;

typedef struct _WinIconBitInfoHdr {
  ULONG  biSize;
  ULONG  biWidth;
  ULONG  biHeight;
  USHORT biPlanes;
  USHORT biBitCount;
  ULONG  biCompression;
  ULONG  biSizeImage;
  ULONG  biXPelsPerMeter;
  ULONG  biYPelsPerMeter;
  ULONG  biClrUsed;
  ULONG  biClrImportant;
  win_rgb_info *p;
} WinIconBitInfoHdr;

#endif /* not WINDATA_H */

