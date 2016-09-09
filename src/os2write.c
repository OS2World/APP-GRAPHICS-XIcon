/* This file is os2write.c (part of XIcon)
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
#include "os2data.h"
#include "os2write.h"

int os2_map_size(bitmap_info map)
{
  int bytewidth;

  bytewidth = ((map.width*map.bits_per_pixel) + 7) / 8;

  while (bytewidth % 4 != 0)
    bytewidth++;

  return bytewidth * map.height;
}

void write_os2_map (bitmap_info map)
{
  BYTE *rowptr, *ch;
  int  padbytes, count;

  count = map.bytewidth;
  padbytes = 0;
  while (count % 4 != 0)
    {
      count++;
      padbytes++;
    }

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

void write_os2_icon12(os2_12_icon_info icon)
{
  int color_count, count;
  BYTE *ch;

  write_shortint(icon.header.flag);
  write_int(icon.header.cbsize);
  write_int(icon.header.next);
  write_shortint(icon.header.display_width);
  write_shortint(icon.header.display_height);

  write_shortint(icon.andxor_map.flag);
  write_int(icon.andxor_map.cbsize);
  write_shortint(icon.andxor_map.hotx);
  write_shortint(icon.andxor_map.hoty);
  write_int(icon.andxor_map.bitmap_offset);
  write_int(icon.andxor_map.bitmap_size);
  write_shortint(icon.andxor_map.width);
  write_shortint(icon.andxor_map.height);
  write_shortint(icon.andxor_map.planes);
  write_shortint(icon.andxor_map.bits_per_pixel);

  color_count = 2 << (icon.andxor_map.bits_per_pixel - 1);
  ch = (char *) icon.andxor_map.p;
  for (count = 0; count < (sizeof(os2_12_rgb_info) * color_count); count++)
    {
      write_char(*ch);
      ch++;
    }

  write_shortint(icon.clr_map.flag);
  write_int(icon.clr_map.cbsize);
  write_shortint(icon.clr_map.hotx);
  write_shortint(icon.clr_map.hoty);
  write_int(icon.clr_map.bitmap_offset);
  write_int(icon.clr_map.bitmap_size);
  write_shortint(icon.clr_map.width);
  write_shortint(icon.clr_map.height);
  write_shortint(icon.clr_map.planes);
  write_shortint(icon.clr_map.bits_per_pixel);

  color_count = 2 << (icon.clr_map.bits_per_pixel - 1);
  ch = (char *) icon.clr_map.p;
  for (count = 0; count < (sizeof(os2_12_rgb_info) * color_count); count++)
    {
      write_char(*ch);
      ch++;
    }
}

void write_os2_file12()
{
  int *info_offsets, *and_map_offsets, *clr_map_offsets;
  int cur_offset;
  os2_12_icon_info icon;
  int count;

  fseek(output, 0, SEEK_SET);

  info_offsets = (int *) malloc (sizeof (int) * (generic_count+1));
  and_map_offsets = (int *) malloc (sizeof (int) * (generic_count+1));
  clr_map_offsets = (int *) malloc (sizeof (int) * (generic_count+1));

  /* Figure out what the offsets will be for the headers ... */
  cur_offset = 0;
  for (count = 0; count < generic_count; count++)
    {
      info_offsets[count] = cur_offset;

      cur_offset += 26+26+14;                    /* sizeof constant parts */
      cur_offset += sizeof(os2_12_rgb_info)*2;   /* sizeof b/w palette */
      cur_offset += sizeof(os2_12_rgb_info)*generic[count].clr_map.num_colors;
                                                 /* sizeof color palette */
    }

  /* And figure out what the offsets will be for the bitmaps ... */
  for (count = 0; count < generic_count; count++)
    {
      and_map_offsets[count] = cur_offset;
      
      cur_offset += os2_map_size(generic[count].and_map);
      cur_offset += os2_map_size(generic[count].xor_map);

      clr_map_offsets[count] = cur_offset;

      cur_offset += os2_map_size(generic[count].clr_map);
    }

  info_offsets[generic_count] = 0;
  and_map_offsets[generic_count] = 0;
  clr_map_offsets[generic_count] = 0;

  /*  Write the headers to the file ... */
  for (count = 0; count < generic_count; count++)
    {
      icon.header.flag = 0x4142;
      icon.header.cbsize = 0x28;
      icon.header.next = info_offsets[count+1];
      icon.header.display_width = 0;
      icon.header.display_height = 0;

      icon.andxor_map.flag = 0x4943;
      icon.andxor_map.cbsize = 0x1A;
      icon.andxor_map.hotx = generic[count].and_map.width / 2;
      icon.andxor_map.hoty = generic[count].and_map.height / 2;
      icon.andxor_map.bitmap_offset = and_map_offsets[count];
      icon.andxor_map.bitmap_size = 12;
      icon.andxor_map.width = generic[count].and_map.width;
      icon.andxor_map.height = generic[count].and_map.height * 2;
      icon.andxor_map.planes = 1;
      icon.andxor_map.bits_per_pixel = 1;
      icon.andxor_map.p = (os2_12_rgb_info *) generic[count].and_map.palette;

      icon.clr_map.flag = 0x4943;
      icon.clr_map.cbsize = 0x1A;
      icon.clr_map.hotx = generic[count].clr_map.width / 2;
      icon.clr_map.hoty = generic[count].clr_map.height / 2;
      icon.clr_map.bitmap_offset = clr_map_offsets[count];
      icon.clr_map.bitmap_size = 12;
      icon.clr_map.width = generic[count].clr_map.width;
      icon.clr_map.height = generic[count].clr_map.height;
      icon.clr_map.planes = 1;
      icon.clr_map.bits_per_pixel = generic[count].clr_map.bits_per_pixel;
      icon.clr_map.p = (os2_12_rgb_info *) generic[count].clr_map.palette;

      write_os2_icon12(icon);
    }

  for (count = 0; count < generic_count; count++)
    {
      write_os2_map(generic[count].xor_map);
      write_os2_map(generic[count].and_map);
      write_os2_map(generic[count].clr_map);
    }
}

