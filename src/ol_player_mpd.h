#ifndef __OL_PLAYER_MPD_H__
#define __OL_PLAYER_MPD_H__

#include "ol_player.h"

/** 
 * @brief Creates a controller of Mpd
 * 
 * @return The controller of Mpd. It's allocated by g_new, so use g_free to free the memory
 */
struct OlPlayer* ol_player_mpd_get ();

#endif // __OL_PLAYER_MPD_H__
