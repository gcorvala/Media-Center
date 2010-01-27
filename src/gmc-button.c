#include "gmc-button.h"

G_DEFINE_TYPE (GmcButton, gmc_button, CLUTTER_TYPE_ACTOR);

#define GMC_BUTTON_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_BUTTON, GmcButtonPrivate))

struct _GmcButtonPrivate
{
  ClutterActor *icon;
  ClutterActor *text;

  gboolean is_pressed;
};

enum {
  PROP_0,
  PROP_TEXT,
  PROP_ICON
};

enum
{
  CLICKED,
  LAST_SIGNAL
};

static guint button_signals[LAST_SIGNAL] = { 0, };

void
gmc_button_set_text (GmcButton *self, const gchar *text)
{
  GmcButtonPrivate *priv;
  ClutterColor *color;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  // TODO check if priv->text->text == text then return
  if (priv->text) {
    g_object_unref (priv->text);
  }

  color = clutter_color_new (0x80, 0x80, 0x80, 0xff);
  priv->text = clutter_text_new_full ("Comic Sans MS 12", text, color);
  clutter_actor_set_parent (priv->text, CLUTTER_ACTOR (self));
  clutter_color_free (color);
}

const gchar *
gmc_button_get_text (GmcButton *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  return clutter_text_get_text (CLUTTER_TEXT (priv->text));
}

void
gmc_button_set_icon (GmcButton *self, ClutterActor *icon)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  if (priv->icon) {
    g_object_unref (priv->icon);
  }

  // TODO make a copy or ref
  priv->icon = icon;
  clutter_actor_set_parent (priv->icon, CLUTTER_ACTOR (self));
}

static void
gmc_button_set_property (GObject      *object,
                         guint         prop_id,
                         const GValue *value,
                         GParamSpec   *pspec)
{
  GmcButton *self;

  self = GMC_BUTTON (object);

  switch (prop_id)
  {
    case PROP_TEXT:
      gmc_button_set_text (self, g_value_get_string (value));
      break;

    case PROP_ICON:
      gmc_button_set_icon (self, CLUTTER_ACTOR (g_value_get_object (value)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gmc_button_get_property (GObject    *object,
                         guint       prop_id,
                         GValue     *value,
                         GParamSpec *pspec)
{
  GmcButton *self;

  self = GMC_BUTTON (object);

  switch (prop_id)
  {
    case PROP_TEXT:
      g_value_set_string (value, gmc_button_get_text (self));
      break;

    /*case PROP_COLUMN:
      g_value_set_uint (value, priv->column);
      break;*/

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

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

  g_signal_emit (self, button_signals[CLICKED], 0, self);
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
  *min_height_p = 25;
  *natural_height_p = 25;
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

  if (priv->icon) {
    actor_box = clutter_actor_box_new (0, 0, 24, 25);
    clutter_actor_allocate (priv->icon, actor_box, flags);
    clutter_actor_box_free (actor_box);
  }
  if (priv->text) {
    actor_box = clutter_actor_box_new (26, 0, width, 25);
    clutter_actor_allocate (priv->text, actor_box, flags);
    clutter_actor_box_free (actor_box);
  }
}

static void
gmc_button_paint (ClutterActor *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

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

  object_class->set_property = gmc_button_set_property;
  object_class->get_property = gmc_button_get_property;
  /*object_class->dispose
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

  g_object_class_install_property (object_class,
                                   PROP_TEXT,
                                   g_param_spec_string ("text",
                                                      "Text",
                                                      "The button text",
                                                      "Default",
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));

  g_object_class_install_property (object_class,
                                   PROP_ICON,
                                   g_param_spec_object ("icon",
                                                        "Icon",
                                                        "The button icon",
                                                        CLUTTER_TYPE_ACTOR,
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE));

  button_signals[CLICKED] = g_signal_new ("clicked",
                                          G_TYPE_FROM_CLASS (klass),
                                          G_SIGNAL_RUN_LAST,
                                          G_STRUCT_OFFSET (GmcButtonClass, clicked),
                                          NULL, NULL,
                                          g_cclosure_marshal_VOID__OBJECT,
                                          G_TYPE_NONE, 1,
                                          GMC_TYPE_BUTTON);
}

static void
gmc_button_init (GmcButton *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  priv->is_pressed = FALSE;
}

ClutterActor *
gmc_button_new (const gchar *text)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_BUTTON,
                       "text", text,
                       NULL);

  return CLUTTER_ACTOR (self);
}

ClutterActor *
gmc_button_new_with_icon (const gchar *text, ClutterActor *icon)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_BUTTON,
                       "text", text,
                       "icon", icon,
                       NULL);

  return CLUTTER_ACTOR (self);
}
