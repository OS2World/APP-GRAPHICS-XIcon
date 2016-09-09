/* This file is macdata.h (part of XIcon)
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

#ifndef MACDATA_H
#define MACDATA_H

#include "icondata.h"

enum MacResourceTypes { Mac_xxxx, Mac_ICNp, Mac_icsp, Mac_icl4, Mac_ics4 };

typedef struct _RsrcHdrStruct {
  ULONG  DataOffset;
  ULONG  MapOffset;
  ULONG  DataLen;
  ULONG  MapLen;
  BYTE   OSReserved[96];
  BYTE   AppReserved[128];
} RsrcHdrStruct;

typedef struct _RsrcMapStruct {
  ULONG   MapCopy[4];
  ULONG   NextMap;
  USHORT  FileRef;
  USHORT  FileAttr;
  USHORT  TypeOffset;
  USHORT  NameOffset;
} RsrcMapStruct;

typedef struct _RsrcTypeStruct {
  char    Name[4];
  USHORT  Count;
  USHORT  RefOffset;
} RsrcTypeStruct;

typedef struct _RsrcRefStruct {
  USHORT Ident;
  USHORT NameOffset;
  BYTE   Attr;
  ULONG  DataOffset;
} RsrcRefStruct;

typedef struct _MacDataStruct {
  ULONG size;
  BYTE  *map;
} MacDataStruct;

#endif /* not MACDATA_H */

