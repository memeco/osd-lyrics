#include <gtk/gtkprivate.h>
#include "ol_classic_window.h"
#include <pango/pangocairo.h>

#define FONT "Sans Bold 10"


#define OL_CLASSIC_WINDOW_GET_PRIVATE(obj)    (G_TYPE_INSTANCE_GET_PRIVATE \
                                                 ((obj),                        \
                                                 ol_classic_window_get_type (),\
                                                  OlClassicWindowPrivate))

static void ol_classic_window_init (OlClassicWindow *self);
static void ol_classic_window_class_init (OlClassicWindowClass *klass);

static void ol_classic_window_destroy (GtkObject *object);

static void ol_classic_window_realize (GtkWidget *widget);
static void ol_classic_window_unrealize (GtkWidget *widget);

static void ol_classic_window_map (GtkWidget *widget);
static void ol_classic_window_unmap (GtkWidget *widget);

static void ol_classic_window_size_allocate (GtkWidget *widget, GtkAllocation *allocation);
static void ol_classic_window_size_request (GtkWidget *widget, GtkRequisition *requisition);

static void ol_classic_window_show (GtkWidget *widget);
static gboolean ol_classic_window_expose (GtkWidget *widget, GdkEventExpose *event);

static void ol_classic_window_paint (OlClassicWindow *classic);

static void ol_classic_window_paint_lyrics (OlClassicWindow *classic, cairo_t *cr);

static void ol_classic_window_set_paint_lyrics (OlClassicWindow *classic);

static GtkWidgetClass *parent_class = NULL;


GType
ol_classic_window_get_type (void)
{
  static GType ol_classic_type = 0;
  if (!ol_classic_type)
  {
    static const GTypeInfo ol_classic_info =
      {
        sizeof (OlClassicWindowClass),
        NULL,                   /* base_init */
        NULL,                   /* base_finalize */
        (GClassInitFunc) ol_classic_window_class_init,
        NULL,                   /* class_finalize */
        NULL,                   /* class_data */
        sizeof (OlClassicWindow),
        16,                     /* n_preallocs */
        (GInstanceInitFunc) ol_classic_window_init,
      };
    ol_classic_type = g_type_register_static (GTK_TYPE_WIDGET, "OlClassicWindow",
                                          &ol_classic_info, 0);
  }
  return ol_classic_type;
}


GtkWidget*
ol_classic_window_new ()
{
  printf ("new\n");
  OlClassicWindow *classic;
  classic = g_object_new (ol_classic_window_get_type (), NULL);
  return GTK_WIDGET (classic);
}

static void
ol_classic_window_init (OlClassicWindow *self)
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
ol_classic_window_class_init (OlClassicWindowClass *klass)
{
  printf("class_init\n");
  GtkObjectClass *object_class;
  GtkWidgetClass *widget_class;

  object_class = (GtkObjectClass*) klass;
  widget_class = (GtkWidgetClass*) klass;
  parent_class = g_type_class_peek_parent (klass);
  // parent_class = gtk_type_class (klass);

  object_class->destroy = ol_classic_window_destroy;

  widget_class->realize = ol_classic_window_realize;
  widget_class->unrealize = ol_classic_window_unrealize;
  widget_class->size_allocate = ol_classic_window_size_allocate;
  widget_class->size_request = ol_classic_window_size_request;
  widget_class->map = ol_classic_window_map;
  widget_class->unmap = ol_classic_window_unmap;
  widget_class->show = ol_classic_window_show;
  widget_class->expose_event = ol_classic_window_expose;
}

static void
ol_classic_window_destroy (GtkObject *object)
{
  OlClassicWindow *classic = OL_CLASSIC_WINDOW (object);
  if (classic->current_lyric_id!= -1)
  {
    //g_free (classic->lyric);
    classic->current_lyric_id = -1;
  }
  GTK_OBJECT_CLASS (parent_class)->destroy (object);
}