void write_os2_icon20(os2_20_icon_info icon)
{
  int color_count, count;
  BYTE *ch;

  write_shortint(icon.header.flag);
  write_int(icon.header.cbsize);
  write_int(icon.header.next);
  write_shortint(icon.header.display_width);
  write_shortint(icon.header.display_height);

  write_shortint(icon.andxor_map.flag);
  write_int(icon.andxor_map.cbsize);
  write_shortint(icon.andxor_map.hotx);
  write_shortint(icon.andxor_map.hoty);
  write_int(icon.andxor_map.bitmap_offset);
  write_int(icon.andxor_map.length);
  write_int(icon.andxor_map.width);
  write_int(icon.andxor_map.height);
  write_shortint(icon.andxor_map.planes);
  write_shortint(icon.andxor_map.bits_per_pixel);
  write_int(icon.andxor_map.compression);
  write_int(icon.andxor_map.bitmap_size);
  write_int(icon.andxor_map.devXres);
  write_int(icon.andxor_map.devYres);
  write_int(icon.andxor_map.clrUsed);
  write_int(icon.andxor_map.clrImportant);
  write_shortint(icon.andxor_map.units);
  write_shortint(icon.andxor_map.reserved);
  write_shortint(icon.andxor_map.recording);
  write_shortint(icon.andxor_map.rendering);
  write_int(icon.andxor_map.size1);
  write_int(icon.andxor_map.size2);
  write_int(icon.andxor_map.color_encoding);
  write_int(icon.andxor_map.app_identifier);

  color_count = 2 << (icon.andxor_map.bits_per_pixel - 1);
  ch = (char *) icon.andxor_map.p;
  for (count = 0; count < (sizeof(os2_20_rgb_info) * color_count); count++)
    {
      write_char(*ch);
      ch++;
    }

  write_shortint(icon.clr_map.flag);
  write_int(icon.clr_map.cbsize);
  write_shortint(icon.clr_map.hotx);
  write_shortint(icon.clr_map.hoty);
  write_int(icon.clr_map.bitmap_offset);
  write_int(icon.clr_map.length);
  write_int(icon.clr_map.width);
  write_int(icon.clr_map.height);
  write_shortint(icon.clr_map.planes);
  write_shortint(icon.clr_map.bits_per_pixel);
  write_int(icon.clr_map.compression);
  write_int(icon.clr_map.bitmap_size);
  write_int(icon.clr_map.devXres);
  write_int(icon.clr_map.devYres);
  write_int(icon.clr_map.clrUsed);
  write_int(icon.clr_map.clrImportant);
  write_shortint(icon.clr_map.units);
  write_shortint(icon.clr_map.reserved);
  write_shortint(icon.clr_map.recording);
  write_shortint(icon.clr_map.rendering);
  write_int(icon.clr_map.size1);
  write_int(icon.clr_map.size2);
  write_int(icon.clr_map.color_encoding);
  write_int(icon.clr_map.app_identifier);

  color_count = 2 << (icon.clr_map.bits_per_pixel - 1);
  ch = (char *) icon.clr_map.p;
  for (count = 0; count < (sizeof(os2_20_rgb_info) * color_count); count++)
    {
      write_char(*ch);
      ch++;
    }
}

os2_20_rgb_info *conv_palette (bitmap_info map)
{
  os2_20_rgb_info *p;
  int count;

  p = (os2_20_rgb_info *) malloc (sizeof (os2_20_rgb_info) * 
				  map.num_colors);

  for (count = 0; count < map.num_colors; count++)
    {
      p[count].red = map.palette[count].red;
      p[count].green = map.palette[count].green;
      p[count].blue = map.palette[count].blue;
      p[count].unused = 0;
    }

  return p;
}

