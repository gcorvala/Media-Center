#include "gmc-button-play.h"

G_DEFINE_TYPE (GMCButtonPlay, gmc_button_play, CLUTTER_TYPE_CAIRO_TEXTURE)

#define GET_PRIVATE(o) \
  (G_TYPE_INSTANCE_GET_PRIVATE ((o), GMC_TYPE_BUTTON_PLAY, GMCButtonPlayPrivate))

typedef struct _GMCButtonPlayPrivate GMCButtonPlayPrivate;

struct _GMCButtonPlayPrivate {
    int dummy;
};

static void
gmc_button_play_class_init (GMCButtonPlayClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (klass, sizeof (GMCButtonPlayPrivate));
}

static void
gmc_button_play_init (GMCButtonPlay *self)
{
	cairo_t *cr;
	gint x = 5, y = 5, w = 38, h = 38, r =  10;

	clutter_cairo_texture_set_surface_size (CLUTTER_CAIRO_TEXTURE (self), 48, 48);

	cr = clutter_cairo_texture_create (CLUTTER_CAIRO_TEXTURE (self));
	cairo_set_line_cap (cr, CAIRO_LINE_CAP_ROUND);
	cairo_set_line_join (cr, CAIRO_LINE_JOIN_ROUND);
	cairo_move_to (cr, x+r,y);
		cairo_line_to (cr, x+w-r,y);
		cairo_curve_to (cr, x+w,y,x+w,y,x+w,y+r);
		cairo_line_to (cr, x+w,y+h-r);
		cairo_curve_to (cr, x+w,y+h,x+w,y+h,x+w-r,y+h);
		cairo_line_to (cr, x+r,y+h);
		cairo_curve_to (cr, x,y+h,x,y+h,x,y+h-r);
		cairo_line_to (cr, x,y+r);
		cairo_curve_to (cr, x,y,x,y,x+r,y);
	cairo_fill (cr);
	cairo_destroy (cr);
}

ClutterActor*
gmc_button_play_new (void)
{
  return g_object_new (GMC_TYPE_BUTTON_PLAY,
 					   "surface-height", 48,
					   "surface-width", 48,
					   NULL);
}