static void
ol_classic_window_realize (GtkWidget *widget)
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
ol_classic_window_unrealize (GtkWidget *widget)
{
  printf ("unrealize\n");
  GTK_WIDGET_CLASS(parent_class)->unrealize (widget);
}


static void
ol_classic_window_map (GtkWidget *widget)
{
  fprintf (stderr, "%s\n", __FUNCTION__);
  GTK_WIDGET_CLASS (parent_class)->map (widget);
  
  if (GTK_WIDGET_MAPPED (widget))
    return;
  GTK_WIDGET_SET_FLAGS (widget, GTK_MAPPED);
}

static void
ol_classic_window_unmap (GtkWidget *widget)
{
  printf ("unmap\n");
  GTK_WIDGET_UNSET_FLAGS (widget, GTK_MAPPED);
}


static void
ol_classic_window_size_allocate (GtkWidget *widget, GtkAllocation *allocation)
{
  printf ("%s\n"
          "(%d, %d) - %d x %d\n",
          __FUNCTION__,
          allocation->x,
          allocation->y,
          allocation->width,
          allocation->height);

  g_return_if_fail(widget != NULL);
  g_return_if_fail(OL_IS_CLASSIC_WINDOW(widget));
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
ol_classic_window_size_request (GtkWidget *widget, GtkRequisition *requisition)
{
  g_return_if_fail(widget != NULL);
  g_return_if_fail(OL_IS_CLASSIC_WINDOW(widget));
  g_return_if_fail(requisition != NULL);

  requisition->width = 1024;
  requisition->height =600;
}

static void
ol_classic_window_show (GtkWidget *widget)
{
  printf ("show\n");
  GTK_WIDGET_SET_FLAGS (widget, GTK_VISIBLE);
  gtk_widget_map (widget);
}

static gboolean
ol_classic_window_expose (GtkWidget *widget, GdkEventExpose *event)
{
  /* fprintf (stderr, "%s\n", __FUNCTION__); */
  ol_classic_window_paint (OL_CLASSIC_WINDOW (widget));
  return FALSE;
}

static void
ol_classic_window_paint (OlClassicWindow *classic)
{
  fprintf(stderr,"apple:%lf",classic->percentage);
  g_return_if_fail (OL_IS_CLASSIC_WINDOW (classic));
  GtkWidget *widget = GTK_WIDGET (classic);
  g_return_if_fail (GTK_WIDGET_REALIZED (widget));
  if (!GTK_WIDGET_VISIBLE (widget))
    return;
  cairo_t *cr;
  cr = gdk_cairo_create (widget->window);
  ol_classic_window_paint_lyrics (classic, cr);
  //cairo_destroy (cr); 
}

static void
ol_classic_window_paint_lyrics (OlClassicWindow *classic, cairo_t *cr)
{
  g_return_if_fail (classic != NULL);
  GtkWidget *widget = GTK_WIDGET (classic);
  if (!GTK_WIDGET_REALIZED (widget))
    gtk_widget_realize (widget);
  
  gdouble ypos = 100, xpos = 100;
  double percentage = classic->percentage;

  cairo_set_source_rgb(cr, 1, 0.5, 1);
  cairo_rectangle(cr, 100, 0, 400, 400);
  cairo_fill(cr);
  /*clip代码*/
  cairo_set_source_rgb(cr, 0, 1, 0.5);
  cairo_rectangle(cr,100,20,400,180);
  cairo_clip (cr);
  
  /* paint the lyric */
   PangoLayout *layout;
   PangoFontDescription *desc;
   int font_height;
   int width, height,i,y=20;
   layout = pango_cairo_create_layout (cr);
   cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);

   if (classic->paint_lyrics !=NULL)
   {
     for (i=0; i<9; i++)
     {
       pango_layout_set_text (layout, classic->paint_lyrics[i], -1);
       desc = pango_font_description_from_string (FONT);
       pango_layout_set_font_description (layout, desc);
       pango_font_description_free (desc);
       pango_layout_get_pixel_size(layout,&width,&height);

       cairo_save (cr);
       if (i == 4)
       {
         cairo_set_source_rgb(cr, 0.1, 0.5, 0.1);
         /* fprintf (stderr, "per:%lf, y:%d\n", percentage, (int)(50+y*(1-percentage)+i*20)); */
       }
       if (i == 7&&percentage>0.60)
       {
         cairo_set_source_rgb(cr, 0.1, 0.5, 0.1);
       }
       cairo_move_to (cr, xpos,y*(1-percentage)+i*20);

       pango_cairo_update_layout (cr, layout);
       pango_cairo_show_layout (cr, layout);
       cairo_restore (cr);
       cairo_set_source_rgb(cr, 0.1, 0.1, 0.1);
     }
     g_object_unref (layout);
   

   }
}

  