void write_os2_file20()
{
  int *info_offsets, *and_map_offsets, *clr_map_offsets;
  int cur_offset;
  os2_20_icon_info icon;
  int count;

  fseek(output, 0, SEEK_SET);

  info_offsets = (int *) malloc (sizeof (int) * (generic_count+1));
  and_map_offsets = (int *) malloc (sizeof (int) * (generic_count+1));
  clr_map_offsets = (int *) malloc (sizeof (int) * (generic_count+1));

  /* Figure out what the offsets will be for the headers ... */
  cur_offset = 0;
  for (count = 0; count < generic_count; count++)
    {
      info_offsets[count] = cur_offset;

      cur_offset += 78+78+14;                    /* sizeof constant parts */
      cur_offset += sizeof(os2_20_rgb_info)*2;   /* sizeof b/w palette */
      cur_offset += sizeof(os2_20_rgb_info)*generic[count].clr_map.num_colors;
                                                 /* sizeof color palette */
    }

  /* And figure out what the offsets will be for the bitmaps ... */
  for (count = 0; count < generic_count; count++)
    {
      and_map_offsets[count] = cur_offset;
      
      cur_offset += os2_map_size(generic[count].and_map);
      cur_offset += os2_map_size(generic[count].xor_map);

      clr_map_offsets[count] = cur_offset;

      cur_offset += os2_map_size(generic[count].clr_map);
    }

  info_offsets[generic_count] = 0;
  and_map_offsets[generic_count] = 0;
  clr_map_offsets[generic_count] = 0;

  /*  Write the headers to the file ... */
  for (count = 0; count < generic_count; count++)
    {
      icon.header.flag = 0x4142;
      icon.header.cbsize = 0x5C;
      icon.header.next = info_offsets[count+1];
      icon.header.display_width = 0;
      icon.header.display_height = 0;

      icon.andxor_map.flag = 0x4943;
      icon.andxor_map.cbsize = 0x4E;
      icon.andxor_map.hotx = generic[count].and_map.width / 2;
      icon.andxor_map.hoty = generic[count].and_map.height / 2;
      icon.andxor_map.bitmap_offset = and_map_offsets[count];
      icon.andxor_map.length = 64;
      icon.andxor_map.width = generic[count].and_map.width;
      icon.andxor_map.height = generic[count].and_map.height * 2;
      icon.andxor_map.planes = 1;
      icon.andxor_map.bits_per_pixel = 1;
      icon.andxor_map.compression = 0;
      icon.andxor_map.bitmap_size =   os2_map_size(generic[count].and_map) 
	                            + os2_map_size(generic[count].xor_map);
      icon.andxor_map.devXres = 0;
      icon.andxor_map.devYres = 0;
      icon.andxor_map.clrUsed = 2;
      icon.andxor_map.clrImportant = 2;
      icon.andxor_map.units = 0;
      icon.andxor_map.reserved = 0;
      icon.andxor_map.recording = 0;
      icon.andxor_map.rendering = 0;
      icon.andxor_map.size1 = 0;
      icon.andxor_map.size2 = 0;
      icon.andxor_map.color_encoding = 0;
      icon.andxor_map.app_identifier = 0;
      icon.andxor_map.p = conv_palette(generic[count].and_map);

      icon.clr_map.flag = 0x4943;
      icon.clr_map.cbsize = 0x4E;
      icon.clr_map.hotx = generic[count].clr_map.width / 2;
      icon.clr_map.hoty = generic[count].clr_map.height / 2;
      icon.clr_map.bitmap_offset = clr_map_offsets[count];
      icon.clr_map.length = 64;
      icon.clr_map.width = generic[count].clr_map.width;
      icon.clr_map.height = generic[count].clr_map.height;
      icon.clr_map.planes = 1;
      icon.clr_map.bits_per_pixel = generic[count].clr_map.bits_per_pixel;
      icon.clr_map.compression = 0;
      icon.clr_map.bitmap_size = os2_map_size(generic[count].clr_map);
      icon.clr_map.devXres = 0;
      icon.clr_map.devYres = 0;
      icon.clr_map.clrUsed = generic[count].clr_map.num_colors;

      if (icon.clr_map.clrUsed <= 2)
        icon.clr_map.clrImportant = icon.clr_map.clrUsed;
      else
        icon.clr_map.clrImportant = 0;

      icon.clr_map.units = 0;
      icon.clr_map.reserved = 0;
      icon.clr_map.recording = 0;
      icon.clr_map.rendering = 0;
      icon.clr_map.size1 = 0;
      icon.clr_map.size2 = 0;
      icon.clr_map.color_encoding = 0;
      icon.clr_map.app_identifier = 0;
      icon.clr_map.p = conv_palette(generic[count].clr_map);

      write_os2_icon20(icon);
    }

  for (count = 0; count < generic_count; count++)
    {
      write_os2_map(generic[count].xor_map);
      write_os2_map(generic[count].and_map);
      write_os2_map(generic[count].clr_map);
    }
}
