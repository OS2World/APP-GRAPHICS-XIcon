/* This file is bitmap.c (part of XIcon)
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
 *
 ************************************************************************/

#include "icondata.h"

BYTE get_bit(bitmap_info m, int row, int col)
{
  BYTE *ch, b, mask;
  int offset, firstbit, firstbyte;
  int pixels_per_byte, bitnumber;
  int count;

  firstbit = ((col - 1) * m.bits_per_pixel) + 1;
  firstbyte = firstbit / 8;
  if ((firstbit % 8 != 0) || (firstbit == 0))
    firstbyte++;

  offset = m.bytewidth * (row - 1) + (firstbyte - 1);
  ch = m.map + offset;

  pixels_per_byte = 8 / m.bits_per_pixel;

  bitnumber = col % pixels_per_byte;
  if (bitnumber == 0)
    bitnumber = pixels_per_byte;

  /* Now move the bits for the pixel to the lowest bits in the byte ... */

  b = *ch >> ((pixels_per_byte - bitnumber) * m.bits_per_pixel);

  /* And mask out the other bits ... */
  mask = 0x01;
  for (count = 1; count < m.bits_per_pixel; count++)
    {
      mask <<= 1;
      mask |= 0x01;
    }

  return (b & mask);
}

void set_bit(bitmap_info m, int row, int col, BYTE b)
{
  BYTE *ch, mask;
  int offset, firstbit, firstbyte;
  int pixels_per_byte, bitnumber;
  int count;

  firstbit = ((col - 1) * m.bits_per_pixel) + 1;
  firstbyte = firstbit / 8;
  if ((firstbit % 8 != 0) || (firstbit == 0))
    firstbyte++;

  offset = m.bytewidth * (row - 1) + (firstbyte - 1);
  ch = m.map + offset;

  pixels_per_byte = 8 / m.bits_per_pixel;

  bitnumber = col % pixels_per_byte;
  if (bitnumber == 0)
    bitnumber = pixels_per_byte;

  /* Now move the bits for the pixel over to the correct column... */

  b = b << ((pixels_per_byte - bitnumber) * m.bits_per_pixel);

  /* And construct a mask for these bits ... */
  mask = 0x01;
  for (count = 1; count < m.bits_per_pixel; count++)
    {
      mask <<= 1;
      mask |= 0x01;
    }

  mask = mask << ((pixels_per_byte - bitnumber) * m.bits_per_pixel);

  b = (b & mask);
  mask = (mask ^ 0xFF);

  *ch = (*ch & mask);
  *ch = (*ch | b);
}

void printmap(bitmap_info m)
{
  char *hexdigits = "0123456789ABCDEF";
  int row, col;
  BYTE b;

  printf("\n");

  if (m.width == 0)
    return;

  for (row = 1; row <= m.height; row++)
    {
      for (col = 1; col <= m.width; col++)
	{
	  b = get_bit(m, row, col);
	  printf("%c", hexdigits[b]);
	}
      printf("\n");
    }
}
