#ifndef __CLASSIC_WINDOW_H_
#define __CLASSIC_WINDOW_H_

#include <gtk/gtk.h>
#include "ol_lrc_parser.h"


#define OL_CLASSIC_WINDOW(obj)                   GTK_CHECK_CAST (obj, ol_classic_window_get_type (), OlClassicWindow)
#define OL_CLASSIC_WINDOW_CLASS(klass)           GTK_CHECK_CLASS_CAST (klass, ol_classic_window_get_type (), OlClassicWindowClass)
#define OL_IS_CLASSIC_WINDOW(obj)                GTK_CHECK_TYPE (obj, ol_classic_window_get_type ())
#define OL_CLASSIC_WINDOW_GET_CLASS(obj)         (G_TYPE_INSTANCE_GET_CLASS ((obj), ol_classic_window_get_type (), OlClassicWindowClass))
#define OL_CLASSIC_WINDOW_MAX_LINE_COUNT         9


typedef struct _OlClassicWindow                  OlClassicWindow;
typedef struct _OlClassicWindowClass             OlClassicWindowClass;

struct _OlClassicWindow
{
  GtkWidget widget;
  gchar *paint_lyrics[OL_CLASSIC_WINDOW_MAX_LINE_COUNT];
  double percentage;
  LrcQueue *lrc_file;
  gint current_lyric_id;
};


struct _OlClassicWindowClass
{
  GtkWidgetClass parent_class;
};

GtkType ol_classic_window_get_type (void);

/** 
 * @brief create a new Osd Window. 
 * To destroy the Osd Window, use g_object_unref
 */

GtkWidget* ol_classic_window_new (void);

/** 
 * @brief Set the lyric of certain line
 * If a line of lyric is set, it will changes to the lyric.
 * @param osd An OlOsdWindow
 * @param line The line whose lyric will be set. Can be 0 or 1.
 * @param lyric The lyric of the line. NULL means the line has no lyric currently.
 */
void ol_classic_window_set_lyric (OlClassicWindow *osd, const LrcInfo *lyric);



/** 
 * @brief Sets the progress of the given lyric line
 * The color of the left part of the given lyric line will be changed, which makes the lyric KaraOK-like.
 * @param osd An OlOsdWindow
 * @param line The line of lyric
 * @param percentage The width percentage of the left part whose color is changed
 */
void ol_classic_window_set_percentage (OlClassicWindow *osd, double percentage);
/** 
 * @brief Sets the progress of the current lyric line
 * The color of the left part of the current lyric line will be changed, which makes the lyric KaraOK-like.
 * @param osd An OlOsdWindow
 * @param percentage The width percentage of the left part whose color is changed
 */
void ol_classic_window_set_current_percentage (OlClassicWindow *osd, double percentage);

#endif /* __OL_OSD_WINDOW_H__ */
