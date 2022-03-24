/////////////////////////////////////////
//
//             Street Rod 3
//
// Copyright (C) Auxiliary Software 2002-2004
//
//
/////////////////////////////////////////
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/


// Common functions
// Created 23/5/02
// Jason Boettcher


#include "defs.h"
#include "System.h"


///////////////////
// Trim the leading/ending spaces/tabs from a string of text
void TrimSpaces(char *str)
{
	char	temp[256];
	uint	n=0, i=0;
	strcpy(temp,"");


	// Leading spaces
	for(n=0,i=0;n<strlen(str);n++) {
		if(str[n] != ' ' && str[n] != 9)
			break;
		i++;
	}
	strcpy(temp,str+i);


	// proceeding spaces
	for(n=strlen(temp)-1;n>=0;n--)
		if(temp[n] != ' ' && temp[n] != 9) {
			temp[n+1] = '\0';
			break;
		}

	strcpy(str,temp);
}


///////////////////
// Safe string copy routine
void sr_strncpy(char *dest, char *src, int count)
{
    while(*src && count--)	{
		*dest++ = *src++;
	}
	if(count)
		*dest++ = 0;
}


///////////////////
// Convert text newlines into character newlines
// ie \ & n into \n
void ConvertNewlines(char *str)
{
    // Safety
    if( !str )
        return;
    if( strlen(str) < 3 )
        return;

    for( int i=0; i<strlen(str)-1; i++) {
        if( str[i] == '\\' && str[i+1] == 'n' ) {
            str[i] = '\n';

            // Move the string down one
            for(int n=i+1; n<strlen(str); n++)
                str[n] = str[n+1];
        }
    }

}


///////////////////
// Get a pixel from the surface
Uint32 GetPixel(SDL_Surface *bmpSrc, int x, int y)
{
    int bpp = bmpSrc->format->BytesPerPixel;
    Uint8 *p = (Uint8 *)bmpSrc->pixels + y * bmpSrc->pitch + x * bpp;

    switch(bpp) {

		// 8 bpp
		case 1:
			return *p;

		// 16 bpp
		case 2:
			return *(Uint16 *)p;

		// 24 bpp
		case 3:
			if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
				return p[0] << 16 | p[1] << 8 | p[2];
			else
				return p[0] | p[1] << 8 | p[2] << 16;

		// 32 bpp
		case 4:
			return *(Uint32 *)p;

		default:
			return 0;
    }
}


///////////////////
// Extract 4 colour components from a packed int
void GetColour4(Uint32 pixel, SDL_Surface *img, Uint8 *r, Uint8 *g, Uint8 *b, Uint8 *a)
{
	SDL_GetRGBA(pixel,img->format,r,g,b,a);
}


///////////////////
// Create a new string
char *newString(char *szString)
{
    char *str = (char *)malloc(strlen(szString)+1);
    if(!str)
        return NULL;

    strcpy(str, szString);

    return str;
}


///////////////////
// Add a string onto another
char *addString(char *dst, char *src)
{
    if(!dst)
        return newString(src);

    // Re-allocate the dest string
    dst = (char *)realloc(dst, strlen(dst) + strlen(src) + 2);
    
    strcat(dst, src);

    return dst;
}


///////////////////
// Write a pascal type string to a file
void writePascalString(char *szString, FILE *fp)
{
    byte l = strlen(szString);
    fwrite(&l,      sizeof(byte),   1,  fp);
    fwrite(szString,sizeof(char),   l,  fp);
}


///////////////////
// Read a pascal type string from a file
char *readPascalString(char *szString, FILE *fp)
{
    byte l = 0;
    fread(&l,       sizeof(byte),   1,  fp);
    fread(szString, sizeof(char),   l,  fp);

    szString[l] = '\0';
    return szString;
}


///////////////////
// Wrap a string by adding newlines
char *wrapText(char *szString, int nWidth, int nFontSize)
{
    int nLength = strlen(szString);
    int nBreakPos = -1;
    int nWord = 0;
    int nPos = 0;

    for(int i=0; i<nLength+1; i++) {
        char c = szString[i];
        nPos += nFontSize;

        if(c == ' ' || c == '\0') {
            nWord = 0;
            if(nPos > nWidth && nBreakPos >= 0) {
                szString[nBreakPos] = '\n';
                nPos = 0;
            }
            nBreakPos = i;
            continue;
        }

        nWord++;
    }

    return szString;
}