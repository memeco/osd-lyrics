#include "ol_music_info.h"
#include "ol_config.h"
#include "ol_osd_window.h"
#include "ol_osd_toolbar.h"
#include "ol_osd_module.h"
#include "ol_lrc.h"
#include "ol_stock.h"
#include "ol_menu.h"
#include "ol_app.h"
#include "ol_debug.h"

const int MESSAGE_DURATION_MS = 3000;
typedef struct _OlOsdModule OlOsdModule;

struct OlLrc;

struct _OlOsdModule
{
  OlMusicInfo music_info;
  gint lrc_id;
  gint lrc_next_id;
  gint current_line;
  gint duration;
  gint line_count;
  struct OlLrc *lrc;
  gboolean display;
  OlOsdWindow *osd;
  OlOsdToolbar *toolbar;
  guint message_source;
};

struct OlPlayer;

/** interfaces */
static OlOsdModule* ol_osd_module_new (struct OlDisplayModule *module);
static void ol_osd_module_free (struct OlDisplayModule *module);
static void ol_osd_module_set_music_info (struct OlDisplayModule *module,
                                          OlMusicInfo *music_info);
static void ol_osd_module_set_player (struct OlDisplayModule *module,
                                      struct OlPlayer *player);
static void ol_osd_module_set_status (struct OlDisplayModule *module,
                                      enum OlPlayerStatus status);
static void ol_osd_module_set_played_time (struct OlDisplayModule *module,
                                           int played_time);
static void ol_osd_module_set_lrc (struct OlDisplayModule *module,
                                   struct OlLrc *lrc_file);
static void ol_osd_module_set_duration (struct OlDisplayModule *module,
                                        int duration);
static void ol_osd_module_set_message (struct OlDisplayModule *module,
                                       const char *message,
                                       int duration_ms);
static void ol_osd_module_search_message (struct OlDisplayModule *module,
                                          const char *message);
static void ol_osd_module_search_fail_message (struct OlDisplayModule *module,
                                               const char *message);
static void ol_osd_module_download_fail_message (struct OlDisplayModule *module,
                                                 const char *message);
static void ol_osd_module_clear_message (struct OlDisplayModule *module);

/** internal functions */

/** 
 * @brief Gets the real lyric of the given lyric
 * A REAL lyric is the nearest lyric to the given lyric, whose text is not empty
 * If the given lyric text is not empty, the given lyric is a real lyric
 * If not real lyric available, returns NULL
 * @param lrc An const struct OlLrcItem
 * 
 * @return The real lyric of the lrc. returns NULL if not available
 */
static const struct OlLrcItem* ol_osd_module_get_real_lyric (const struct OlLrcItem *lrc);
static void ol_osd_module_update_next_lyric (OlOsdModule *module,
                                             const struct OlLrcItem *current_lrc);
static void ol_osd_module_init_osd (OlOsdModule *module);
static void config_change_handler (OlConfig *config, gchar *group, gchar *name, gpointer userdata);
static void ol_osd_moved_handler (OlOsdWindow *osd, gpointer data);
static gboolean ol_osd_button_release (OlOsdWindow *osd,
                                       GdkEventButton *event,
                                       gpointer data);
static void ol_osd_scroll (OlOsdWindow *osd,
                           GdkEventScroll *event,
                           gpointer data);
static gboolean hide_message (OlOsdModule *module);
static void clear_lyrics (OlOsdModule *module);

static void
ol_osd_moved_handler (OlOsdWindow *osd, gpointer data)
{
  ol_log_func ();
  OlConfig *config = ol_config_get_instance ();
  double xalign, yalign;
  ol_osd_window_get_alignment (osd, &xalign, &yalign);
  ol_config_set_double (config, "OSD", "xalign", xalign);
  ol_config_set_double (config, "OSD", "yalign", yalign);
}

static gboolean
ol_osd_button_release (OlOsdWindow *osd,
                       GdkEventButton *event,
                       gpointer data)
{
  if (event->button == 3)
  {
    gtk_menu_popup (GTK_MENU (ol_menu_get_popup ()),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    event->button,
                    event->time);
    return TRUE;
  }
  return FALSE;
}

