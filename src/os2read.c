/* This file is os2read.c (part of XIcon)
 *
 * Copyright (C) 1993,94 by Norman Walsh
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
#include "os2data.h"
#include "iconio.h"

#define MAX_ICONS 100
generic_info *g;

int two_to_the (int power) 
{
  int x = 1;
  while (power > 0) {
    x <<= 1;
    power--;
  }

  return x;
}

os2_12_bitmap_info read_os2_12_bitmap_info() 
{
  os2_12_bitmap_info i;
  int palette_entries;
  int count;

  i.flag = read_shortint();
  i.cbsize = read_int();
  i.hotx = read_shortint();
  i.hoty = read_shortint();
  i.bitmap_offset = read_int();
  i.bitmap_size = read_int();
  i.width = read_shortint();
  i.height = read_shortint();
  i.planes = read_shortint();
  i.bits_per_pixel = read_shortint();

  palette_entries = two_to_the(i.bits_per_pixel) * i.planes;
  i.p = (os2_12_rgb_info *) malloc(palette_entries * sizeof(os2_12_rgb_info));

  for (count = 0; count < palette_entries; count++) {
    i.p[count].blue = read_char();
    i.p[count].green = read_char();
    i.p[count].red = read_char();
  }

  return i;
}

os2_20_bitmap_info read_os2_20_bitmap_info() 
{
  os2_20_bitmap_info i;
  int palette_entries;
  int count;

  i.flag = read_shortint();
  i.cbsize = read_int();
  i.hotx = read_shortint();
  i.hoty = read_shortint();
  i.bitmap_offset = read_int();
  i.length = read_int();
  i.width = read_int();
  i.height = read_int();
  i.planes = read_shortint();
  i.bits_per_pixel = read_shortint();
  i.compression = read_int();
  i.bitmap_size = read_int();
  i.devXres = read_int();
  i.devYres = read_int();
  i.clrUsed = read_int();
  i.clrImportant = read_int();
  i.units = read_shortint();
  i.reserved = read_shortint();
  i.recording = read_shortint();
  i.rendering = read_shortint();
  i.size1 = read_int();
  i.size2 = read_int();
  i.color_encoding = read_int();
  i.app_identifier = read_int();

  palette_entries = two_to_the(i.bits_per_pixel) * i.planes;
  i.p = (os2_20_rgb_info *) malloc(palette_entries * sizeof(os2_20_rgb_info));

  for (count = 0; count < palette_entries; count++) {
    i.p[count].blue = read_char();
    i.p[count].green = read_char();
    i.p[count].red = read_char();
    i.p[count].unused = read_char();
  }

  return i;
}

/*
void dump_os2_icon_header (os2_icon_hdr header)
{
  printf ("Header:\n");
  printf ("\tflag  : %d\n", header.flag);
  printf ("\tcbsize: %d\n", header.cbsize);
  printf ("\tnext  : %d\n", header.next);
  printf ("\tdisplay_width : %d\n", header.display_width);
  printf ("\tdisplay_height: %d\n", header.display_height);
}

void dump_os2_12_palette (int num_entries, os2_12_rgb_info *p)
{
  return;
}

void dump_os2_12_bitmap_info (os2_12_bitmap_info i)
{
  printf ("Info 12:\n");
  printf ("\tflag          : %d\n", i.flag);
  printf ("\tcbsize        : %d\n", i.cbsize);
  printf ("\thotx, hoty    : %d, %d\n", i.hotx, i.hoty);
  printf ("\tbitmap_offset : %x\n", i.bitmap_offset);
  printf ("\tbitmap_size   : %d\n", i.bitmap_size);
  printf ("\twidth         : %d\n", i.width);
  printf ("\theight        : %d\n", i.height);
  printf ("\tplanes        : %d\n", i.planes);
  printf ("\tbits_per_pixel: %d\n", i.bits_per_pixel);

  dump_os2_12_palette (i.planes*two_to_the(i.bits_per_pixel), i.p);
}

void dump_os2_12_icon_info(os2_12_icon_info i)
{
  printf("OS/2 1.2 Icon:\n");

  dump_os2_icon_header(i.header);

  printf ("OS/2 1.2 andxor_map ");
  dump_os2_12_bitmap_info (i.andxor_map);

  printf ("OS/2 1.2 clr_map ");
  dump_os2_12_bitmap_info (i.clr_map);

}

void dump_os2_20_bitmap_info (os2_20_bitmap_info i)
{
  printf ("Info 20:\n");
  printf ("\tflag		: %d\n", i.flag);
  printf ("\tcbsize		: %d\n", i.cbsize);
  printf ("\thotx		: %d\n", i.hotx);
  printf ("\thoty		: %d\n", i.hoty);
  printf ("\tbitmap_offset	: %x\n", i.bitmap_offset);
  printf ("\tlength		: %d\n", i.length);
  printf ("\twidth		: %d\n", i.width);
  printf ("\theight		: %d\n", i.height);
  printf ("\tplanes		: %d\n", i.planes);
  printf ("\tbits_per_pixel	: %d\n", i.bits_per_pixel);
  printf ("\tcompression	: %d\n", i.compression);
  printf ("\tbitmap_size	: %d\n", i.bitmap_size);
  printf ("\tdevXres		: %d\n", i.devXres);
  printf ("\tdevYres		: %d\n", i.devYres);
  printf ("\tclrUsed		: %d\n", i.clrUsed);
  printf ("\tclrImportant	: %d\n", i.clrImportant);
  printf ("\tunits		: %d\n", i.units);
  printf ("\treserved		: %d\n", i.reserved);
  printf ("\trecording		: %d\n", i.recording);
  printf ("\trendering		: %d\n", i.rendering);
  printf ("\tsize1		: %d\n", i.size1);
  printf ("\tsize2		: %d\n", i.size2);
  printf ("\tcolor_encoding	: %d\n", i.color_encoding);
  printf ("\tapp_identifier	: %d\n", i.app_identifier);
}
*/

