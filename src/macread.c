/* This file is macread.c (part of XIcon)
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
#include "macdata.h"
#include "iconio.h"
#include "iconvars.h"
#include "generic.h"
#include "palette.h"
#include "bitmap.h"

int cvtICNp;
int cvticsp;
int cvticl4;
int cvtics4;
int MacPalette;

int Mac_typeCount;
RsrcTypeStruct *Mac_types;
RsrcRefStruct  *Mac_refs;
MacDataStruct  *Mac_data;

RsrcHdrStruct read_mac_header()
{
  RsrcHdrStruct Header;
  int count;

  Header.DataOffset = read_macint();
  Header.MapOffset  = read_macint();
  Header.DataLen    = read_macint();
  Header.MapLen     = read_macint();

  for (count = 0; count < 96; count++)
    Header.OSReserved[count] = read_macchar();

  for (count = 0; count < 128; count++)
    Header.AppReserved[count] = read_macchar();

  return Header;
}

RsrcMapStruct read_mac_map()
{
  RsrcMapStruct Map;
  int count;

  for (count = 0; count < 4; count++)
    Map.MapCopy[count] = read_macint();

  Map.NextMap = read_macint();
  Map.FileRef = read_macshortint();
  Map.FileAttr = read_macshortint();
  Map.TypeOffset = read_macshortint();
  Map.NameOffset = read_macshortint();

  return Map;
}

RsrcTypeStruct read_mac_type()
{
  RsrcTypeStruct Type;
  int count;

  for (count = 0; count < 4; count++)
    Type.Name[count] = read_macchar();

  Type.Count = read_macshortint();
  Type.RefOffset = read_macshortint();

  return Type;
}

RsrcRefStruct read_mac_ref()
{
  RsrcRefStruct Ref;
  int count;

  Ref.Ident = read_macshortint();
  Ref.NameOffset = read_macshortint();
  Ref.Attr = read_macchar();
  Ref.DataOffset = read_macint3();

  return Ref;
}

enum MacResourceTypes MacResType(RsrcTypeStruct t)
{
  if (strncmp (t.Name, "ICN#", 4) == 0)
    return Mac_ICNp;

  if (strncmp (t.Name, "ics#", 4) == 0)
    return Mac_icsp;

  if (strncmp (t.Name, "icl4", 4) == 0)
    return Mac_icl4;

  if (strncmp (t.Name, "ics4", 4) == 0)
    return Mac_ics4;

  return Mac_xxxx;
}

MacDataStruct *find_mac_data(int nth, enum MacResourceTypes type)
{
  int count, place;

  if (nth == 0)
    return NULL;

  for (count = 0; count < Mac_typeCount; count++)
    {
      if (MacResType(Mac_types[count]) == type)
	nth--;
      
      if (nth == 0)
	return &Mac_data[count];
    }

  return NULL;
}

bitmap_info mac_init_generic(int width, 
			     int height, 
			     int bits, 
			     BYTE *dataptr)
{
  bitmap_info m;

  m.width = width;
  m.height = height;
  m.bits_per_pixel = bits;
  m.num_colors = 2 << (bits - 1);

  m.bytewidth = ((m.width*m.bits_per_pixel) + 7) / 8;
  m.size = (m.bytewidth * m.height);

  if (bits == 1)
    m.palette = bw_palette;
  else
    if (MacPalette)
      m.palette = mac_palette;
    else
      m.palette = pc_palette;

  m.map = dataptr;

  return m;
}

bitmap_info mac_empty_map(int width, int height, int bits)
{
  bitmap_info m;
  BYTE *ch;
  int count;

  m.width = width;
  m.height = height;
  m.bits_per_pixel = bits;
  m.num_colors = 2 << (bits - 1);

  m.bytewidth = ((m.width*m.bits_per_pixel) + 7) / 8;
  m.size = (m.bytewidth * m.height);

  if (bits == 1)
    m.palette = bw_palette;
  else
    if (MacPalette)
      m.palette = mac_palette;
    else
      m.palette = pc_palette;
  
  m.map = (BYTE *) malloc(m.size);
  
  for (count = 0, ch = m.map;
       count < m.size;
       count++, ch++)
    *ch = 0;

  return m;
}

void dump_mac_header(RsrcHdrStruct h, RsrcMapStruct m)
{
  printf("Mac header:\t\tMac map:\n");
  printf("DataOffset (4): 0x%04x\tNextMap    (4) : 0x%04x\n", 
	 h.DataOffset, m.NextMap);
  printf("MapOffset  (4): 0x%04x\tFileRef    (2) :   %4d\n", 
	 h.MapOffset, m.FileRef);
  printf("DataLen    (4):   %4d\tFileAttr   (2) :   %4d\n", 
	 h.DataLen, m.FileAttr);
  printf("MapLen     (4):   %4d\tTypeOffset (2) :   %4d\n",
	 h.MapLen, m.TypeOffset);
  printf("\t\t\tNameOffset (2) :   %4d\n", 
	 m.NameOffset);
  printf("\n");
}

void dump_mac_info(RsrcTypeStruct t, RsrcRefStruct r)
{
  printf("Mac type:\t\tMac ref:\n");
  printf("Name      (4) :   %c%c%c%c\tIdent      (2) : 0x%04x\n",
	 t.Name[0], t.Name[1], t.Name[2], t.Name[3], r.Ident);
  printf("Count     (2) :   %4d\tNameOffset (2) : 0x%04x\n",
	 t.Count, r.NameOffset);
  printf("RefOffset (2) : 0x%04x\tAttr       (1) : %d\n",
	 t.RefOffset, r.Attr);
  printf("\t\t\tDataOffset (2) : 0x%04x\n", 
	 r.DataOffset);
  printf("\n");
}

void dump_maps(generic_info g)
{
  printf("Dump of 'and' map:\n");
  printmap(g.and_map);
  printf("\nDump of 'xor' map:\n");
  printmap(g.xor_map);
  printf("\nDump of 'clr' map:\n");
  printmap(g.clr_map);
  printf("\n");
}

void fix_transparency (bitmap_info and_map, bitmap_info clr_map)
{
  int row, col;

  for (row = 1; row <= and_map.height; row++)
    for (col = 1; col <= and_map.width; col++)
      if (get_bit(and_map, row, col))
	set_bit (clr_map, row, col, 0xFF);
}

void fix_small_map (bitmap_info and_map, bitmap_info clr_map)
{
  int row, col;
  BYTE ab, cb;

  for (row = 1; row <= and_map.height; row++)
    for (col = 1; col <= and_map.width; col++)
      {
	ab = get_bit(and_map, row, col);
	cb = get_bit(clr_map, row, col);
	set_bit(and_map, row, col, ab | cb);
	set_bit(clr_map, row, col, 0);
      }
}

void read_mac_file()
{
  RsrcHdrStruct Mac_header;
  RsrcMapStruct Mac_map;
  int count, place;
  int ICNpcnt, icspcnt, icl4cnt, ics4cnt;
  int ICNpcur, icspcur, icl4cur, ics4cur;
  MacDataStruct *dataptr, *maskptr;
  BYTE XlateArr[16], ch;
  int row, col;

  XlateArr[0] = 0;
  XlateArr[1] = 14;
  XlateArr[2] = 12;
  XlateArr[3] = 12;
  XlateArr[4] = 13;
  XlateArr[5] = 1;
  XlateArr[6] = 9;
  XlateArr[7] = 3;
  XlateArr[8] = 10;
  XlateArr[9] = 2;
  XlateArr[10] = 4;
  XlateArr[11] = 6;
  XlateArr[12] = 8;
  XlateArr[13] = 8;
  XlateArr[14] = 7;
  XlateArr[15] = 15;

  fseek(input, 0, SEEK_SET);

  Mac_header = read_mac_header();

  fseek(input, Mac_header.MapOffset, SEEK_SET);

  Mac_map = read_mac_map();

  if (dump_input)
    dump_mac_header(Mac_header, Mac_map);

  fseek(input, Mac_header.MapOffset + Mac_map.TypeOffset, SEEK_SET);

  Mac_typeCount = read_macshortint()+1;
  Mac_types = (RsrcTypeStruct *) malloc (Mac_typeCount*sizeof(RsrcTypeStruct));

  for (count = 0; count < Mac_typeCount; count++)
    Mac_types[count] = read_mac_type();

  Mac_refs = (RsrcRefStruct *) malloc (Mac_typeCount*sizeof(RsrcRefStruct));

  for (count = 0; count < Mac_typeCount; count++)
    {
      fseek(input,
            Mac_header.MapOffset
            + Mac_map.TypeOffset
            + Mac_types[count].RefOffset,
            SEEK_SET);
      Mac_refs[count] = read_mac_ref();
    }

  if (dump_input)
    for (count = 0; count < Mac_typeCount; count++)
      dump_mac_info(Mac_types[count], Mac_refs[count]);

  ICNpcnt = icspcnt = icl4cnt = ics4cnt = 0;
  ICNpcur = icspcur = icl4cur = ics4cur = 0;

  for (count = 0; count < Mac_typeCount; count++)
    switch (MacResType (Mac_types[count]))
      {
      case Mac_ICNp:
	if (cvtICNp)
	  ICNpcnt++;
	break;
      case Mac_icsp:
	if (cvticsp)
	  icspcnt++;
	break;
      case Mac_icl4:
	if (cvticl4)
	  icl4cnt++;
	break;
      case Mac_ics4:
	if (cvtics4)
	  ics4cnt++;
	break;
      default:
	break;
      }

  generic_count = ICNpcnt + icspcnt + icl4cnt + ics4cnt;

  Mac_data = (MacDataStruct *) malloc (sizeof(MacDataStruct) * Mac_typeCount);
  dataptr  = Mac_data;

  for (count = 0; count < Mac_typeCount; count++)
    {
      if (MacResType (Mac_types[count]) == Mac_ICNp
	  || MacResType (Mac_types[count]) == Mac_icsp
	  || MacResType (Mac_types[count]) == Mac_icl4
	  || MacResType (Mac_types[count]) == Mac_ics4)
	{
	  fseek (input, 
		 Mac_header.DataOffset+Mac_refs[count].DataOffset, 
		 SEEK_SET);
	  
	  dataptr->size = read_macint();
	  dataptr->map  = (BYTE *) malloc (dataptr->size);
	  
	  fread (dataptr->map, 1, dataptr->size, input);

	  if (MacResType (Mac_types[count]) == Mac_ICNp)
	    {
	      for (place = 128; place < 256; place++)
		dataptr->map[place] ^= 0xFF;
	    }
	  else
	    if (MacResType (Mac_types[count]) == Mac_icsp)
	      {
		for (place = 32; place < 64; place++)
		  dataptr->map[place] ^= 0xFF;
	      }
	}
      dataptr++;
    }

  generic = (generic_info *) malloc (sizeof(generic_info) * generic_count);

  for (count = 0; count < generic_count; /* nop */)
    {
      init_generic(count);

      if (icl4cur < icl4cnt)
	{
	  dataptr = find_mac_data (++icl4cur, Mac_icl4);
	  maskptr = find_mac_data (icl4cur, Mac_ICNp);

	  generic[count].and_map = mac_init_generic(32, 32, 1, maskptr->map + 128);
	  generic[count].xor_map = mac_empty_map(32, 32, 1);
	  generic[count].clr_map = mac_init_generic(32, 32, 4, dataptr->map);

	  if (dump_input)
	    dump_maps(generic[count]);

	  /* Translate the colors in the map */
	  if (!MacPalette)
	    for (row = 1; row <= generic[count].clr_map.height; row++)
	      {
		for (col = 1; col <= generic[count].clr_map.width; col++)
		  {
		    ch = get_bit(generic[count].clr_map, row, col);
		    ch = XlateArr[ch];
		    set_bit(generic[count].clr_map, row, col, ch);
		  }
	      }

	  fix_transparency (generic[count].and_map, generic[count].clr_map);
	  
	  count++;
	}

      if (ICNpcur < ICNpcnt)
	{
	  dataptr = find_mac_data (++ICNpcur, Mac_ICNp);
	  maskptr = dataptr;

	  generic[count].and_map = mac_init_generic(32, 32, 1, maskptr->map + 128);
	  generic[count].xor_map = mac_empty_map(32, 32, 1);
	  generic[count].clr_map = mac_init_generic(32, 32, 1, dataptr->map);

	  if (dump_input)
	    dump_maps(generic[count]);

	  for (row = 1; row <= generic[count].clr_map.height; row++)
	    for (col = 1; col <= generic[count].clr_map.width; col++)
	      if (get_bit (generic[count].clr_map, row, col))
		set_bit (generic[count].clr_map, row, col, 0);
	      else
		set_bit (generic[count].clr_map, row, col, 1);

	  fix_transparency (generic[count].and_map, generic[count].clr_map);
	  
	  count++;
	}

      if (ics4cur < ics4cnt)
	{
	  dataptr = find_mac_data (++ics4cur, Mac_ics4);
	  maskptr = find_mac_data (ics4cur, Mac_icsp);

	  generic[count].and_map = mac_init_generic(16, 16, 1, maskptr->map + 32);
	  generic[count].xor_map = mac_empty_map(16, 16, 1);
	  generic[count].clr_map = mac_init_generic(16, 16, 4, dataptr->map);

	  if (dump_input)
	    dump_maps(generic[count]);

	  /* Translate the colors in the map */
	  if (!MacPalette)
	    for (row = 1; row <= generic[count].clr_map.height; row++)
	      for (col = 1; col <= generic[count].clr_map.width; col++)
		{
		  ch = get_bit(generic[count].clr_map, row, col);
		  ch = XlateArr[ch];
		  set_bit(generic[count].clr_map, row, col, ch);
		}

	  fix_transparency (generic[count].and_map, generic[count].clr_map);
	  
	  count++;
	}
	  
      if (icspcur < icspcnt)
	{
	  dataptr = find_mac_data (++icspcur, Mac_icsp);
	  maskptr = dataptr;

	  generic[count].and_map = mac_init_generic(16, 16, 1, maskptr->map + 32);
	  generic[count].xor_map = mac_empty_map(16, 16, 1);
	  generic[count].clr_map = mac_init_generic(16, 16, 1, dataptr->map);

	  if (dump_input)
	    dump_maps(generic[count]);

	  for (row = 1; row <= generic[count].clr_map.height; row++)
	    for (col = 1; col <= generic[count].clr_map.width; col++)
	      if (get_bit (generic[count].clr_map, row, col))
		set_bit (generic[count].clr_map, row, col, 0);
	      else
		set_bit (generic[count].clr_map, row, col, 1);

	  fix_transparency (generic[count].and_map, generic[count].clr_map);
	  fix_small_map (generic[count].and_map, generic[count].clr_map);
	  
	  count++;
	}
    }
}