static void
ol_osd_scroll (OlOsdWindow *osd,
               GdkEventScroll *event,
               gpointer data)
{
  int doffset;
  if (event->direction == GDK_SCROLL_DOWN ||
      event->direction == GDK_SCROLL_RIGHT)
    doffset = -200;
  else if (event->direction == GDK_SCROLL_UP ||
           event->direction == GDK_SCROLL_LEFT)
    doffset = 200;
  ol_app_adjust_lyric_offset (doffset);
}

static void
ol_osd_module_update_next_lyric (OlOsdModule *module, const struct OlLrcItem *current_lrc)
{
  if (module->line_count == 1)
  {
    module->lrc_next_id = -1;
    return;
  }
  const struct OlLrcItem *info = ol_lrc_item_next (current_lrc);
  info = ol_osd_module_get_real_lyric (info);
  if (info == NULL)
  {
    if (module->lrc_next_id == -1)
    {
      return;
    }
    else
    {
      module->lrc_next_id = -1;
      ol_osd_window_set_lyric (module->osd, 1 - module->current_line, "");
    }
  }
  else
  {
    if (module->lrc_next_id == ol_lrc_item_get_id (info))
      return;
    if (info != NULL)
    {
      module->lrc_next_id = ol_lrc_item_get_id (info);
      ol_osd_window_set_lyric (module->osd, 1 - module->current_line,
                               ol_lrc_item_get_lyric (info));
    }
  }
  ol_osd_window_set_percentage (module->osd, 1 - module->current_line, 0.0);
}

static void
config_change_handler (OlConfig *config, gchar *group, gchar *name, gpointer userdata)
{
  ol_debugf ("%s:[%s]%s\n", __FUNCTION__, group, name);
  OlOsdModule *module = (OlOsdModule*) userdata;
  if (module == NULL)
    return;
  OlOsdWindow *osd = module->osd;
  /* OlOsdWindow *osd = OL_OSD_WINDOW (userdata); */
  if (osd == NULL || !OL_IS_OSD_WINDOW (osd))
    return;
  if (strcmp (name, "locked") == 0)
  {
    ol_debugf ("  locked: %d\n", ol_config_get_bool (config, "OSD", "locked"));
    ol_osd_window_set_locked (osd,
                              ol_config_get_bool (config, "OSD", "locked"));
  }
  else if (strcmp (name, "xalign") == 0 || strcmp (name, "yalign") == 0)
  {
    double xalign = ol_config_get_double (config, "OSD", "xalign");
    double yalign = ol_config_get_double (config, "OSD", "yalign");
    ol_osd_window_set_alignment (osd, xalign, yalign);
  }
  else if (strcmp (name, "font-family") == 0)
  {
    gchar *font = ol_config_get_string (config, "OSD", "font-family");
    ol_assert (font != NULL);
    ol_osd_window_set_font_family (osd, font);
    g_free (font);
  }
  else if (strcmp (name, "font-size") == 0)
  {
    ol_osd_window_set_font_size (osd,
                                 ol_config_get_double (config, "OSD", "font-size"));
  }
  else if (strcmp (name, "width") == 0)
  {
    ol_osd_window_set_width (osd,
                             ol_config_get_int (config, "OSD", "width"));
  }
  else if (strcmp (name, "lrc-align-0") == 0)
  {
    ol_osd_window_set_line_alignment (osd, 0,
                                      ol_config_get_double (config, "OSD", name));
  }
  else if (strcmp (name, "lrc-align-1") == 0)
  {
    ol_osd_window_set_line_alignment (osd, 1,
                                      ol_config_get_double (config, "OSD", name));
  }
  else if (strcmp (name, "active-lrc-color") == 0)
  {
    int len;
    char **color_str = ol_config_get_str_list (config, "OSD", name, &len);
    ol_debugf ("len = %d\n", len);
    if (len != OL_LINEAR_COLOR_COUNT) return;
    if (color_str != NULL)
    {
      OlColor *colors = ol_color_from_str_list ((const char**)color_str, NULL);
      ol_osd_window_set_active_colors (osd, colors[0], colors[1], colors[2]);
      g_free (colors);
      g_strfreev (color_str);
    }
  }
  else if (strcmp (name, "inactive-lrc-color") == 0)
  {
    int len;
    char **color_str = ol_config_get_str_list (config, "OSD", name, &len);
    ol_debugf ("len = %d\n", len);
    if (len != OL_LINEAR_COLOR_COUNT) return;
    if (color_str != NULL)
    {
      OlColor *colors = ol_color_from_str_list ((const char**)color_str, NULL);
      ol_osd_window_set_inactive_colors (osd, colors[0], colors[1], colors[2]);
      g_free (colors);
      g_strfreev (color_str);
    }
  }
  else if (strcmp (name, "line-count") == 0)
  {
    module->line_count = ol_config_get_int (config, "OSD", name);
    ol_osd_window_set_line_count (osd, module->line_count);
  }
  else if (strcmp (name, "visible") == 0)
  {
    gboolean visible = ol_config_get_bool (config, "General", name);
    if (visible)
    {
      gtk_widget_show (GTK_WIDGET (module->osd));
    }
    else
    {
      gtk_widget_hide (GTK_WIDGET (module->osd));
    }
  }
  else if (strcmp (name, "translucent-on-mouse-over") == 0)
  {
    ol_osd_window_set_translucent_on_mouse_over (osd, ol_config_get_bool (config, "OSD", name));
  }
  else if (strcmp (name, "outline-width") == 0)
  {
    ol_osd_window_set_outline_width (osd, ol_config_get_int (config, "OSD", name));
  }
}

