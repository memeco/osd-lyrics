#ifndef _OL_CLASSIC_MODULE_H_
#define _OL_CLASSIC_MODULE_H_

#include "ol_lrc_parser.h"
#include "ol_lrc_utility.h"
#include "ol_music_info.h"
#include "ol_classic_window.h"

typedef struct _OlClassicModule OlClassicModule;

struct _OlClassicModule
{
  OlMusicInfo music_info;
  gint duration;
  LrcQueue *lrc_file;
  OlClassicWindow *classic;
};

OlClassicModule* ol_classic_module_new ();
void ol_classic_module_destroy (OlClassicModule *module);
void ol_classic_module_set_music_info (OlClassicModule *module, OlMusicInfo *music_info);
void ol_classic_module_set_played_time (OlClassicModule *module, int played_time);
void ol_classic_module_set_lrc (OlClassicModule *module, LrcQueue *lrc_file);
void ol_classic_module_set_duration (OlClassicModule *module, int duration);

#endif

