/* This file is xbmread.c (part of XIcon)
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
#include "xbmread.h"
#include "iconio.h"
#include "icondata.h"
#include "iconvars.h"
#include "generic.h"
#include "bitmap.h"
#include "palette.h"
#include "xbmdata.h"

int XBMrevbits;
int XBMforg;
int XBMback;

extern int ReadXBM(FILE *, ULONG *, ULONG *, BYTE **);

BYTE revbits(BYTE b)
{
  BYTE inmask, outmask;
  BYTE outchar;

  outchar = 0;
  for (inmask = 0x80, outmask = 0x01;
       inmask > 0;
       inmask >>= 1, outmask <<= 1)
    if ((b & inmask) == inmask)
      outchar |= outmask;

  return outchar;
}

void read_xbm_file()
{
  BYTE *XBMbits_map = NULL, *XBMmask_map = NULL, *XBMch;
  ULONG XBMwidth, XBMheight, XBM_mask_width, XBM_mask_height;
  bitmap_info XBMbits, XBMmask;
  generic_info *g;
  int row, col, count, rc;
  BYTE ch;
  int mBit1, mBit2, rXBMforg, rXBMback, bitColor;

  generic_count = 1;
  generic = (generic_info *) malloc(sizeof(generic_info));

  init_generic(0);
  g = generic;

  fseek(input, 0, SEEK_SET);
  rc = ReadXBM(input, &XBMwidth, &XBMheight, &XBMbits_map);

  if (rc != 0)
    {
      fprintf(stderr, "Error reading XBM file\n");
      exit(1);
    }

  XBMbits.width = XBMwidth;
  XBMbits.height = XBMheight;
  XBMbits.bits_per_pixel = 1;
  XBMbits.bytewidth = (XBMwidth+7) / 8;
  XBMbits.size = XBMbits.bytewidth * XBMheight;
  XBMbits.num_colors = 2;
  XBMbits.palette = bw_palette;
  XBMbits.map = XBMbits_map;

  if (XBMrevbits)
    {
      XBMch = XBMbits.map;
      for (count = 0; count < XBMbits.size; count++, XBMch++)
	*XBMch = revbits(*XBMch);
    }

  g->clr_map = init_empty_map(XBMwidth, XBMheight, 4);
  g->clr_map.palette = pc_palette;
  g->and_map = init_empty_map(XBMwidth, XBMheight, 1);
  g->and_map.palette = bw_palette;
  g->xor_map = init_empty_map(XBMwidth, XBMheight, 1);
  g->xor_map.palette = bw_palette;

  if (maskfile)
    {
      fseek(maskfile, 0, SEEK_SET);
      rc = ReadXBM(maskfile, &XBM_mask_width, &XBM_mask_height, &XBMmask_map);

      if (rc != 0)
	{
	  fprintf(stderr, "Error reading XBM mask file, mask ignored.\n");
	  XBMmask_map = NULL;
	}
      else
	if (XBM_mask_width != XBMwidth || XBM_mask_height != XBMheight)
	  {
	    fprintf(stderr, "XBM image and mask are different sizes, mask ignored.\n");
	    XBMmask_map = NULL;
	  }
    }

  XBMmask = XBMbits;
  if (XBMmask_map)
    {
      XBMmask.map = XBMmask_map;
      /* We want the bits the other way around, 1 for transparent... */
      XBMch = XBMmask.map;
      for (count = 0; count < XBMmask.size; count++, XBMch++)
	{
	  if (XBMrevbits)
	    *XBMch = revbits(*XBMch);
	  *XBMch = (*XBMch ^ 0xFF);
	}
    }
  else
    { 
      XBMmask = init_empty_map (XBMwidth, XBMheight, 1);
    }
      
  /* Now "XBMbits" holds the image, 1=on, 0=off.
         "XBMmask" holds the mask, 1=transparent, 0=setbit.
   */

  if (0 && dump_input)
    {
      printf("XBMbits:\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    if (get_bit(XBMbits, row, col))
	      printf("1");
	    else
	      printf("0");
	  printf("\n");
	}

      printf("XBMmask:\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    if (get_bit(XBMmask, row, col))
	      printf("1");
	    else
	      printf("0");
	  printf("\n");
	}
    }

  if (XBMforg < 0)
    rXBMforg = 0;
  else
    rXBMforg = XBMforg;

  if (XBMback < 0)
    rXBMback = 0;
  else
    rXBMback = XBMback;

  mBit1 = 2;
  mBit2 = 2;
  if (XBMback == XBM_TRANSPARENT)
    mBit1 = 0;
  if (XBMforg == XBM_TRANSPARENT)
    mBit2 = 1;

  for (row = 1; row <= XBMheight; row++)
    for (col = 1; col <= XBMwidth; col++)
      {
	bitColor = get_bit (XBMbits, row, col);
	if ((bitColor == mBit1  || bitColor == mBit2)
	    && get_bit (XBMmask, row, col) == 0)
	  set_bit(XBMmask, row, col, 1);
      }

  mBit1 = 2;
  mBit2 = 2;
  if (XBMback == XBM_REVERSEVID)
    mBit1 = 0;
  if (XBMforg == XBM_REVERSEVID)
    mBit2 = 1;

  for (row = 1; row <= XBMheight; row++)
    for (col = 1; col <= XBMwidth; col++)
      {
	bitColor = get_bit (XBMbits, row, col);
	if ((bitColor == mBit1  || bitColor == mBit2)
	    && get_bit (XBMmask, row, col) == 0)
	  {
	    set_bit(XBMmask, row, col, 1);
	    set_bit(g->xor_map, row, col, 1);
	  }
      }

  g->and_map.map = XBMmask.map;
  
  for (row = 1; row <= XBMheight; row++)
    for (col = 1; col <= XBMwidth; col++)
      {
	if (get_bit(XBMmask, row, col) == 0)
	  if (get_bit(XBMbits, row, col))
	    set_bit(generic[0].clr_map, row, col, rXBMforg);
	  else
	    set_bit(generic[0].clr_map, row, col, rXBMback);
	else
	  set_bit(generic[0].clr_map, row, col, 0xFF);
      }