static void
ol_osd_module_init_osd (OlOsdModule *module)
{
  module->osd = OL_OSD_WINDOW (ol_osd_window_new ());
  if (module->osd == NULL)
    return;
  GtkIconTheme *icontheme = gtk_icon_theme_get_default ();
  GdkPixbuf *bg = gtk_icon_theme_load_icon (icontheme,
                                            OL_STOCK_OSD_BG,
                                            32,
                                            0,
                                            NULL);
  ol_osd_window_set_bg (module->osd, bg);
  g_object_unref (bg);
  /* ol_osd_window_resize (osd, 1024, 100); */
  /* gtk_widget_show (GTK_WIDGET (module->osd)); */
  module->toolbar = OL_OSD_TOOLBAR (ol_osd_toolbar_new ());
  if (module->toolbar != NULL)
  {
    gtk_container_add (GTK_CONTAINER (module->osd),
                       GTK_WIDGET (module->toolbar));
    gtk_widget_show_all (GTK_WIDGET (module->toolbar));
  }
  module->display = FALSE;
  OlConfig *config = ol_config_get_instance ();
  ol_assert (config != NULL);
  config_change_handler (config, "OSD", "visible", module);
  config_change_handler (config, "OSD", "locked", module);
  config_change_handler (config, "OSD", "line-count", module);
  config_change_handler (config, "OSD", "xalign", module);
  config_change_handler (config, "OSD", "font-family", module);
  config_change_handler (config, "OSD", "font-size", module);
  config_change_handler (config, "OSD", "width", module);
  config_change_handler (config, "OSD", "lrc-align-0", module);
  config_change_handler (config, "OSD", "lrc-align-1", module);
  config_change_handler (config, "OSD", "active-lrc-color", module);
  config_change_handler (config, "OSD", "inactive-lrc-color", module);
  config_change_handler (config, "OSD", "translucent-on-mouse-over", module);
  config_change_handler (config, "OSD", "outline-width", module);
  g_signal_connect (module->osd, "moved",
                    G_CALLBACK (ol_osd_moved_handler),
                    NULL);
  g_signal_connect (module->osd, "button-release-event",
                    G_CALLBACK (ol_osd_button_release),
                    NULL);
  g_signal_connect (module->osd, "scroll-event",
                    G_CALLBACK (ol_osd_scroll),
                    NULL);
  g_signal_connect (config, "changed",
                    G_CALLBACK (config_change_handler),
                    module);
}

static OlOsdModule*
ol_osd_module_new (struct OlDisplayModule *module)
{
  ol_log_func ();
  OlOsdModule *data = g_new (OlOsdModule, 1);
  data->osd = NULL;
  data->lrc = NULL;
  data->lrc_id = -1;
  data->lrc_next_id = -1;
  data->current_line = 0;
  data->message_source = 0;
  ol_music_info_init (&data->music_info);
  ol_osd_module_init_osd (data);
  return data;
}

