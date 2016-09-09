/* This file is winread.c (part of XIcon)
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
#include "windata.h"
#include "iconio.h"
#include "iconvars.h"
#include "generic.h"
#include "palette.h"
#include "bitmap.h"

win_rgb_info read_win_rgb()
{
  win_rgb_info p;

  p.red = read_char();
  p.green = read_char();
  p.blue = read_char();
  p.unused = read_char();

  if (dump_input)
    printf("\t\tred: %3d green: %3d blue: %3d\n",
	   p.red, p.green, p.blue);
  
  return p;
}

WinIconDirEntry read_win_direntry()
{
  WinIconDirEntry e;

  e.bWidth = read_char();
  e.bHeight = read_char();
  e.bColorCount = read_char();
  e.bReserved = read_char();
  e.wPlanes = read_shortint();
  e.wBitCount = read_shortint();
  e.dwBytesInRes = read_int();
  e.dwImageOffset = read_int();

  if (dump_input)
    {
      printf("Windows dir entry:\n");
      printf("Width        (1) :   %d\n", e.bWidth);
      printf("Height       (1) :   %d\n", e.bHeight);
      printf("ColorCount   (1) :   %d\n", e.bColorCount);
      printf("Reserved     (1) :   %d\n", e.bReserved);
      printf("Planes       (2) :   %d\n", e.wPlanes);
      printf("BitCount     (2) :   %d\n", e.wBitCount);
      printf("BytesInRes   (4) :   %d\n", e.dwBytesInRes);
      printf("Image Offset (4) : 0x%04x\n", e.dwImageOffset);
      printf("\n");
    }

  return e;
}

WinIconHeader read_win_header()
{
  WinIconHeader h;

  h.idReserved = read_shortint();
  h.idType = read_shortint();
  h.idCount = read_shortint();

  if (dump_input)
    {
      printf("Windows header:\n");
      printf("idReserved   (2) :   %d\n", h.idReserved);
      printf("idType       (2) :   %d\n", h.idType);
      printf("idCount      (2) :   %d\n", h.idCount);
      printf("\n");
    }

  return h;
}

WinIconBitInfoHdr read_win_bitinfo()
{
  WinIconBitInfoHdr b;
  int count;

  b.biSize = read_int();
  b.biWidth = read_int();
  b.biHeight = read_int();
  b.biPlanes = read_shortint();
  b.biBitCount = read_shortint();
  b.biCompression = read_int();
  b.biSizeImage = read_int();
  b.biXPelsPerMeter = read_int();
  b.biYPelsPerMeter = read_int();
  b.biClrUsed = read_int();
  b.biClrImportant = read_int();

  b.biClrUsed = 2 << (b.biBitCount - 1);

  if (dump_input)
    {
      printf("Windows bit info hdr:\n");
      printf("biSize       (4) :   %d\n", b.biSize);
      printf("biWidth      (4) :   %d\n", b.biWidth);
      printf("biHeight     (4) :   %d\n", b.biHeight);
      printf("biPlanes     (2) :   %d\n", b.biPlanes);
      printf("biBitCount   (2) :   %d\n", b.biBitCount);
      printf("biCompression(4) :   %d\n", b.biCompression);
      printf("biSizeImage  (4) :   %d\n", b.biSizeImage);
      printf("biXPels/m    (4) :   %d\n", b.biXPelsPerMeter);
      printf("biYPels/m    (4) :   %d\n", b.biYPelsPerMeter);
      printf("biClrUsed    (4) :   %d\n", b.biClrUsed);
      printf("biClrImport. (4) :   %d\n", b.biClrImportant);
      printf("\n");
    }

  b.p = (win_rgb_info *) malloc (sizeof(win_rgb_info) * b.biClrUsed);
  for (count = 0; count < b.biClrUsed; count++)
    b.p[count] = read_win_rgb();

  return b;
}

bitmap_info read_win_clr_map (WinIconBitInfoHdr b)
{
  bitmap_info i;
  BYTE *buf, *RowPtr, *sch, *dch;
  int count, place;

  i.width = b.biWidth;
  i.height = b.biHeight / 2;
  i.bits_per_pixel = b.biBitCount;
  i.bytewidth = ((i.width * i.bits_per_pixel) + 7) / 8;
  i.size = i.bytewidth * i.height;
  i.num_colors = 2 << (i.bits_per_pixel - 1);

  i.palette = (rgb_info *) malloc (sizeof (rgb_info) * i.num_colors);
  for (count = 0; count < i.num_colors; count++)
    {
      i.palette[count].red = b.p[count].red;
      i.palette[count].green = b.p[count].green;
      i.palette[count].blue = b.p[count].blue;
    }

  buf = (BYTE *) malloc(i.size);
  i.map = (BYTE *) malloc(i.size);

  for (count = 0; count < i.size; count++)
    buf[count] = read_char();

  dch = i.map;
  RowPtr = buf + i.size - i.bytewidth;
  while (RowPtr >= buf)
    {
      sch = RowPtr;
      for (place = 0; place < i.bytewidth; place++)
	{
	  *dch = *sch;
	  dch++;
	  sch++;
	}

      RowPtr -= i.bytewidth;
    }

  free(buf);

  if (dump_input)
    {
      printf("ClrMap:\n");
      printf("width, height: %d, %d\n", i.width, i.height);
      printf("bits, size: %d, %d\n", i.bits_per_pixel, i.size);
      printmap(i);
    }

  return i;
}

bitmap_info read_win_and_map (WinIconBitInfoHdr b)
{
  bitmap_info i;
  BYTE *buf, *RowPtr, *sch, *dch;
  int count, place;

  i.width = b.biWidth;
  i.height = b.biHeight / 2;
  i.bits_per_pixel = 1;
  i.bytewidth = (i.width + 7) / 8;
  i.size = i.bytewidth * i.height;
  i.num_colors = 2;
  i.palette = bw_palette;

  buf = (BYTE *) malloc(i.size);
  i.map = (BYTE *) malloc(i.size);

  for (count = 0; count < i.size; count++)
    buf[count] = read_char();

  dch = i.map;
  RowPtr = buf + i.size - i.bytewidth;
  while (RowPtr >= buf)
    {
      sch = RowPtr;
      for (place = 0; place < i.bytewidth; place++)
	{
	  *dch = *sch;
	  dch++;
	  sch++;
	}

      RowPtr -= i.bytewidth;
    }

  free(buf);

  if (dump_input)
    {
      printf("AndMap:\n");
      printf("width, height: %d, %d\n", i.width, i.height);
      printf("bits, size: %d, %d\n", i.bits_per_pixel, i.size);
      printmap(i);
    }

  return i;
}

bitmap_info win_empty_bw_map(WinIconBitInfoHdr b)
{
  bitmap_info m;

  m = init_empty_map(b.biWidth, b.biHeight / 2, 1);
  m.palette = bw_palette;

  return m;
}

void read_win_file()
{
  WinIconHeader Win_header;
  WinIconDirEntry *Win_direntries;
  WinIconBitInfoHdr *Win_bitinfo;
  int count, row, col;
  BYTE ch;

  fseek (input, 0, SEEK_SET);

  Win_header = read_win_header();

  Win_direntries = (WinIconDirEntry *) malloc(sizeof(WinIconDirEntry) *
					      Win_header.idCount);

  Win_bitinfo = (WinIconBitInfoHdr *) malloc(sizeof(WinIconBitInfoHdr) *
					     Win_header.idCount);

  for (count = 0; count < Win_header.idCount; count++)
    Win_direntries[count] = read_win_direntry();

  generic_count = Win_header.idCount;
  generic = (generic_info *) malloc(sizeof(generic_info) * 
				    Win_header.idCount);

  for (count = 0; count < Win_header.idCount; count++)
    {
      fseek (input, Win_direntries[count].dwImageOffset, SEEK_SET);
      Win_bitinfo[count] = read_win_bitinfo();
      generic[count].clr_map = read_win_clr_map(Win_bitinfo[count]);
      generic[count].and_map = read_win_and_map(Win_bitinfo[count]);
      generic[count].xor_map = win_empty_bw_map(Win_bitinfo[count]);

      for (row = 1; row <= generic[count].and_map.height; row++)
	for (col = 1; col <= generic[count].clr_map.width; col++)
	  {
	    ch = get_bit (generic[count].and_map, row, col);
	    if (ch)
	      set_bit (generic[count].clr_map, row, col, (BYTE) 0xFF);
	  }
    }
}