os2_arr_hdr read_os2_arr_hdr ()
{
  os2_arr_hdr bm_array_hdr;

  bm_array_hdr.flag = read_shortint();
  bm_array_hdr.cbsize = read_int();
  bm_array_hdr.next = read_int();
  bm_array_hdr.display_width = read_shortint();
  bm_array_hdr.display_height = read_shortint();
  
  return bm_array_hdr;
}

void flip_a_map (bitmap_info *map)
{
  char *buf, *sch, *dch, *RowPtr;
  int place;
  
  buf = (char *) malloc (map->size);

  dch = buf;
  RowPtr = map->map + map->size - map->bytewidth;
  while (RowPtr >= (char *)map->map)
    {
      sch = RowPtr;
      for (place = 0; place < map->bytewidth; place++)
	{
	  *dch = *sch;
	  dch++;
	  sch++;
	}

      RowPtr -= map->bytewidth;
    }

  RowPtr = map->map;
  map->map = buf;
  free (RowPtr);
}

void flip_generic_map () 
{
  flip_a_map (&g->and_map);
/*
  printf ("And map (flipped):\n");
  dump_bitmap(g->and_map);
*/

  flip_a_map (&g->xor_map);
/*
  printf ("Xor map (flipped):\n");
  dump_bitmap(g->xor_map);
*/

  flip_a_map (&g->clr_map);
/*
  printf ("Clr map (flipped):\n");
  dump_bitmap(g->clr_map);
*/
}

void read_generic_bitmap (bitmap_info map)
{
  int row, col, count, padbytes;
  char *ch;

  count = map.bytewidth;
  padbytes = 0;
  while (count % 4 != 0) {
    padbytes++;
    count++;
  }

  ch = map.map;
  for (row = 0; row < map.height; row++) {
    for (col = 0; col < map.bytewidth; col++) {
      *ch = read_char();
      ch++;
    }

    for (count = 0; count < padbytes; count++) {
      read_char();
    }
  }
}