static void
ol_osd_module_free (struct OlDisplayModule *module)
{
  ol_log_func ();
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  if (priv->osd != NULL)
  {
    g_object_unref (priv->osd);
    priv->osd = NULL;
  }
  ol_music_info_clear (&priv->music_info);
  g_free (priv);
}

static void
ol_osd_module_set_music_info (struct OlDisplayModule *module, OlMusicInfo *music_info)
{
  ol_log_func ();
  ol_assert (music_info != NULL);
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  ol_music_info_copy (&priv->music_info, music_info);
  clear_lyrics (priv);
  hide_message (priv);
}

static void
ol_osd_module_set_played_time (struct OlDisplayModule *module, int played_time)
{
  /* updates the time and lyrics */
  /* ol_log_func (); */
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  if (priv->lrc != NULL && priv->osd != NULL)
  {
    double percentage;
    int id, lyric_id;
    ol_lrc_get_lyric_by_time (priv->lrc,
                              played_time,
                              priv->duration,
                              NULL,
                              &percentage,
                              &lyric_id);
    const struct OlLrcItem *info = ol_lrc_get_item (priv->lrc, lyric_id);
    info = ol_osd_module_get_real_lyric (info);
    if (info == NULL)
      id = -1;
    else
      id = ol_lrc_item_get_id (info);
    if (priv->lrc_id != id)
    {
      if (id == -1)
      {
        clear_lyrics (priv);
        return;
      }
      if (id != priv->lrc_next_id)
      {
        priv->current_line = 0;
        if (ol_lrc_item_get_lyric (info) != NULL)
          ol_osd_window_set_lyric (priv->osd, priv->current_line,
                                   ol_lrc_item_get_lyric (info));
        if (id != lyric_id)
          ol_osd_window_set_current_percentage (priv->osd, 0.0);
        ol_osd_module_update_next_lyric (priv, info);
      }
      else
      {
        ol_osd_window_set_percentage (priv->osd, priv->current_line, 1.0);
        priv->current_line = 1 - priv->current_line;
      } /* if (id != module->lrc_next_id) */
      priv->lrc_id = id;
      ol_osd_window_set_current_line (priv->osd, priv->current_line);
    } /* if (module->lrc_id != id) */
    if (id == lyric_id && percentage > 0.5)
      ol_osd_module_update_next_lyric (priv, info);
    if (id == lyric_id)
    {
      ol_osd_window_set_current_percentage (priv->osd, percentage);
    }
    /* if (!module->display) */
    /* { */
    /*   module->display = TRUE; */
    /*   if (ol_config_get_bool (ol_config_get_instance (), "General", "visible")) */
    /*     gtk_widget_show (GTK_WIDGET (module->osd)); */
    /* } */
  } /* if (module->lrc_file != NULL && module->osd != NULL) */
  else
  {
    /* if (module->osd != NULL && GTK_WIDGET_MAPPED (GTK_WIDGET (module->osd))) */
    /* { */
    /*   ol_debug ("-1"); */
    /*   clear_lyrics (module); */
    /* } */
  }
}

static const struct OlLrcItem*
ol_osd_module_get_real_lyric (const struct OlLrcItem *lrc)
{
  while (lrc != NULL)
  {
    if (!ol_is_string_empty (ol_lrc_item_get_lyric (lrc)))
      break;
    lrc = ol_lrc_item_next (lrc);
  }
  return lrc;
}

static void
ol_osd_module_set_lrc (struct OlDisplayModule *module, struct OlLrc *lrc_file)
{
  ol_log_func ();
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  priv->lrc = lrc_file;
  if (lrc_file != NULL && priv->message_source != 0)
  {
    ol_osd_module_clear_message (module);
  }
  if (lrc_file == NULL && priv->message_source == 0)
  {
    clear_lyrics (priv);
  }
  /* if (lrc_file != NULL) */
  /*   module->display = TRUE; */
}

