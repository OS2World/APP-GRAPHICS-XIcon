/* This file is winwrite.c (part of XIcon)
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
#include "icondata.h"
#include "iconvars.h"
#include "iconio.h"
#include "windata.h"
#include "winwrite.h"

win_rgb_info *convert_palette(rgb_info *p, int cnt)
{
  int count;
  win_rgb_info *w = (win_rgb_info *) malloc(cnt * sizeof(win_rgb_info));

  for (count = 0; count < cnt; count++, p++)
    {
      w[count].red = p->red;
      w[count].green = p->green;
      w[count].blue = p->blue;
      w[count].unused = 0;
    }

  return w;
}

int win_map_size(bitmap_info map)
{
  int bytewidth;

  bytewidth = ((map.width*map.bits_per_pixel) + 7) / 8;

/*
  while (bytewidth % 4 != 0)
    bytewidth++;
*/

  return bytewidth * map.height;
}

void write_win_map (bitmap_info map)
{
  BYTE *rowptr, *ch;
  int  padbytes, count;

  count = map.bytewidth;
  padbytes = 0;

/*
  while (count % 4 != 0)
    {
      count++;
      padbytes++;
    }
*/

  rowptr = map.map + map.size - map.bytewidth;
  while (rowptr >= map.map)
    {
      for (count = 0, ch = rowptr; count < map.bytewidth; count++, ch++)
	write_char(*ch);

      for (count = 0; count < padbytes; count++)
	write_char(0);
      
      rowptr -= map.bytewidth;
    }
}

void write_win_direntry(WinIconDirEntry direntry)
{
  write_char(direntry.bWidth);
  write_char(direntry.bHeight);
  write_char(direntry.bColorCount);
  write_char(direntry.bReserved);
  write_shortint(direntry.wPlanes);
  write_shortint(direntry.wBitCount);
  write_int(direntry.dwBytesInRes);
  write_int(direntry.dwImageOffset);
}

void write_win_icon(WinIconBitInfoHdr icon)
{
  int color_count, count;
  BYTE *ch;

  write_int(icon.biSize);
  write_int(icon.biWidth);
  write_int(icon.biHeight);
  write_shortint(icon.biPlanes);
  write_shortint(icon.biBitCount);
  write_int(icon.biCompression);
  write_int(icon.biSizeImage);
  write_int(icon.biXPelsPerMeter);
  write_int(icon.biYPelsPerMeter);
  write_int(icon.biClrUsed);
  write_int(icon.biClrImportant);

  color_count = 2 << (icon.biBitCount - 1);
  ch = (char *) icon.p;
  for (count = 0; count < (sizeof(win_rgb_info) * color_count); count++)
    {
      write_char(*ch);
      ch++;
    }
}

void write_win_file()
{
  int *info_offsets;
  int cur_offset;
  WinIconDirEntry icon_entry;
  WinIconBitInfoHdr icon;
  int count;
  BYTE *ch;

  fseek(output, 0, SEEK_SET);

  info_offsets = (int *) malloc (sizeof (int) * (generic_count+1));

  /* Figure out what the offsets will be for the headers ... */
  cur_offset = 6 + (16 * generic_count);
  for (count = 0; count < generic_count; count++)
    {
      info_offsets[count] = cur_offset;

      cur_offset += 40;                    /* sizeof constant parts */
      cur_offset += sizeof(win_rgb_info)*generic[count].clr_map.num_colors;
                                           /* sizeof color palette */
      cur_offset += win_map_size(generic[count].clr_map);
      cur_offset += win_map_size(generic[count].and_map);
    }

  info_offsets[generic_count] = cur_offset;

  /* Write the header ... */
  write_shortint(0);
  write_shortint(1);
  write_shortint(generic_count);

  /*  Write the dir entries to the file ... */
  for (count = 0; count < generic_count; count++)
    {
      icon_entry.bWidth = generic[count].clr_map.width;
      icon_entry.bHeight = generic[count].clr_map.height;
      icon_entry.bColorCount = generic[count].clr_map.num_colors;
      icon_entry.bReserved = 0;
      icon_entry.wPlanes = 1;
      icon_entry.wBitCount = generic[count].clr_map.bits_per_pixel;
      icon_entry.dwBytesInRes = info_offsets[count+1] - info_offsets[count];
      icon_entry.dwImageOffset = info_offsets[count];
      write_win_direntry(icon_entry);
    }

  /*  Write the headers to the file ... */
  for (count = 0; count < generic_count; count++)
    {
      icon.biSize = 40; /* ? */
      icon.biWidth = generic[count].clr_map.width;
      icon.biHeight = generic[count].clr_map.height * 2;
      icon.biPlanes = 1;
      icon.biBitCount = generic[count].clr_map.bits_per_pixel;
      icon.biCompression = 0;
      icon.biSizeImage = win_map_size(generic[count].and_map)+
	                 win_map_size(generic[count].clr_map);
      icon.biXPelsPerMeter = 0;
      icon.biYPelsPerMeter = 0;
      icon.biClrUsed = generic[count].clr_map.num_colors;

      if (generic[count].clr_map.num_colors == 2)
	icon.biClrImportant = 2;
      else
	icon.biClrImportant = 0;

      icon.p = convert_palette(generic[count].clr_map.palette,
			       generic[count].clr_map.num_colors);
      write_win_icon(icon);

      write_win_map(generic[count].clr_map);
      write_win_map(generic[count].and_map);
    }
}

