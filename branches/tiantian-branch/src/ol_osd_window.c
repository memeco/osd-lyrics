#include <gtk/gtkprivate.h>
#include "ol_osd_window.h"
#include <pango/pangocairo.h>

#define FONT "Sans Bold 10"


#define OL_OSD_WINDOW_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE \
                                                 ((obj),                        \
                                                 ol_osd_window_get_type (),\
                                                  OlOsdWindowPrivate))

static void ol_osd_window_init (OlOsdWindow *self);
static void ol_osd_window_class_init (OlOsdWindowClass *klass);

static void ol_osd_window_destroy (GtkObject *object);

static void ol_osd_window_realize (GtkWidget *widget);
static void ol_osd_window_unrealize (GtkWidget *widget);

static void ol_osd_window_map (GtkWidget *widget);
static void ol_osd_window_unmap (GtkWidget *widget);

static void ol_osd_window_size_allocate (GtkWidget *widget, GtkAllocation *allocation);
static void ol_osd_window_size_request (GtkWidget *widget, GtkRequisition *requisition);

static void ol_osd_window_show (GtkWidget *widget);
static gboolean ol_osd_window_expose (GtkWidget *widget, GdkEventExpose *event);

static void ol_osd_window_paint (OlOsdWindow *osd);

static void ol_osd_window_paint_lyrics (OlOsdWindow *osd, cairo_t *cr);

static void ol_osd_window_set_paint_lyrics (OlOsdWindow *osd);

static GtkWidgetClass *parent_class = NULL;

GType
ol_osd_window_get_type (void)
{
  static GType ol_osd_type = 0;
  if (!ol_osd_type)
  {
    static const GTypeInfo ol_osd_info =
      {
        sizeof (OlOsdWindowClass),
        NULL,                   /* base_init */
        NULL,                   /* base_finalize */
        (GClassInitFunc) ol_osd_window_class_init,
        NULL,                   /* class_finalize */
        NULL,                   /* class_data */
        sizeof (OlOsdWindow),
        16,                     /* n_preallocs */
        (GInstanceInitFunc) ol_osd_window_init,
      };
    ol_osd_type = g_type_register_static (GTK_TYPE_WIDGET, "OlOsdWindow",
                                          &ol_osd_info, 0);
  }
  return ol_osd_type;
}


GtkWidget*
ol_osd_window_new ()
{
  printf ("new\n");
  OlOsdWindow *osd;
  osd = g_object_new (ol_osd_window_get_type (), NULL);
  return GTK_WIDGET (osd);
}

static void
ol_osd_window_init (OlOsdWindow *self)
{
  printf ("init\n");
  GTK_WIDGET_SET_FLAGS (self, GTK_TOPLEVEL);
  GTK_PRIVATE_SET_FLAG (self, GTK_ANCHORED);
  int i;
  for (i = 0; i < 10; i++)
  {
    self->paint_lyrics[i] = NULL;
  }
  self->percentage = 0.0;
  self->lrc_file = NULL;
  self->current_lyric_id = -1; 
}

static void
ol_osd_window_class_init (OlOsdWindowClass *klass)
{
  printf("class_init\n");
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass*) klass;
  widget_class = (GtkWidgetClass*) klass;
  parent_class = g_type_class_peek_parent (klass);
  // parent_class = gtk_type_class (klass);

  object_class->destroy = ol_osd_window_destroy;

  widget_class->realize = ol_osd_window_realize;
  widget_class->unrealize = ol_osd_window_unrealize;
  widget_class->size_allocate = ol_osd_window_size_allocate;
  widget_class->size_request = ol_osd_window_size_request;
  widget_class->map = ol_osd_window_map;
  widget_class->unmap = ol_osd_window_unmap;
  widget_class->show = ol_osd_window_show;
  widget_class->expose_event = ol_osd_window_expose;
}