static void
ol_osd_module_set_duration (struct OlDisplayModule *module, int duration)
{
  ol_log_func ();
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  priv->duration = duration;
}

static void
ol_osd_module_set_message (struct OlDisplayModule *module,
                           const char *message,
                           int duration_ms)
{
  ol_log_func ();
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  ol_assert (message != NULL);
  ol_assert (priv->osd != NULL);
  if (priv->lrc != NULL)
    return;
  ol_debugf ("  message:%s\n", message);
  ol_osd_window_set_current_line (priv->osd, 0);
  ol_osd_window_set_current_percentage (priv->osd, 1.0);
  ol_osd_window_set_lyric (priv->osd, 0, message);
  ol_osd_window_set_lyric (priv->osd, 1, NULL);
  if (priv->message_source != 0)
    g_source_remove (priv->message_source);
  priv->message_source = g_timeout_add (duration_ms,
                                        (GSourceFunc) hide_message,
                                        (gpointer) module);
}

static void
ol_osd_module_search_message (struct OlDisplayModule *module, const char *message)
{
  ol_osd_module_set_message (module, message, -1);
}

static void
ol_osd_module_search_fail_message (struct OlDisplayModule *module, const char *message)
{
  ol_osd_module_set_message (module, message, MESSAGE_DURATION_MS);
}

static void
ol_osd_module_download_fail_message (struct OlDisplayModule *module, const char *message)
{
  ol_osd_module_set_message (module, message, MESSAGE_DURATION_MS);
}

static gboolean
hide_message (OlOsdModule *module)
{
  ol_log_func ();
  ol_assert_ret (module != NULL, FALSE);
  if (module->lrc != NULL)
    return FALSE;
  ol_osd_window_set_lyric (module->osd, 0, NULL);
  /* gtk_widget_hide (GTK_WIDGET (module->osd)); */
  module->message_source = 0;
  return FALSE;
}

static void
clear_lyrics (OlOsdModule *module)
{
  ol_log_func ();
  if (module->osd != NULL && module->message_source == 0)
  {
    module->display = FALSE;
    /* gtk_widget_hide (GTK_WIDGET (module->osd)); */
    ol_osd_window_set_lyric (module->osd, 0, NULL);
    ol_osd_window_set_lyric (module->osd, 1, NULL);
  }
  module->current_line = 0;
  module->lrc_id = -1;
  module->lrc_next_id = -1;
}

static void
ol_osd_module_clear_message (struct OlDisplayModule *module)
{
  ol_log_func ();
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  if (priv->message_source != 0)
  {
    g_source_remove (priv->message_source);
    hide_message (priv);
  }
  ol_debug ("  clear message done");
}

static void
ol_osd_module_set_player (struct OlDisplayModule *module, struct OlPlayer *player)
{
  ol_log_func ();
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  if (priv->toolbar != NULL)
    ol_osd_toolbar_set_player (priv->toolbar, player);
}

static void
ol_osd_module_set_status (struct OlDisplayModule *module, enum OlPlayerStatus status)
{
  ol_log_func ();
  ol_assert (module != NULL);
  OlOsdModule *priv = ol_display_module_get_data (module);
  ol_assert (priv != NULL);
  if (priv->toolbar != NULL)
    ol_osd_toolbar_set_status (priv->toolbar, status);
}

struct OlDisplayClass*
ol_osd_module_get_class ()
{
  struct OlDisplayClass *klass = ol_display_class_new ("OSD",
                                                       ol_osd_module_new,
                                                       ol_osd_module_free);
  klass->clear_message = ol_osd_module_clear_message;
  klass->download_fail_message = ol_osd_module_download_fail_message;
  klass->search_fail_message = ol_osd_module_search_fail_message;
  klass->search_message = ol_osd_module_search_message;
  klass->set_duration = ol_osd_module_set_duration;
  klass->set_lrc = ol_osd_module_set_lrc;
  klass->set_message = ol_osd_module_set_message;
  klass->set_music_info = ol_osd_module_set_music_info;
  klass->set_played_time = ol_osd_module_set_played_time;
  klass->set_player = ol_osd_module_set_player;
  klass->set_status = ol_osd_module_set_status;
  return klass;
}
