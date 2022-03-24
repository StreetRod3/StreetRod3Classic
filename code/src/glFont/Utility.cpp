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

#include "Utility.h"

namespace platformUtility {
/*
=============
FileGetSize
=============
   Returns the size of the file that is assiated with the passed stream.
*/
long Utility::FileGetSize(FILE *stream)
{
   long curpos, length;

   curpos = ftell(stream);
   fseek(stream, 0L, SEEK_END);
   length = ftell(stream);
   fseek(stream, curpos, SEEK_SET);

   return length;
}

/*
=============
FileNameGetSize
=============
   Returns the size of the file that has the passed name.
*/
long Utility::FileNameGetSize(const char *name)
{
   FILE *f = fopen(name, "r");
   
   if (!f)
      return 0;

   long size = FileGetSize(f);

   fclose(f);

   return size;
}

/*
=============
FileReadAll
=============
   Reads the entire contents of the passed file handle into a buffer and
returns it.  If 'dsize' is not 0, the amount of data that was read is stored in
it.  

   Returns an array of bytes on success and 0 on failure.  
*/
byte *Utility::FileReadAll(FILE *in, long *dsize)
{
   byte *b = 0;
   long size = 0;
   
   size = FileGetSize(in);   

   if (!size)
      return 0;
   
   if (!(b = new byte[size + 1]))
      return 0;

   memset(b, '\0', size + 1);

   if (!b)
      return 0;
   
   fread(b, 1, size, in); // check return with size?

   if (dsize)
      *dsize = size;

   return b;
}

/*
=============
FileReadAll
=============
   Same as the FileReadAll above, but this version takes a file name.
*/
byte *Utility::FileReadAll(const char *name, long *dsize)
{
   FILE *in = fopen(name, "r");
   
   if (!in)
      return 0;

   byte *b = FileReadAll(in, dsize);
   
   fclose(in);
   
   return b;
}

/*
=============
LineRead
=============
   Read a line out of a character buffer and place it into the 'dest' array.  
If max is a number greater than 0, the function will only read max - 1 
characters (null is the last character that is added).  This function advances
the 'pos' pointer as it goes along the character buffer.
*/
void Utility::LineRead(char **pos, char *dest, int max)
{
   int num = 0;
   int end = max - 1;
   char *start = *pos;

   while (**pos && **pos != '\n' && **pos != '\r')
   {
      if (max > 0 && num >= end)
         break;
      dest[*pos - start] = *(*pos)++;
      num++;
   }
   dest[*pos - start] = '\0';
}

/*
=============
LineRead
=============
   Reads a line out of a character buffer and returns an array of bytes for the
current line that was read.  This function advances the 'pos' pointer as it
goes along the character buffer.
*/
char *Utility::LineRead(char **pos)
{
   char *start = *pos;
   char *buffer = 0;
   
   while (**pos && **pos != '\n' && **pos != '\r')
      (*pos)++;

   if (!(buffer = new char[(*pos - start) + 1]))
      return 0;

   strncpy(buffer, start, *pos - start);
   buffer[*pos - start] = '\0';

   return buffer;
}

/*
=============
LineReadUntil   
=============
   Reads from the passed character buffer until the 'end' character is found.  
The 'pos' pointer is advanced as the function moves through the array.  A 
buffer containing all characters read is returned.  

   On success an array of chars is returned and on failure 0 is returned.
*/
char *Utility::LineReadUntil(char **pos, char end)
{
   char *start = *pos;
   char *buffer = 0;

   while (**pos && **pos != end)
      (*pos)++;   

   if (**pos != end)
      return 0;

   if (!(buffer = new char[(*pos - start) + 1]))
      return 0;

   strncpy(buffer, start, *pos - start);
   buffer[*pos - start] = '\0';

   return buffer;
}

/*
=============
LineReadUntil
=============
   Same as above, but the line is read into the 'dest' buffer.  Also the number
of characters read will always be less than 'size'.  The 'pos' pointer is
advanced as the array is walked.

   On success 1 is returned and on failure 0 is returned.
*/
bool Utility::LineReadUntil(char **pos, char end, char *dest, int size)
{
   int cnt = 0;

   while (**pos && **pos != end && cnt < size)
   {
      dest[cnt] = **pos;
      ++cnt;
      ++(*pos);
   }

   if (cnt < size)
      dest[cnt] = '\0';
   
   return (**pos == end) && (cnt < size);
}

/*
=============
LineEat
=============
   Reads all characters until an end of line is found.  The pointer is then 
advanced to the next character after the end of line.  

   I changed this to make it clearer,
*/
void Utility::LineEat(char **pos)
{
   while(**pos)
   {
      if (**pos == '\n' || **pos == '\r')
         break;

      ++(*pos);
   }

   if (**pos)
      (*pos)++;
}

/*
=============
LineEatWhite
=============
   Advances the pointer until the first non white space character.
*/
void Utility::LineEatWhite(char **pos)
{
   while (**pos)
   {
      if (**pos != ' ' && **pos != '\t' && **pos != '\n' && **pos != '\r')
         return;

      ++(*pos);
   }
}

/*
=============
LineEatUntil   
=============
   Advances the pointer until the current character matches 'end'.
*/
void Utility::LineEatUntil(char **pos, char end)
{
   while (**pos && **pos != end)
      (*pos)++;
}

int Utility::LineReadInt(char **pos)
{
   int val = 0;
   while (**pos && (**pos >= '0' && **pos <= '9'))
      val = (val * 10) + (*(*pos)++ - '0');
   return val;
}

}