void
ol_classic_window_set_lyric (OlClassicWindow *classic, const LrcInfo *lyric)
{

  g_return_if_fail (OL_IS_CLASSIC_WINDOW (classic));
  if (lyric != NULL)
    classic->current_lyric_id = ol_lrc_parser_get_lyric_id (lyric);
  else
    classic->current_lyric_id = -1;
  ol_classic_window_set_paint_lyrics (classic);
  ol_classic_window_paint(classic);

}


void
ol_classic_window_set_current_percentage (OlClassicWindow *classic, double percentage)
{
  /*   fprintf (stderr, "%s:%lf\n", __FUNCTION__, percentage); */
  ol_classic_window_set_percentage (classic, percentage);
}


void
ol_classic_window_set_percentage (OlClassicWindow *classic, double percentage)
{
  /*   fprintf (stderr, "%s:%lf\n", __FUNCTION__, percentage); */
  g_return_if_fail (OL_IS_CLASSIC_WINDOW (classic));
  classic->percentage = percentage;
  ol_classic_window_paint (classic);
}

static void
ol_classic_window_set_paint_lyrics (OlClassicWindow *classic)
{
  fprintf(stderr,"lyric_id:%d",classic->current_lyric_id);
  int id = classic->current_lyric_id;
  LrcInfo *info;
  if (id < 0)
  {
    int i;
    for (i = 0; i<9; i++)
      classic->paint_lyrics[i] = "";
  }
  else if (5+id < 9)
  {
    int i;
    for (i = 4;i<9;i++)
    {
      info = ol_lrc_parser_get_lyric_by_id (classic->lrc_file, (id+i-4));
      classic->paint_lyrics[i] = ol_lrc_parser_get_lyric_text (info);
    }
    for (i = 4-1;i>=0;i--)
    {
      if (id+i-4>=0)
      {
        info = ol_lrc_parser_get_lyric_by_id (classic->lrc_file, (id+i-4));
        classic->paint_lyrics[i] = ol_lrc_parser_get_lyric_text (info);
      }
    }
  }
  else if(5+id>=9)
  {
    int i = 0;
    for (i = 0; i < 9; i++)
    {
      info = ol_lrc_parser_get_lyric_by_id (classic->lrc_file, (id-4+i));
      classic->paint_lyrics[i] = ol_lrc_parser_get_lyric_text (info);
    }
  }
  else if(id>ol_lrc_parser_get_most_id_of_list(classic->lrc_file)-4&&id<ol_lrc_parser_get_most_id_of_list(classic->lrc_file))
  {
    int i = 0;
    for (i = 0; i < 9; i++)
    {
      if(id-4+i <= ol_lrc_parser_get_most_id_of_list(classic->lrc_file))
      {
        info = ol_lrc_parser_get_lyric_by_id (classic->lrc_file, (id-4+i));
        classic->paint_lyrics[i] = ol_lrc_parser_get_lyric_text (info);
      }
    }
  }
}