void add_os2_12_generic_bitmap (os2_12_bitmap_info andxor_bitmap12, 
				os2_12_bitmap_info clr_bitmap12)
{
  int count;
  char *ch;
  rgb_info *rgb;
  os2_12_rgb_info *rgb12;
  int padbytes, padcount;

  g->and_map.width = andxor_bitmap12.width;
  g->and_map.height = andxor_bitmap12.height / 2;
  g->and_map.bits_per_pixel = andxor_bitmap12.bits_per_pixel;
  g->and_map.bytewidth = ((andxor_bitmap12.width * andxor_bitmap12.bits_per_pixel)+7) / 8;
  g->and_map.size = g->and_map.bytewidth * g->and_map.height;

  g->and_map.num_colors
    = two_to_the(andxor_bitmap12.bits_per_pixel) * andxor_bitmap12.planes;

  g->and_map.palette = (rgb_info *) malloc(g->and_map.num_colors * 
					     sizeof(rgb_info));
  rgb12 = andxor_bitmap12.p;
  rgb = g->and_map.palette;
  for (count = 0; count < g->and_map.num_colors; count++) {
    rgb->blue = rgb12->blue;
    rgb->red = rgb12->red;
    rgb->green = rgb12->green;
    rgb++;
    rgb12++;
  }

  g->and_map.map = NULL;
  g->xor_map = g->and_map;

  g->clr_map.width = clr_bitmap12.width;
  g->clr_map.height = clr_bitmap12.height;
  g->clr_map.bits_per_pixel = clr_bitmap12.bits_per_pixel;

  g->clr_map.bytewidth = ((clr_bitmap12.width * clr_bitmap12.bits_per_pixel)+7) / 8;
  g->clr_map.size = g->clr_map.bytewidth * g->clr_map.height;

  g->clr_map.num_colors
    = two_to_the(clr_bitmap12.bits_per_pixel) * clr_bitmap12.planes;

  g->clr_map.palette = (rgb_info *) malloc(g->clr_map.num_colors * 
					     sizeof(rgb_info));
  rgb12 = clr_bitmap12.p;
  rgb = g->clr_map.palette;
  for (count = 0; count < g->clr_map.num_colors; count++) {
    rgb->blue = rgb12->blue;
    rgb->red = rgb12->red;
    rgb->green = rgb12->green;
    rgb++;
    rgb12++;
  }

  g->clr_map.map = NULL;
  
  fseek(input, andxor_bitmap12.bitmap_offset, SEEK_SET);
  g->and_map.map = (char *) malloc(g->and_map.size);
  read_generic_bitmap(g->and_map);

  g->xor_map.map = (char *) malloc(g->xor_map.size);
  read_generic_bitmap(g->xor_map);

  fseek(input, clr_bitmap12.bitmap_offset, SEEK_SET);
  g->clr_map.map = (char *) malloc(g->clr_map.size);
  read_generic_bitmap(g->clr_map);

  flip_generic_map();
  g++;
}

void add_os2_20_generic_bitmap (os2_20_bitmap_info andxor_bitmap20, 
				os2_20_bitmap_info clr_bitmap20)
{
  int count;
  char *ch;
  rgb_info *rgb;
  os2_20_rgb_info *rgb20;
  int bmlength;

  g->and_map.width = andxor_bitmap20.width;
  g->and_map.height = andxor_bitmap20.height / 2;
  g->and_map.bits_per_pixel = andxor_bitmap20.bits_per_pixel;
  g->and_map.bytewidth = ((andxor_bitmap20.width * andxor_bitmap20.bits_per_pixel)+7) / 8;
  g->and_map.size = g->and_map.bytewidth * g->and_map.height;
  g->and_map.num_colors
    = two_to_the(andxor_bitmap20.bits_per_pixel) * andxor_bitmap20.planes;

  g->and_map.palette = (rgb_info *) malloc(g->and_map.num_colors * 
					     sizeof(rgb_info));
  rgb20 = andxor_bitmap20.p;
  rgb = g->and_map.palette;
  for (count = 0; count < g->and_map.num_colors; count++) {
    rgb->blue = rgb20->blue;
    rgb->red = rgb20->red;
    rgb->green = rgb20->green;
    rgb++;
    rgb20++;
  }

  g->and_map.map = NULL;
  g->xor_map = g->and_map;

  g->clr_map.width = clr_bitmap20.width;
  g->clr_map.height = clr_bitmap20.height;
  g->clr_map.bits_per_pixel = clr_bitmap20.bits_per_pixel;

  g->clr_map.bytewidth = ((clr_bitmap20.width * clr_bitmap20.bits_per_pixel)+7) / 8;
  g->clr_map.size = g->clr_map.bytewidth * g->clr_map.height;

  g->clr_map.num_colors
    = two_to_the(clr_bitmap20.bits_per_pixel) * clr_bitmap20.planes;

  g->clr_map.palette = (rgb_info *) malloc(g->clr_map.num_colors * 
					     sizeof(rgb_info));
  rgb20 = clr_bitmap20.p;
  rgb = g->clr_map.palette;
  for (count = 0; count < g->clr_map.num_colors; count++) {
    rgb->blue = rgb20->blue;
    rgb->red = rgb20->red;
    rgb->green = rgb20->green;
    rgb++;
    rgb20++;
  }

  g->clr_map.map = NULL;

  fseek(input, andxor_bitmap20.bitmap_offset, SEEK_SET);
  g->and_map.map = (char *) malloc(g->and_map.size);
  read_generic_bitmap (g->and_map);

  g->xor_map.map = (char *) malloc(g->xor_map.size);
  read_generic_bitmap (g->xor_map);

  fseek(input, clr_bitmap20.bitmap_offset, SEEK_SET);
  g->clr_map.map = (char *) malloc(g->clr_map.size);
  read_generic_bitmap (g->clr_map);

  flip_generic_map();
  g++;
}

