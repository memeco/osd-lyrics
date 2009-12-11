#include "ol_classic_module.h"

static void
ol_classic_module_init_classic (OlClassicModule *module)
{
  module->classic = OL_CLASSIC_WINDOW (ol_classic_window_new ());
  if (module->classic == NULL)
  {
    return;
  }
}

OlClassicModule*
ol_classic_module_new ()
{
  OlClassicModule *module = g_new (OlClassicModule, 1);
  module->classic = NULL;
  module->lrc_file = NULL;
  ol_music_info_init (&module->music_info);
  ol_classic_module_init_classic (module);
  return module;
}

void
ol_classic_module_destroy (OlClassicModule *module)
{
  if (module->classic != NULL)
    g_object_unref (module->classic);
  ol_music_info_finalize (&module->music_info);
  g_free (module);
}

void
ol_classic_module_set_music_info (OlClassicModule *module, OlMusicInfo *music_info)
{
  g_return_if_fail (music_info != NULL);
  ol_music_info_copy (&module->music_info, music_info);
  if (module->classic != NULL)
  {
    ol_classic_window_set_lyric (module->classic, NULL);
  }
}

void
ol_classic_module_set_played_time (OlClassicModule *module, int played_time)
{

  if (module->lrc_file != NULL && module->classic != NULL)
  {
    char current_lrc[1024];
    double percentage;
    int id, lyric_id;
    ol_lrc_utility_get_lyric_by_time (module->lrc_file,
                                      played_time,
                                      module->duration,
                                      current_lrc,
                                      &percentage,
                                      &lyric_id);
    fprintf (stderr, "per:%lf\n", percentage);
    LrcInfo *info = ol_lrc_parser_get_lyric_by_id (module->lrc_file,
                                                   lyric_id);
    fprintf (stderr, "per:%d\n", lyric_id);

    if (lyric_id == -1)
    {
      ol_classic_module_set_lrc(module, NULL);
      return;
    }
    else
    {
      if (lyric_id != module->classic->current_lyric_id)
      {
        module->classic->current_lyric_id = lyric_id;
        ol_classic_window_set_lyric (module->classic, info);
      }
      else
        ol_classic_window_set_current_percentage (module->classic, percentage);
    }
    
  }
  else
    ol_classic_module_set_lrc (module->classic, NULL);
}



void
ol_classic_module_set_lrc (OlClassicModule *module, LrcQueue *lrc_file)
{
  module->lrc_file = lrc_file;
  if(module->classic == NULL)
  {
      module->classic = OL_CLASSIC_WINDOW (ol_classic_window_new ());
  }
  module->classic->lrc_file = lrc_file;

}

void
ol_classic_module_set_duration (OlClassicModule *module, int duration)
{
  module->duration = duration;
}
