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


// Radio class
// Created 27/5/03
// Jason Boettcher


#ifndef __CRADIO_H__
#define __CRADIO_H__


// MP3 playlist structure
typedef struct mp3list_s {

    char        szFilename[256];

    struct  mp3list_s *psNext;
    struct  mp3list_s *psPrev;

} mp3list_t;



class CRadio {
private:
    // Attributes

    HSTREAM     m_mp3Stream;
    mp3list_t   *m_psMP3Playlist;
    mp3list_t   *m_psMP3CurSong;
    int         m_MusicVolume;
    bool        m_bStopped;



public:
    // Methods

    CRadio();
    
    bool        initialize(void);
    void        shutdown(void);

    // Player
    void        processPlayer(void);
    void        loadSongs(void);
    void        addSong(char *szFilename);
    void        freePlaylist(void);
    void        playSong(mp3list_t *psMP3);
    void        nextSong(void);
    void        prevSong(void);
    void        playCurrent(void);
    void        stopSong(void);

    // Gui display
    bool        initDisplay(void);
    void        processDisplay(void);

    char        *getCurrentSong(char *szBuffer);


};





#endif  //  __CRADIO_H__