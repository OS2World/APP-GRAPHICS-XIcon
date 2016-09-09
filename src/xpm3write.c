/* This file is xpm3write.c (part of XIcon)
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
#include <ctype.h>
#include "icondata.h"
#include "iconvars.h"
#include "iconio.h"
#include "xpm3write.h"

rgb_color *color_db = NULL;

void write_xpm_map (bitmap_info map)
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
	{
	  char hi, lo;
	  hi = (*ch) >> 4;
	  lo = (*ch) & 0x0F;
	  printf ("%c%c", hi+'a', lo+'a');
	}
      printf ("\n");
/*
      for (count = 0; count < padbytes; count++)
	write_char(0);
*/      
      rowptr -= map.bytewidth;
    }
}

void dump_bitmap(bitmap_info map) 
{
  int row,col,foo;

  printf ("Dumping %dx%d bitmap (%d bits/pixel, %d bytes)\n",
	  map.width, map.height, map.bits_per_pixel, map.size);

  for (row = 1; row <= map.height; row++) 
    {
      for (col = 1; col <= map.width; col++)
	{
	  foo = get_bit(map,row,col);
	  printf("%c", foo+'a');
	}
      printf ("\n");
    }
}

char *find_color(rgb_info rgb)
{
  int deltared, deltagreen, deltablue;
  int smallred, smallgreen, smallblue;
  rgb_color *c = color_db;
  char *name = NULL;

  smallred   = 1024;
  smallgreen = 1024;
  smallblue  = 1024;

  for (c = color_db; c->name; c++) {
    deltared   = abs(c->red - rgb.red);
    deltagreen = abs(c->green - rgb.green);
    deltablue  = abs(c->blue - rgb.blue);

    if ((deltared <= smallred)
	&& (deltagreen <= smallgreen)
	&& (deltablue <= smallblue))
      {
	smallred = deltared;
	smallgreen = deltagreen;
	smallblue = deltablue;
	name = c->name;
      }
	  
    if (c->red == rgb.red
	&& c->green == rgb.green
	&& c->blue == rgb.blue) {
      return c->name;
    }
  }

  return name;
}

void write_xpm3_file(int count)
{
  int rgbcount;
  int row,col,foo;
  rgb_info *rgb;

  if (color_db == NULL) {
    fprintf (stderr, "Reading colors database %s...\n", color_db_file);
    color_db = read_rgb_txt (color_db_file);
    if (!color_db) {
      fprintf (stderr, "Cannot read colors database: %s\n", color_db_file);
      exit(1);
    }
  }

#ifdef DUMP_BITMAPS
  printf("xor_map:\n");
  dump_bitmap(generic[count].xor_map);

  printf("and_map:\n");
  dump_bitmap(generic[count].and_map);

  printf("clr_map:\n");
  dump_bitmap(generic[count].clr_map);
#endif /* DUMP_BITMAPS */

  fprintf (output, "/* XPM */\n");
  fprintf (output, "static char * image[] = {\n");
  fprintf (output, "\"%d %d %d 1\",\n",
	   generic[count].clr_map.width,
	   generic[count].clr_map.height,
	   generic[count].clr_map.num_colors+1);
  
  fprintf (output, "\"  c none\",\n");
  rgb = generic[count].clr_map.palette;
  for (rgbcount = 0; rgbcount < generic[count].clr_map.num_colors; rgbcount++)
    {
      fprintf (output, "\"%c c %s\",\n", rgbcount+'a', find_color(*rgb));
      rgb++;
    }

  for (row = 1; row <= generic[count].clr_map.height; row++) 
    {
      if (row > 1) fprintf (output, ",\n");
      fprintf (output, "\"");
      for (col = 1; col <= generic[count].clr_map.width; col++)
	{
	  if ((get_bit(generic[count].xor_map,row,col)
	      != get_bit(generic[count].and_map,row,col))
	       || (get_bit(generic[count].xor_map,row,col)
		   && get_bit(generic[count].and_map,row,col)))
	    foo = 32;
	  else
	    foo = get_bit(generic[count].clr_map,row,col) + 'a';

	  fprintf(output, "%c", foo);
	}
      fprintf (output, "\"");
    }
  fprintf (output, "};\n");
}

rgb_color *read_rgb_txt(char *fn)
{
  rgb_color *colors, *temp_colors;
  int i, cur_color, num_colors;
  FILE *text;
  char line[1024], *name;
  char *red, *green, *blue;
  char *ch;

  text = fopen(fn, "r");
  if (text == (FILE *) NULL) {
    return NULL;
  }

  cur_color = 0;
  num_colors = 500;
  colors = (rgb_color *) malloc(num_colors * sizeof(rgb_color));

  while (fgets(line, 1023, text)) 
    {
      ch = line;

      while (*ch && isspace(*ch)) ch++;
      red = ch;
      while (*ch && isdigit(*ch)) ch++;
      *ch = 0;
      ch++;

      while (*ch && isspace(*ch)) ch++;
      green = ch;
      while (*ch && isdigit(*ch)) ch++;
      *ch = 0;
      ch++;

      while (*ch && isspace(*ch)) ch++;
      blue = ch;
      while (*ch && isdigit(*ch)) ch++;
      *ch = 0;
      ch++;

      while (*ch && isspace(*ch)) ch++;
      name = (char *) malloc(strlen(ch) + 1);
      strcpy(name, ch);

      name[strlen(name)-1] = 0; /* get rid of the newline */

      sscanf(red, "%d", &i);   colors[cur_color].red   = i;
      sscanf(green, "%d", &i); colors[cur_color].green = i;
      sscanf(blue, "%d", &i);  colors[cur_color].blue   = i;
      colors[cur_color].name = name;

      cur_color++;
      if (cur_color >= num_colors) { 
	temp_colors = colors;
	colors = (rgb_color *) malloc(num_colors * 2 * sizeof(rgb_color));
	for (i = 0; i < num_colors; i++) {
	  colors[i] = temp_colors[i];
	}
	num_colors *= 2;
	free (temp_colors);
      }
    }
  fclose(text);

  colors[cur_color].name = NULL;

  return colors;
}