static void
ol_osd_window_destroy (GtkObject *object)
{
  OlOsdWindow *osd = OL_OSD_WINDOW (object);
  if (osd->current_lyric_id!= -1)
  {
    //g_free (osd->lyric);
    osd->current_lyric_id = -1;
  }
  GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
ol_osd_window_realize (GtkWidget *widget)
{
  GdkWindowAttr attributes;
  guint attributes_mask;

  g_return_if_fail(widget != NULL);

  GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

  attributes.window_type = GDK_WINDOW_CHILD;
  attributes.x = widget->allocation.x;
  attributes.y = widget->allocation.y;
  attributes.width = 800;
  attributes.height = 1000;

  attributes.wclass = GDK_INPUT_OUTPUT;
  attributes.event_mask = gtk_widget_get_events(widget) | GDK_EXPOSURE_MASK;

  attributes_mask = GDK_WA_X | GDK_WA_Y;

  widget->window = gdk_window_new(
     gtk_widget_get_parent_window (widget),
     & attributes, attributes_mask
  );

  gdk_window_set_user_data(widget->window, widget);

  widget->style = gtk_style_attach(widget->style, widget->window);
  gtk_style_set_background(widget->style, widget->window, GTK_STATE_NORMAL);


  
}


static void
ol_osd_window_unrealize (GtkWidget *widget)
{
  printf ("unrealize\n");
  GTK_WIDGET_CLASS(parent_class)->unrealize (widget);
}


static void
ol_osd_window_map (GtkWidget *widget)
{
  fprintf (stderr, "%s\n", __FUNCTION__);
  GTK_WIDGET_CLASS (parent_class)->map (widget);
  
  if (GTK_WIDGET_MAPPED (widget))
    return;
  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);
}

static void
ol_osd_window_unmap (GtkWidget *widget)
{
  printf ("unmap\n");
  GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED);
}


static void
ol_osd_window_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
  printf ("%s\n"
          "(%d, %d) - %d x %d\n",
          __FUNCTION__,
          allocation->x,
          allocation->y,
          allocation->width,
          allocation->height);

  g_return_if_fail(widget != NULL);
  g_return_if_fail(OL_IS_OSD_WINDOW(widget));
  g_return_if_fail(allocation != NULL);

  widget->allocation = *allocation;

  if (GTK_WIDGET_REALIZED(widget)) {
     gdk_window_move_resize(
         widget->window,
         allocation->x,
         allocation->y,
         allocation->width,
         allocation->height
     );
  }
}


static void
ol_osd_window_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
  g_return_if_fail(widget != NULL);
  g_return_if_fail(OL_IS_OSD_WINDOW(widget));
  g_return_if_fail(requisition != NULL);

  requisition->width = 1024;
  requisition->height =600;
}

static void
ol_osd_window_show (GtkWidget *widget)
{
  printf ("show\n");
  GTK_WIDGET_SET_FLAGS (widget, GTK_VISIBLE);
  gtk_widget_map (widget);
}

static gboolean
ol_osd_window_expose (GtkWidget *widget, GdkEventExpose *event)
{
  /* fprintf (stderr, "%s\n", __FUNCTION__); */
  ol_osd_window_paint (OL_OSD_WINDOW (widget));
  return FALSE;
}

static void
ol_osd_window_paint (OlOsdWindow *osd)
{
  g_return_if_fail (OL_IS_OSD_WINDOW (osd));
  GtkWidget *widget = GTK_WIDGET (osd);
  g_return_if_fail (GTK_WIDGET_REALIZED (widget));
  if (!GTK_WIDGET_VISIBLE (widget))
    return;
  cairo_t *cr;
  cr = gdk_cairo_create (widget->window);
  ol_osd_window_paint_lyrics (osd, cr);
  //cairo_destroy (cr); 
}

