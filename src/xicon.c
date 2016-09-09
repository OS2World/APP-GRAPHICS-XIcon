/* This file is xicon.c (part of XIcon)
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
 * Norman Walsh can be reached electronically at <norm@ora.com>
 * or via paper mail to:
 *  
 *  Norman Walsh
 *  32 Spencer Rd, Apt 32M
 *  Boxborough, MA 01719
 ************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef __EMX__
#include <getopt.h>
#endif /* __EMX__ */
#include "macread.h"
#include "winread.h"
#include "bitmap.h"
#include "icondata.h"
#include "os2read.h"
#include "os2write.h"
#include "xbmread.h"
#include "palette.h"
#include "xbmdata.h"
#include "winwrite.h"
#include "xpm3write.h"

/* Optional arguments */

#ifndef __EMX__
extern char *optarg;
extern int optind, opterr;
#endif /* not __EMX__ */

int generic_count = 0;
generic_info *generic = NULL;
int dump_input = 0;

#define FORMAT_OS212 0
#define FORMAT_OS220 1
#define FORMAT_WIN   2
#define FORMAT_XPM3  3

int write_format = FORMAT_OS212;
int write_all    = 0;

rgb_info *pc_palette = NULL;
rgb_info *mac_palette = NULL;
rgb_info *bw_palette = NULL;

char *color_db_file = "/usr/local/lib/X11/rgb.txt";

extern int cvtICNp;
extern int cvticsp;
extern int cvticl4;
extern int cvtics4;
extern int MacPalette;

extern int XBMrevbits;
extern int XBMforg;
extern int XBMback;

FILE *input;
FILE *output;
FILE *maskfile;
int rc;

void error (char *msg)
{
  printf("%s\n", msg);
  exit(1);
}

void init_palettes()
{
  pc_palette = (rgb_info *) malloc (sizeof (rgb_info) * 16);
  mac_palette = (rgb_info *) malloc (sizeof (rgb_info) * 16);
  bw_palette = (rgb_info *) malloc (sizeof (rgb_info) * 2);

  pc_palette[ 0] = palette_color (255, 255, 255);
  pc_palette[ 1] = palette_color (128,   0,   0);
  pc_palette[ 2] = palette_color (  0, 128,   0);
  pc_palette[ 3] = palette_color (128, 128,   0);
  pc_palette[ 4] = palette_color (  0,   0, 128);
  pc_palette[ 5] = palette_color (128,   0, 128);
  pc_palette[ 6] = palette_color (  0, 128, 128);
  pc_palette[ 7] = palette_color (128, 128, 128);
  pc_palette[ 8] = palette_color (204, 204, 204);
  pc_palette[ 9] = palette_color (255,   0,   0);
  pc_palette[10] = palette_color (  0, 255,   0);
  pc_palette[11] = palette_color (255, 255,   0);
  pc_palette[12] = palette_color (  0,   0, 255);
  pc_palette[13] = palette_color (255,   0, 255);
  pc_palette[14] = palette_color (  0, 255, 255);
  pc_palette[15] = palette_color (  0,   0,   0);
  
  mac_palette[ 0] = palette_color (255, 255, 255);
  mac_palette[ 1] = palette_color (  0, 255, 255);
  mac_palette[ 2] = palette_color ( 16, 128, 255);
  mac_palette[ 3] = palette_color (  0,   0, 255);
  mac_palette[ 4] = palette_color (128,   0, 255);
  mac_palette[ 5] = palette_color (177,  17,  20);
  mac_palette[ 6] = palette_color (212,   0,   0);
  mac_palette[ 7] = palette_color (194, 194,   0);
  mac_palette[ 8] = palette_color (  0, 255,   0);
  mac_palette[ 9] = palette_color (  0, 162,   0);
  mac_palette[10] = palette_color ( 75,  97, 193);
  mac_palette[11] = palette_color ( 25, 159, 192);
  mac_palette[12] = palette_color (224, 224, 224);
  mac_palette[13] = palette_color (161, 161, 161);
  mac_palette[14] = palette_color ( 82,  82,  82);
  mac_palette[15] = palette_color (  0,   0,   0);
  
  bw_palette[0] = palette_color(  0,   0,    0);
  bw_palette[1] = palette_color(255, 255, 255);
}