void read_os2_icon_file () 
{
  os2_arr_hdr bm_array_hdr;
  os2_file_hdr bm_file_hdr;
  os2_12_bitmap_info andxor_bitmap12, clr_bitmap12;
  os2_20_bitmap_info andxor_bitmap20, clr_bitmap20;
  int count, cur_pos;
  char *ch;
  USHORT flag;

  generic_count = 0;
  generic = (generic_info *) malloc(MAX_ICONS * sizeof(generic_info));
  g = generic;

/* Does not handle arrays of OS/2 2.0 icons yet!!! */

  fseek(input, 0, SEEK_SET);
  flag = read_shortint();

  fseek(input, 0, SEEK_SET);
  if (flag == 0x4142) {
    bm_array_hdr = read_os2_arr_hdr();

    cur_pos = ftell(input);
    andxor_bitmap12 = read_os2_12_bitmap_info();

    if (andxor_bitmap12.cbsize == 26) {
      clr_bitmap12 = read_os2_12_bitmap_info();
      generic_count++;
      add_os2_12_generic_bitmap(andxor_bitmap12, clr_bitmap12);
    } else {
      fseek(input, cur_pos, SEEK_SET);
      andxor_bitmap20 = read_os2_20_bitmap_info ();
      clr_bitmap20 = read_os2_20_bitmap_info();
      generic_count++;
      add_os2_20_generic_bitmap(andxor_bitmap20, clr_bitmap20);
    }

    while (bm_array_hdr.next) {
      fseek(input, bm_array_hdr.next, SEEK_SET);
      bm_array_hdr = read_os2_arr_hdr();
      cur_pos = ftell(input);
      andxor_bitmap12 = read_os2_12_bitmap_info();
      if (andxor_bitmap12.flag = 0x4943) {
	if (generic_count+1 >= MAX_ICONS) {
	  fprintf(stderr,"Compile time constant exceeded, too many icons.\n");
	  return;
	}

	if (andxor_bitmap12.cbsize == 26) {
	  clr_bitmap12 = read_os2_12_bitmap_info();
	  generic_count++;
	  add_os2_12_generic_bitmap(andxor_bitmap12, clr_bitmap12);
	} else {
	  fseek(input, cur_pos, SEEK_SET);
	  andxor_bitmap20 = read_os2_20_bitmap_info ();
	  clr_bitmap20 = read_os2_20_bitmap_info();
	  generic_count++;
	  add_os2_20_generic_bitmap(andxor_bitmap20, clr_bitmap20);
	}
      }
    }
  } else {
    andxor_bitmap12 = read_os2_12_bitmap_info ();

    if (andxor_bitmap12.cbsize == 26) {
      clr_bitmap12 = read_os2_12_bitmap_info();
      generic_count++;
      add_os2_12_generic_bitmap(andxor_bitmap12, clr_bitmap12);
    } else {
      fseek(input, 0, SEEK_SET);
      andxor_bitmap20 = read_os2_20_bitmap_info ();
      clr_bitmap20 = read_os2_20_bitmap_info();
      generic_count++;
      add_os2_20_generic_bitmap(andxor_bitmap20, clr_bitmap20);
    }
  }
}

