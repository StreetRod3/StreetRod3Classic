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


#include "defs.h"
#include "game.h"
#include "system.h"
#include "CRadio.h"


///////////////////
// CRadio constructor
CRadio::CRadio()
{
    m_mp3Stream = NULL;
    m_psMP3Playlist = NULL;
    m_psMP3CurSong = NULL;
    m_MusicVolume = 80;
    m_bStopped = false;
}


///////////////////
// Initialize the radio
bool CRadio::initialize(void)
{
    m_psMP3Playlist = NULL;
    m_psMP3CurSong = NULL;
    m_bStopped = false;
    m_MusicVolume = psOptions->nMusicVolume;


    return true;
}


///////////////////
// Shutdown the radio
void CRadio::shutdown(void)
{
    BASS_StreamFree( m_mp3Stream );

    freePlaylist();
}


///////////////////
// Free the playlist
void CRadio::freePlaylist(void)
{
    mp3list_t *psMP3 = m_psMP3Playlist;
    mp3list_t *next = NULL;

    for(; psMP3; psMP3=next) {
        next = psMP3->psNext;

        delete psMP3;
    }

    m_psMP3Playlist = NULL;
    m_psMP3CurSong = NULL;
}


///////////////////
// Load the playlist
void CRadio::loadSongs(void)
{
    // Free the old playlist
    freePlaylist();

    // Get the first file
    char filename[256];
    if( !FindFirst("music", "*.mp3", filename) )
        return;

    writeLog("Radio: Loading files:\n");
    
    while(1) {

        addSong(filename);
        writeLog(" Adding %s\n",filename);

        if( !FindNext(filename) )
            break;
    }

	// TODO: Create a shuffle array

    // Play the first song
    if( m_psMP3Playlist )
        playSong(m_psMP3Playlist);

    return;
}


///////////////////
// Play a song
void CRadio::playSong(mp3list_t *psMP3)
{
    m_psMP3CurSong = psMP3;
    if( !psMP3 )
        return;

    // Stop & free the current song
    if( m_mp3Stream ) {
        BASS_ChannelStop( m_mp3Stream );
        BASS_StreamFree( m_mp3Stream );
    }

    m_mp3Stream = BASS_StreamCreateFile(false, psMP3->szFilename, 0, 0, 0);    
    if(m_mp3Stream != 0) {
        BASS_ChannelSetAttributes(m_mp3Stream, -1, m_MusicVolume, -101);
        BASS_StreamPlay( m_mp3Stream, true, 0 );
        m_bStopped = false;
    }

    // Show the error
    if(m_mp3Stream == 0) {
        writeLog("Radio: Could not play \"%s\"\n Reason: ",psMP3->szFilename);
        switch(BASS_ErrorGetCode()) {
            case BASS_ERROR_FILEOPEN:   writeLog("The file could not be opened\n");  break;
            case BASS_ERROR_FORMAT:     writeLog("The file's format is not recognised/supported\n");  break;
            case BASS_ERROR_MEM:        writeLog("There is insufficent memory\n");  break;
            case BASS_ERROR_NO3D:       writeLog("Couldn't initialize 3D support for the stream\n");  break;
            case BASS_ERROR_UNKNOWN:    writeLog("Some mystery problem\n");  break;            
            case BASS_ERROR_INIT:       writeLog("Sound is disabled\n");    break;
            default:                    writeLog("Unknown\n");  break;
        }
    }
}


///////////////////
// Go to the next song in the list
void CRadio::nextSong(void)
{
    if( m_psMP3CurSong ) {
        if( m_psMP3CurSong->psNext )
            playSong( m_psMP3CurSong->psNext );
    }
}


///////////////////
// Go to the previous song in the list
void CRadio::prevSong(void)
{
    if( m_psMP3CurSong ) {
        if( m_psMP3CurSong->psPrev )
            playSong( m_psMP3CurSong->psPrev );
    }
}


///////////////////
// Play the current song
void CRadio::playCurrent(void)
{
    playSong(m_psMP3CurSong);
}


///////////////////
// Stop the current song
void CRadio::stopSong(void)
{
    // Stop & free the current song
    if( m_mp3Stream ) {
        BASS_ChannelStop( m_mp3Stream );
        BASS_StreamFree( m_mp3Stream );
    }
    m_bStopped = true;
}


///////////////////
// Add a song to the playlist
void CRadio::addSong(char *szFilename)
{
    mp3list_t *psMP3 = new mp3list_t;
    if( !psMP3 )
        return;

    psMP3->psNext = NULL;
    psMP3->psPrev = NULL;
    strcpy(psMP3->szFilename, szFilename);

    // Link it in at the end
    mp3list_t *mp3 = m_psMP3Playlist;
    for(; mp3; mp3=mp3->psNext) {
        if( mp3->psNext == NULL ) {
            mp3->psNext = psMP3;
            psMP3->psPrev = mp3;
            break;
        }
    }

    // First item?
    if( m_psMP3Playlist == NULL )
        m_psMP3Playlist = psMP3;
}


///////////////////
// Process the player
void CRadio::processPlayer(void)
{
    // Check if the current mp3 has stopped
    if( !BASS_ChannelIsActive( m_mp3Stream ) && !m_bStopped ) {
        // Go to the next in the list
        nextSong();
    }
}


///////////////////
// Get the current song name
char *CRadio::getCurrentSong(char *szBuffer)
{
    szBuffer[0] = '\0';

    if(!m_psMP3CurSong)
        return szBuffer;

    // Remove the slash bits
    char *d = strrchr(m_psMP3CurSong->szFilename, '\\');
    if(d)
        strcpy(szBuffer, d+1);
    else
        strcpy(szBuffer,m_psMP3CurSong->szFilename);

    return szBuffer;

}