int main (int argc, char **argv)
{
  int count;
  char *inputfn, *outputfn, *maskfn;
  char *arg, c;

  inputfn = NULL;
  outputfn = NULL;
  maskfn = NULL;
  maskfile = NULL;

  cvtICNp = 1;
  cvticsp = 1;
  cvticl4 = 1;
  cvtics4 = 1;
  MacPalette = 0;
  XBMrevbits = 1;
  XBMforg = 15;
  XBMback = XBM_TRANSPARENT;

  init_palettes();

  fprintf(stderr, "XIcon  v1.0  Copyright (c) 1993,94 by Norman Walsh\n");
  if (argc < 2)
    {
      fprintf(stderr, "Usage: xicon <options> <inputfile> <outputfile>\n");
      fprintf(stderr, "Where:\n");
      fprintf(stderr, "  <inputfile>  is an icon file in Mac format (raw resource fork), Windows \n");
      fprintf(stderr, "               format, or X11 XBM format.\n");
      fprintf(stderr, "  <outputfile> is the name of the OS/2 icon file that will be created.\n");
      fprintf(stderr, "  <options>    -2  write an OS/2 v2.0 icon file rather than a v1.2 icon file.\n");
      fprintf(stderr, "               -w  write a Windows icon file rather than an OS/2 icon file.\n");
      fprintf(stderr, "               -x  write an XPM3 icon file rather than an OS/2 icon file.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "               For XPM files only:\n");
      fprintf(stderr, "                 -a    write all the icons rather than just the first.\n");
      fprintf(stderr, "                 -R f  Use 'f' instead of standard rgb.txt file.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "               For XBM files only:\n");
      fprintf(stderr, "                 -f C  set foreground color to 'C'.  C=0..15 or 't' for \n");
      fprintf(stderr, "                       transparent, or 'r' for reverse of background.\n");
      fprintf(stderr, "                 -b C  set background color to 'C'\n");
      fprintf(stderr, "                 -r    don't reverse the bit order of the bytes in the file.\n");
      fprintf(stderr, "                 -m f  Set mask file to 'f'.  The mask file is ANDed with the\n");
      fprintf(stderr, "                       background and the image.  Set the bits in the mask area.\n");
      fprintf(stderr, "\n");
      fprintf(stderr, "               For Mac files only:\n");
      fprintf(stderr, "                 -p    Use Mac palette rather than OS/2 palette.\n");
      fprintf(stderr, "                 -I    Don't include ICN# resources (32x32x1 images)\n");
      fprintf(stderr, "                 -#    Don't include ics# resources (16x16x1 images)\n");
      fprintf(stderr, "                 -l    Don't include icl4 resources (32x32x4 images)\n");
      fprintf(stderr, "                 -s    Don't include ics4 resources (16x16x4 images)\n");
      fprintf(stderr, "\n");
    }

  opterr = 0;
#ifdef __EMX__
  optswchar = "-/";
  optmode = GETOPT_ANY;
#endif /* __EMX__ */
    
  while ((c = getopt(argc, argv, "f:b:m:R:dpwx2rI#lsa")) != EOF)
    {
      switch (c)
	{
	case 'f':
	  if (strcmp(optarg, "t") == 0 || strcmp(optarg, "T") == 0)
	    XBMforg = XBM_TRANSPARENT;
	  else
	    if (strcmp(optarg, "r") == 0 || strcmp(optarg, "R") == 0)
	      XBMforg = XBM_REVERSEVID;
	    else
	      {
		XBMforg = (atoi(optarg) & 0x0F);
	      }
	  break;

	case 'b':
	  if (strcmp(optarg, "t") == 0 || strcmp(optarg, "T") == 0)
	    XBMback = XBM_TRANSPARENT;
	  else
	    if (strcmp(optarg, "r") == 0 || strcmp(optarg, "R") == 0)
	      XBMback = XBM_REVERSEVID;
	    else
	      {
		XBMback = (atoi(optarg) & 0x0F);
	      }
	  break;

	case 'm':
	  maskfn = optarg;
	  if (access (maskfn, R_OK))
	    {
	      fprintf(stderr, "Cannot read mask file, mask ignored.\n");
	      maskfn = NULL;
	    }
	  break;

	case 'R':
	  color_db_file = optarg;
	  if (access (color_db_file, R_OK))
	    {
	      fprintf(stderr, "Cannot read color database file...\n");
	      exit(1);
	    }
	  break;

	case 'd': /*  dump */
	  dump_input = 1;
	  break;

	case 'p': /*  Mac Palette */
	  MacPalette = 1;
	  break;

	case '2': /*  Output OS/2 v2.0 icon format */
	  write_format = FORMAT_OS220;
	  break;

	case 'w': /* Output Windows format */
	  write_format = FORMAT_WIN;
	  break;

	case 'x': /* Output XPM3 format */
	  write_format = FORMAT_XPM3;
	  break;

	case 'r': /*  Don't reverse bits in XBM file */
	  XBMrevbits = 0;
	  break;
	  
	case 'I': /*  Don't include ICN# resources */
	  cvtICNp = 0;
	  break;
	  
	case '#': /*  Don't include ics# resources */
	  cvticsp = 0;
	  break;

	case 'l': /*  Don't include icl4 resources */
	  cvticl4 = 0;
	  break;

	case 's': /*  Don't include ics4 resources */
	  cvtics4 = 0;
	  break;

	case 'a': /*  Write all the icons */
	  write_all = 1;
	  break;

	default:
	  fprintf(stderr, "Illegal option ignored.\n");
	  break;
	}
    }

  for ( ; optind < argc; optind++) 
    {
      if (inputfn == NULL)
	inputfn = argv[optind];
      else
	if (outputfn == NULL)
	  outputfn = argv[optind];
	else
	  error("Too many filenames.");
    }

  if (inputfn == NULL || outputfn == NULL)
    error("You must specify input and output filenames.");

  if (access (inputfn, R_OK))
    error("Cannot read input file.");

  input = fopen(inputfn, "rb");
  
  if (write_format != FORMAT_XPM3 || ! write_all) {
    output = fopen(outputfn, "wb");

    if (output == NULL)
      error("Cannot create output file.");
  }

  if (maskfn)
    maskfile = fopen(maskfn, "rb");

  if (maskfile != NULL)
    fprintf(stderr, "Using mask from %s.\n", maskfn);

  switch (QueryFileType())
    {
    case MacIcon:
      fprintf(stderr, "Reading Macintosh icon file from %s.\n", inputfn);
      read_mac_file();
      break;
    case OS2Icon12:
    case OS2Icon20:
      fprintf(stderr, "Reading OS/2 icon file from %s.\n", inputfn);
      read_os2_icon_file();
      break;
    case WinIcon:
      fprintf(stderr, "Reading Windows icon file from %s.\n", inputfn);
      read_win_file();
      break;
    case XBMicon:
      fprintf(stderr, "Reading X11 XBM icon file from %s.\n", inputfn);
      read_xbm_file();
      break;
    default:
      error("Can't read that type of file.");
    }

  fprintf(stderr, "%d icons in file.\n", generic_count);

  switch (write_format) {
  case FORMAT_OS220:
    write_os2_file20();
    fprintf(stderr,"Wrote OS/2 version 2.0 icon file on %s.\n", outputfn);
    break;
  case FORMAT_WIN:
    write_win_file();
    fprintf(stderr,"Wrote Windows icon file on %s.\n", outputfn);
    break;
  case FORMAT_XPM3:
    if (write_all) {
      char fn[1024];
      int count = 1;

      for (count = 0; count < generic_count; count++) {
	sprintf(fn, "%s%d.xpm", outputfn, count+1);
	output = fopen(fn, "wb");
	write_xpm3_file(count);
	fclose(output);
	fprintf(stderr,"Wrote XPM3 icon file on %s.\n", fn);
      }
    } else {    
      write_xpm3_file(0);
      fprintf(stderr,"Wrote XPM3 icon file on %s.\n", outputfn);
    }

    break;
  case FORMAT_OS212:
  default:
    write_os2_file12();
    fprintf(stderr,"Wrote OS/2 version 1.2 icon file on %s.\n", outputfn);
    break;
  }

  fclose(input);

  if (write_format != FORMAT_XPM3 || ! write_all)
    fclose(output);

  if (maskfile)
    fclose(maskfile);
}



