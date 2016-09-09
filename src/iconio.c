/* This file is iconio.c (part of XIcon)
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
#include "iconio.h"
#include "icondata.h"
#include "macdata.h"
#include "macread.h"

int read_int()
{
  unsigned char Ca[4];
  int datum;

  rc = fread(&Ca, 1, 4, input);
  datum = (Ca[3] << 24) + (Ca[2] << 16) + (Ca[1] << 8) + (Ca[0]);
  return datum;

/*
  int datum;
  rc = fread(&datum, sizeof (datum), 1, input);
  return datum;
*/
}

short int read_shortint()
{
  unsigned char Ca[2];
  int datum;

  rc = fread(&Ca, 1, 2, input);
  datum = (Ca[1] << 8) + (Ca[0]);
  return datum;

/*
  short int datum;
  rc = fread(&datum, sizeof (datum), 1, input);
  return datum;
*/
}

unsigned char read_char()
{
  unsigned char datum;
  rc = fread(&datum, sizeof (datum), 1, input);
  return datum;
}

int read_macint()
{
  unsigned char Ca[4];
  int datum;

  rc = fread(&Ca, 1, 4, input);
  datum = (Ca[0] << 24) + (Ca[1] << 16) + (Ca[2] << 8) + (Ca[3]);
  return datum;
}

int read_macint3()
{
  unsigned char Ca[3];
  int datum;

  rc = fread(&Ca, 1, 3, input);
  datum = (Ca[0] << 16) + (Ca[1] << 8) + (Ca[2]);
  return datum;
}

short int read_macshortint()
{
  unsigned char Ca[2];
  int datum;

  rc = fread(&Ca, 1, 2, input);
  datum = (Ca[0] << 8) + (Ca[1]);
  return datum;
}

unsigned char read_macchar()
{
  return read_char();
}

void write_int(unsigned int datum)
{
  rc = fwrite(&datum, sizeof (datum), 1, output);
}

void write_shortint(unsigned short int datum)
{
  rc = fwrite(&datum, sizeof (datum), 1, output);
}

void write_char(unsigned char datum)
{
  rc = fwrite(&datum, sizeof (datum), 1, output);
}

enum iconFileType QueryFileType()
{
  USHORT id;
  USHORT type;
  RsrcHdrStruct MacHeader;
  RsrcMapStruct MacMap;
  char bytes[5];

  /* First, is this thing likely to be a Mac icon? */
  fseek(input, 0, SEEK_SET);
  MacHeader = read_mac_header();
  fseek(input, MacHeader.MapOffset, SEEK_SET);
  MacMap = read_mac_map();

  if (MacHeader.MapOffset != 0
      && MacHeader.DataOffset == MacMap.MapCopy[0]
      && MacHeader.MapOffset == MacMap.MapCopy[1]
      && MacHeader.DataLen == MacMap.MapCopy[2]
      && MacHeader.MapLen == MacMap.MapCopy[3])
    return MacIcon;

  fseek(input, 0, SEEK_SET);
  id = read_shortint();
  type = read_int();

  fseek(input, 0, SEEK_SET);
  fread(&bytes, 1, 4, input);
  bytes[4] = 0;

  switch (id)
    {
    case 0x4142:
      if (type == 0x5C)
        return OS2Icon20;
      else
        if (type == 0x28)
          return OS2Icon12;
      break;
    case 0x4943:
      if (type == 0x4E)
	return OS2Icon20;
      else 
	if (type == 0x1A)
	  return OS2Icon12;
      break;
    case 0x0000:
      if (type = 0x0001)
        return WinIcon;
      break;
    default:
      if (strcmp(bytes, "#def") == 0)
	return XBMicon;
      break;
    }

  return UnkFile;
}


