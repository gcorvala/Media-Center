#include "gmc-carousel.h"

G_DEFINE_TYPE (GmcCarousel, gmc_carousel, CLUTTER_TYPE_GROUP);

#define GMC_CAROUSEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_CAROUSEL, GmcCarouselPrivate))

typedef struct Element
{
  ClutterActor *actor;
  ClutterBehaviour *ellipse;
  ClutterBehaviour *opacity;
  ClutterBehaviour *depth;
} Element;

struct _GmcCarouselPrivate
{
  GList *elements;
  ClutterAlpha *alpha;
};

static void
gmc_carousel_class_init (GmcCarouselClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (gobject_class, sizeof (GmcCarouselPrivate));
}

void
actor_added_cb (GmcCarousel  *carousel,
                ClutterActor *actor)
{
  GmcCarouselPrivate *priv;
  gfloat width, height;
  gdouble angle_start, angle_end;
  ClutterTimeline *timeline;
  Element *elem, *curr;
  guint nb;
  GList *current;
  guint tmp, step;

  priv = GMC_CAROUSEL_GET_PRIVATE (carousel);

  nb = g_list_length (priv->elements);
  clutter_actor_get_size (CLUTTER_ACTOR (carousel), &width, &height);
  timeline = clutter_alpha_get_timeline (priv->alpha);

  g_debug ("[%s] nb : %u - w : %f - h : %f", G_STRFUNC, nb, width, height);

  elem = g_new0 (Element, 1);
  elem->actor = actor;
  elem->ellipse = clutter_behaviour_ellipse_new (priv->alpha,
                                                 width / 2, height / 2,
                                                 width / 2, height / 2,
                                                 CLUTTER_ROTATE_CW,
                                                 0, 0);
  clutter_behaviour_apply (elem->ellipse, elem->actor);
  clutter_actor_set_position (elem->actor, width, height / 2);

  step = 360 / (nb + 1);
/*  if (nb != 0) {
    tmp = 0;
    angle_start = 90;
    for (current = priv->elements->next; current != NULL && current->next != priv->elements; current = current->next) {
      angle_start = angle_start + tmp * (360 / (nb + 1));
      angle_end = angle_start + (360 / (nb + 1));
      g_debug ("angle_start : %f - angle_end : %f", angle_start, angle_end);
      curr = (Element *) current->data;
      clutter_behaviour_ellipse_set_angle_start (CLUTTER_BEHAVIOUR_ELLIPSE (curr->ellipse), angle_start);
      clutter_behaviour_ellipse_set_angle_end (CLUTTER_BEHAVIOUR_ELLIPSE (curr->ellipse), angle_end);
      ++tmp;
      // g_debug ("%d", tmp);
    }
    clutter_timeline_start (timeline);
  }
*/
  priv->elements = g_list_prepend (priv->elements, elem);
//  elem->opacity
//  elem->depth
}

static void
gmc_carousel_init (GmcCarousel *self)
{
  GmcCarouselPrivate *priv;
  ClutterTimeline *timeline;
  GError *error = NULL;
  Element *element;

  priv = GMC_CAROUSEL_GET_PRIVATE (self);

  priv->elements = NULL;
  timeline = clutter_timeline_new (3000);
  priv->alpha = clutter_alpha_new_full (timeline, CLUTTER_EASE_OUT_SINE);

  g_signal_connect (CLUTTER_GROUP (self), "actor-added", G_CALLBACK (actor_added_cb), NULL);

/*  element = g_new (Element, 1);

  element->actor = clutter_texture_new_from_file ("../data/accessories-text-editor.png", &error);
  if (element->actor == NULL)
	  g_critical ("-- %s --", G_STRFUNC);
  clutter_container_add (CLUTTER_CONTAINER (self), element->actor, NULL);
  g_list_append (priv->actors, element);

  priv->ellipse = clutter_behaviour_ellipse_new (priv->alpha, 0, 0, 300, 300, CLUTTER_ROTATE_CW, 0, 90);
  priv->depth = clutter_behaviour_depth_new (priv->alpha, 0, -200);
  priv->opacity = clutter_behaviour_opacity_new (priv->alpha, 0xff, 0x20);
  priv->rotation = clutter_behaviour_rotate_new (priv->alpha, CLUTTER_Y_AXIS, CLUTTER_ROTATE_CW, 0, 90);

  clutter_behaviour_apply (priv->ellipse, icon);
  clutter_behaviour_apply (priv->depth, icon);
  clutter_behaviour_apply (priv->opacity, icon);
  //clutter_behaviour_apply (priv->rotation, icon);
  clutter_timeline_start (priv->timeline);*/
}

ClutterActor *
gmc_carousel_new (void)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_CAROUSEL,
                       NULL);

  return CLUTTER_ACTOR (self);
}
