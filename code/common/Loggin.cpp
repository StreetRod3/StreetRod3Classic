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


// Logging system
// Created 7/5/02
// Jason Boettcher


#include "defs.h"

FILE *fpLog = NULL;

///////////////////
// Write a log to a file
void writeLog(char *fmt, ...)
{
	va_list arg;

	// If the file isn't open, open it
	if(!fpLog) {
		fpLog = fopen("logfile.txt","wt");
		if(fpLog == NULL)
			return;

		fprintf(fpLog,"Street Rod 3 Log file:\n"
					  "----------------------\n\n");
	}

	if(fpLog == NULL)
		return;

	va_start(arg, fmt);
	vfprintf(fpLog, fmt, arg);
	va_end(arg);

	fflush(fpLog);
}


///////////////////
// Close the log file
void closeLog(void)
{
	if(fpLog)
		fclose(fpLog);
	fpLog = NULL;
}