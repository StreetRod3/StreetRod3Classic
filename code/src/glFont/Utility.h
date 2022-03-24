/*
   Copyright (C) 2002 Nate Miller nathanm@uci.edu

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

   See gpl.txt for more information regarding the GNU General Public License.
*/
/*
   WORKLOG
      (09/28/01)
         - tried to make it not suck as bad
      (10/31/00)
         - removed duplicate code
      (10/23/00)
         - added clamp, min and max.  they assume the object has the proper 
           operators avaliable
      (10/17/00)
         - LineReadUntil() now returns 0 if it doesn't hit the 'end' char
      (10/13/00) 
         - added another version of LineReadUntil()
      (10/07/00)
         - added IsAlpha() and IsComment()
         - added LineReadChars(), only reads characters
   TODO  
      * write more standard library replacement code
*/
#ifndef __UTILITYH__
#define __UTILITYH__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Types.h"

namespace platformUtility
{

struct Utility
{
   struct EqualString
   {
      bool operator()(const char *s1, const char *s2) const
      {
         return strcmp(s1, s2) == 0;
      }
   };

   // assmues objects have proper operators
   template <class Type> static inline Type Clamp(Type a, Type min, Type max)
    {return a < min ? min : a > max ? max : a;}
   template <class Type> static inline Type Max(Type a, Type b)
    {return a > b ? a : b;}
   template <class Type> static inline Type Min(Type a, Type b)   
    {return a < b ? a : b;}

   static inline bool IsAlpha(const char &c) 
    {return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));}
   static inline bool IsNum(const char &c)
    {return (c >= '0' && c <= '9');}
   static inline bool IsComment(char *line) 
    {return (line[0] == '/' && line[1] == '/');}
   static inline bool IsWhite(char c)
    {return (c == ' ' || c == '\t' || c == '\r' || c == '\n');}

   // returns the file size
   static long FileGetSize(FILE *stream);
   static long FileNameGetSize(const char *name);

   // returns the contents of a file
   static byte *FileReadAll(FILE *in, long *size = 0);
   static byte *FileReadAll(const char *name, long *size = 0);

   static void LineRead(char **pos, char *dest, int max = -1);
   static char *LineRead(char **pos);

   static char *LineReadUntil(char **pos, char end);
   static bool LineReadUntil(char **pos, char end, char *dest, int size);

   // eats a line or white space
   static void LineEat(char **pos);
   static void LineEatWhite(char **pos);
   static void LineEatUntil(char **pos, char end);

   static int LineReadInt(char **pos);
};

}

#endif