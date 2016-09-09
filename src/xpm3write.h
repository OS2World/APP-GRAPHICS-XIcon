/* This file is xmp3write.h (part of XIcon)
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

#ifndef XPM3WRITE_H
#define XPM3WRITE_H

#include "icondata.h"

typedef struct _rgb_color {
  BYTE blue;
  BYTE green;
  BYTE red;
  char *name;
} rgb_color;

void write_xpm3_file(int count);
rgb_color *read_rgb_txt(char *fn);

#endif /* not XPM3WRITE_H */




