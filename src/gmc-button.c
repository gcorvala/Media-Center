#include "gmc-button.h"

G_DEFINE_TYPE (GmcButton, gmc_button, CLUTTER_TYPE_RECTANGLE);

#define GMC_BUTTON_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_BUTTON, GmcButtonPrivate))

struct _GmcButtonPrivate
{
  ClutterActor *icon;
  ClutterActor *text;

  gboolean is_pressed;
};

enum {
  PROP_0,
  PROP_ICON,
  PROP_NAME
};

enum
{
  CLICKED,
  LAST_SIGNAL
};

static guint button_signals[LAST_SIGNAL] = { 0, };

static void
gmc_button_push (GmcButton *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  priv->is_pressed = TRUE;

  clutter_grab_pointer (CLUTTER_ACTOR (self));
}

static void
gmc_button_pull (GmcButton *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  if (priv->is_pressed == FALSE) {
    return;
  }

  clutter_ungrab_pointer ();

  priv->is_pressed = FALSE;

  g_signal_emit (self, button_signals[CLICKED], 0);
}

static gboolean
gmc_button_press_event (ClutterActor       *self,
                        ClutterButtonEvent *event)
{
  if (event->button == 1) {
    gmc_button_push (GMC_BUTTON (self));
  }

  return TRUE;
}

static gboolean
gmc_button_release_event (ClutterActor       *self,
                          ClutterButtonEvent *event)
{
  if (event->button == 1) {
    gmc_button_pull (GMC_BUTTON (self));
  }

  return TRUE;
}

static void
gmc_button_get_preferred_width (ClutterActor *actor,
                                gfloat        for_height,
                                gfloat       *min_width_p,
                                gfloat       *natural_width_p)
{
  *min_width_p = 100;
  *natural_width_p = 100;
}

static void
gmc_button_get_preferred_height (ClutterActor *actor,
                                 gfloat        for_width,
                                 gfloat       *min_height_p,
                                 gfloat       *natural_height_p)
{
  *min_height_p = 100;
  *natural_height_p = 100;
}

static void
gmc_button_allocate (ClutterActor           *actor,
                    const ClutterActorBox  *box,
                    ClutterAllocationFlags  flags)
{
  GmcButtonPrivate *priv;
  ClutterActorBox *actor_box;
  gfloat width, height;

  priv = GMC_BUTTON_GET_PRIVATE (actor);
  CLUTTER_ACTOR_CLASS (gmc_button_parent_class)->allocate (actor, box, flags);

  clutter_actor_box_get_size (box,
                              &width,
                              &height);

  actor_box = clutter_actor_box_new (0, 0, 100, 100);
  clutter_actor_allocate (priv->icon, actor_box, flags);
  actor_box = clutter_actor_box_new (0, 0, 100, 100);
  clutter_actor_allocate (priv->text, actor_box, flags);
}

static void
gmc_button_paint (ClutterActor *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  CLUTTER_ACTOR_CLASS (gmc_button_parent_class)->paint (self);

  if (priv->icon) {
    clutter_actor_paint (priv->icon);
  }
  if (priv->text) {
    clutter_actor_paint (priv->text);
  }
}

static void
gmc_button_pick (ClutterActor       *self,
                 const ClutterColor *pick_color)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  CLUTTER_ACTOR_CLASS (gmc_button_parent_class)->pick (self, pick_color);

  if (priv->icon) {
    clutter_actor_paint (priv->icon);
  }
  if (priv->text) {
    clutter_actor_paint (priv->text);
  }
}

static void
gmc_button_map (ClutterActor *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  CLUTTER_ACTOR_CLASS (gmc_button_parent_class)->map (self);

  if (priv->icon) {
    clutter_actor_map (priv->icon);
  }
  if (priv->text) {
    clutter_actor_map (priv->text);
  }
}

static void
gmc_button_unmap (ClutterActor *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  CLUTTER_ACTOR_CLASS (gmc_button_parent_class)->unmap (self);

  if (priv->icon) {
    clutter_actor_unmap (priv->icon);
  }
  if (priv->text) {
    clutter_actor_unmap (priv->text);
  }
}
static void
gmc_button_class_init (GmcButtonClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  g_type_class_add_private (object_class, sizeof (GmcButtonPrivate));

  /*object_class->set_property
  object_class->get_property
  object_class->dispose
  object_class->finalize*/

  actor_class->button_press_event = gmc_button_press_event;
  actor_class->button_release_event = gmc_button_release_event;
  /*actor_class->key_press_event
  actor_class->key_release_event
  actor_class->enter_event
  actor_class->leave_event*/

  actor_class->get_preferred_width = gmc_button_get_preferred_width;
  actor_class->get_preferred_height = gmc_button_get_preferred_height;
  actor_class->allocate = gmc_button_allocate;
  actor_class->paint = gmc_button_paint;
  actor_class->pick = gmc_button_pick;
  actor_class->map = gmc_button_map;
  actor_class->unmap = gmc_button_unmap;

  button_signals[CLICKED] = g_signal_new ("clicked",
                                          G_TYPE_FROM_CLASS (klass),
                                          G_SIGNAL_RUN_LAST,
                                          G_STRUCT_OFFSET (GmcButtonClass, clicked),
                                          NULL, NULL,
                                          g_cclosure_marshal_VOID__VOID,
                                          G_TYPE_NONE, 0);
}

static void
gmc_button_init (GmcButton *self)
{
  GmcButtonPrivate *priv;
  ClutterColor *color;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  priv->text = clutter_text_new ();
  clutter_text_set_text (CLUTTER_TEXT (priv->text), "Hello World !");
  clutter_actor_set_parent (priv->text, CLUTTER_ACTOR (self));

  color = clutter_color_new (0x80, 0x80, 0x80, 0xff);
  priv->icon = clutter_rectangle_new_with_color (color);
  clutter_actor_set_size (priv->icon, 100, 100);
  clutter_actor_set_parent (priv->icon, CLUTTER_ACTOR (self));

  clutter_actor_show (priv->icon);
}

ClutterActor *
gmc_button_new (gchar *name)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_BUTTON,
                       NULL);

  return CLUTTER_ACTOR (self);
}