#if 0
  /* One combination is special.  If the desired color is 0, the xor
     mask should be 1, and the and and clr masks should be 0. */

  mBit1 = 2;
  mBit2 = 2;
  if (XBMback == 0)
    mBit1 = 0;
  if (XBMforg == 0)
    mBit2 = 1;
  
  for (row = 1; row <= XBMheight; row++)
    for (col = 1; col <= XBMwidth; col++)
      {
	bitColor = get_bit(XBMbits, row, col);
	if ((bitColor == mBit1 || bitColor == mBit2)
	    && get_bit (XBMmask, row, col) == 0)
	  {
	    set_bit(generic[0].and_map, row, col, 0);
	    set_bit(generic[0].xor_map, row, col, 1);
	    set_bit(generic[0].clr_map, row, col, 0);
	  }
      }
#endif /* 0 */

  if (dump_input)
    {
      bitmap_info foo;

      printf("CLR mask:\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    printf("%x", get_bit(generic[0].clr_map, row, col));
	  printf("\n");
	}

      printf("AND mask:\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    if (get_bit(generic[0].and_map, row, col))
	      printf("1");
	    else
	      printf("0");
	  printf("\n");
	}

      printf("XOR mask:\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    if (get_bit(generic[0].xor_map, row, col))
	      printf("1");
	    else
	      printf("0");
	  printf("\n");
	}

      foo = init_empty_map (XBMwidth, XBMheight, 4);
      XBMch = foo.map;
      for (count = 0; count < generic[0].clr_map.size; count++, XBMch++)
	*XBMch = 0xBB;

/*
      printf("RESULT (raw):\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    printf("%x", get_bit(foo, row, col));
	  printf("\n");
	}
*/

      for (row = 1; row <= XBMheight; row++)
	for (col = 1; col <= XBMwidth; col++)
	  {
	    if (!get_bit(generic[0].and_map, row, col))
	      set_bit(foo, row, col, 0);
	  }

/*
      printf("RESULT (after AND):\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    printf("%x", get_bit(foo, row, col));
	  printf("\n");
	}
*/

      for (row = 1; row <= XBMheight; row++)
	for (col = 1; col <= XBMwidth; col++)
	  {
	    bitColor = get_bit(foo, row, col);
	    if (get_bit (generic[0].xor_map, row, col))
	      set_bit(foo, row, col, (bitColor ^ 0xF));
	  }

/*
      printf("RESULT (after XOR):\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    printf("%x", get_bit(foo, row, col));
	  printf("\n");
	}
*/

      for (row = 1; row <= XBMheight; row++)
	for (col = 1; col <= XBMwidth; col++)
	  {
	    bitColor = get_bit(foo, row, col);
	    set_bit (foo, row, col, bitColor ^ 
		     get_bit(generic[0].clr_map, row, col));
	    
	  }

      printf("RESULT (on a field of 0xbb):\n");
      for (row = 1; row <= XBMheight; row++)
	{
	  for (col = 1; col <= XBMwidth; col++)
	    printf("%x", get_bit(foo, row, col));
	  printf("\n");
	}
    }
}
