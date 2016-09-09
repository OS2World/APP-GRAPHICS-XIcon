/* This file is iconio.h (part of XIcon)
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

#ifndef ICONIO_H
#define ICONIO_H

#ifndef SEEK_SET
#define SEEK_SET 0
#endif

extern FILE *input;
extern FILE *output;
extern FILE *maskfile;
extern int  rc;

int read_int();
short int read_shortint();
unsigned char read_char();
int read_macint();
int read_macint3();
short int read_macshortint();
unsigned char read_macchar();
void write_int(unsigned int);
void write_shortint(unsigned short int);
void write_char(unsigned char);

#endif /* not ICONIO_H */