static void
ol_osd_window_paint_lyrics (OlOsdWindow *osd, cairo_t *cr)
{
  g_return_if_fail (osd != NULL);
  GtkWidget *widget = GTK_WIDGET (osd);
  if (!GTK_WIDGET_REALIZED (widget))
    gtk_widget_realize (widget);
  
  gdouble ypos = 400, xpos = 100;
  double percentage = osd->percentage;
  printf("%f",percentage);


  cairo_set_source_rgb(cr, 0.5, 0.5, 1);
  cairo_rectangle(cr, 100, 0, 400, 400);
  cairo_fill(cr);


  
  /* paint the lyric */
   PangoLayout *layout;
   PangoFontDescription *desc;
   int font_height;
   int width, height,i,y=10;
   layout = pango_cairo_create_layout (cr);
   cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);


   if (osd->paint_lyrics !=NULL)
   {
     for (i=0; i<10; i++)
     {
       pango_layout_set_text (layout, osd->paint_lyrics[i], -1);
       desc = pango_font_description_from_string (FONT);
       pango_layout_set_font_description (layout, desc);
       pango_font_description_free (desc);

       cairo_save (cr);
       if (i == 6)
       {
         cairo_set_source_rgb(cr, 0.1, 0.5, 0.1);
       }
       cairo_move_to (cr, xpos,50+y*(1-percentage)+i*20);

       pango_cairo_update_layout (cr, layout);
       //pango_layout_get_size (layout, &width, &height);
       // cairo_move_to (cr, - ((double)width / PANGO_SCALE) / 2, - RADIUS);
       pango_cairo_show_layout (cr, layout);
       cairo_restore (cr);
       cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
     }
     g_object_unref (layout);
   }









  
  /* cairo_set_source_rgb(cr, 0.5, 0.5, 1);
  cairo_rectangle(cr, 100, 0, 400, 400);
  cairo_fill(cr);

  cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
  cairo_select_font_face(cr, "Purisa",
      CAIRO_FONT_SLANT_NORMAL,
      CAIRO_FONT_WEIGHT_BOLD);
  cairo_set_font_size(cr, 13);


  int i = 0,y = 10;
  
  for (i = 0; i<10 ;i++)
  {
    if (i == 4)
    {
      cairo_set_source_rgb(cr, 0.1, 0.5, 0.1);
    }
    cairo_move_to (cr, xpos,50+y*(1-percentage)+i*20);
    cairo_show_text(cr, osd->paint_lyrics[i]);
    cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
  }*/

  cairo_destroy(cr);
}

void
ol_osd_window_set_lyric (OlOsdWindow *osd, const LrcInfo *lyric)
{
  // fprintf (stderr, "%s\n%s\n",
  //       __FUNCTION__, lyric);
  g_return_if_fail (OL_IS_OSD_WINDOW (osd));
  /*
  if (osd->lyric!= NULL)
    g_free (osd->lyric);
  if (lyric != NULL)
    osd->lyric = g_strdup (lyric);
  else
  osd->lyric = g_strdup ("");*/
  if (lyric != NULL)
    osd->current_lyric_id = ol_lrc_parser_get_lyric_id (lyric);
  else
    osd->current_lyric_id = -1;
  ol_osd_window_set_paint_lyrics (osd);
  ol_osd_window_paint(osd);

}


void
ol_osd_window_set_current_percentage (OlOsdWindow *osd, double percentage)
{
  /*   fprintf (stderr, "%s:%lf\n", __FUNCTION__, percentage); */
  ol_osd_window_set_percentage (osd, percentage);
}


void
ol_osd_window_set_percentage (OlOsdWindow *osd, double percentage)
{
  /*   fprintf (stderr, "%s:%lf\n", __FUNCTION__, percentage); */
  g_return_if_fail (OL_IS_OSD_WINDOW (osd));
  osd->percentage = percentage;
  ol_osd_window_paint (osd);
}

static void
ol_osd_window_set_paint_lyrics (OlOsdWindow *osd)
{
  int id = osd->current_lyric_id;
  LrcInfo *info;
  if (id == -1)
  {
    int i;
    for(i = 0; i <10 ; i++)
    {
      osd->paint_lyrics[i] = NULL;
    }
  }
  else if (id > 5)
  {
    int i = 0;
    for (i = 0; i < 10; i++)
    {
      info = ol_lrc_parser_get_lyric_by_id (osd->lrc_file, (id-6+i));
      osd->paint_lyrics[i] = ol_lrc_parser_get_lyric_text (info);
    }
  }
  else
  {
    int j = 0;
    for (j = 0; j < 5-id ;j++)
    {
      if (j < 5-id)
      {
        osd->paint_lyrics[j] = "";
      }
      else
      {
        info = ol_lrc_parser_get_lyric_by_id (osd->lrc_file, (id-6+j));
        osd->paint_lyrics[j] = ol_lrc_parser_get_lyric_text (info);
      }
    }
  }
}
