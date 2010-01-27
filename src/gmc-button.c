#include "gmc-button.h"

G_DEFINE_TYPE (GmcButton, gmc_button, CLUTTER_TYPE_ACTOR);

#define GMC_BUTTON_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_BUTTON, GmcButtonPrivate))

struct _GmcButtonPrivate
{
  ClutterActor *label;
  ClutterActor *icon;

  gboolean is_pressed;
  gboolean is_hover;

  gfloat spacing;
};

enum {
  PROP_0,
  PROP_LABEL,
  PROP_ICON
};

enum
{
  CLICKED,
  LAST_SIGNAL
};

static guint button_signals[LAST_SIGNAL] = { 0, };

void
gmc_button_set_label (GmcButton *self, const gchar *label)
{
  GmcButtonPrivate *priv;
  ClutterColor *color;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  if (priv->label) {
    if (g_strcmp0 (label, clutter_text_get_text (CLUTTER_TEXT (priv->label))) == 0)
      return;
    clutter_text_set_text (CLUTTER_TEXT (priv->label), label);
    return;
  }

  color = clutter_color_new (0x80, 0x80, 0x80, 0xff);
  priv->label = clutter_text_new_full ("Comic Sans MS 12", label, color);
  clutter_actor_set_parent (priv->label, CLUTTER_ACTOR (self));
  clutter_color_free (color);
}

const gchar *
gmc_button_get_label (GmcButton *self)
{
  GmcButtonPrivate *priv;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  if (priv->label) {
    return clutter_text_get_text (CLUTTER_TEXT (priv->label));
  }
  else {
    return NULL;
  }
}

void
gmc_button_set_icon (GmcButton *self, const gchar *filename)
{
  GmcButtonPrivate *priv;
  ClutterActor *icon;
  GError *error = NULL;

  priv = GMC_BUTTON_GET_PRIVATE (self);

  icon = clutter_texture_new_from_file (filename, &error);
  if (!icon) {
    g_critical ("%s - %s", G_STRFUNC, error->message);
    return;
  }

  if (priv->icon) {
    g_object_unref (priv->icon);
  }

  priv->icon = icon;
  clutter_actor_set_size (priv->icon, 48, 48);
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
    case PROP_LABEL:
      gmc_button_set_label (self, g_value_get_string (value));
      break;

    case PROP_ICON:
      gmc_button_set_icon (self, g_value_get_string (value));
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
    case PROP_LABEL:
      g_value_set_string (value, gmc_button_get_label (self));
      break;

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

static gboolean
gmc_button_enter_event (ClutterActor *actor,
                        ClutterCrossingEvent *event)
{
  GmcButton *self;
  GmcButtonPrivate *priv;
  ClutterColor *color;

  self = GMC_BUTTON (actor);
  priv = GMC_BUTTON_GET_PRIVATE (self);

  if (priv->label) {
    color = clutter_color_new (0xff, 0xff, 0xff, 0xff);
    clutter_text_set_color (CLUTTER_TEXT (priv->label), color);
    clutter_color_free (color);
  }

  priv->is_hover = TRUE;

  return TRUE;
}

static gboolean
gmc_button_leave_event (ClutterActor *actor,
                        ClutterCrossingEvent *event)
{
  GmcButton *self;
  GmcButtonPrivate *priv;
  ClutterColor *color;

  self = GMC_BUTTON (actor);
  priv = GMC_BUTTON_GET_PRIVATE (self);

  if (priv->label) {
    color = clutter_color_new (0x80, 0x80, 0x80, 0xff);
    clutter_text_set_color (CLUTTER_TEXT (priv->label), color);
    clutter_color_free (color);
  }

  priv->is_hover = FALSE;

  return TRUE;
}

static void
gmc_button_get_preferred_width (ClutterActor *actor,
                                gfloat        for_height,
                                gfloat       *min_width_p,
                                gfloat       *natural_width_p)
{
  GmcButton *self;
  GmcButtonPrivate *priv;
  gfloat icon_w_min = 0, icon_w_nat = 0, label_w_min = 0, label_w_nat = 0;

  self = GMC_BUTTON (actor);
  priv = GMC_BUTTON_GET_PRIVATE (self);

  if (priv->icon) {
    clutter_actor_get_preferred_width (priv->icon, for_height, &icon_w_min, &icon_w_nat);
  }
  if (priv->label) {
    clutter_actor_get_preferred_width (priv->label, for_height, &label_w_min, &label_w_nat);
  }

  *min_width_p = icon_w_min + priv->spacing + label_w_min;
  *natural_width_p = icon_w_nat + priv->spacing + label_w_nat;
}

static void
gmc_button_get_preferred_height (ClutterActor *actor,
                                 gfloat        for_width,
                                 gfloat       *min_height_p,
                                 gfloat       *natural_height_p)
{
  GmcButton *self;
  GmcButtonPrivate *priv;
  gfloat icon_h_min = 0, icon_h_nat = 0, label_h_min = 0, label_h_nat = 0;

  self = GMC_BUTTON (actor);
  priv = GMC_BUTTON_GET_PRIVATE (self);

  if (priv->icon) {
    clutter_actor_get_preferred_height (priv->icon, for_width, &icon_h_min, &icon_h_nat);
  }
  if (priv->label) {
    clutter_actor_get_preferred_height (priv->label, for_width, &label_h_min, &label_h_nat);
  }

  *min_height_p = MAX (icon_h_min, label_h_min);
  *natural_height_p = MAX (icon_h_nat, label_h_nat);
}

static void
gmc_button_allocate (ClutterActor           *actor,
                    const ClutterActorBox  *box,
                    ClutterAllocationFlags  flags)
{
  GmcButtonPrivate *priv;
  ClutterActorBox *actor_box;
  gfloat width, height;
  gfloat icon_w_min = 0, icon_h_min = 0, icon_w_nat = 0, icon_h_nat = 0;
  gfloat label_w_min = 0, label_h_min = 0, label_w_nat = 0, label_h_nat = 0;
  gfloat spacing = 0;

  priv = GMC_BUTTON_GET_PRIVATE (actor);
  CLUTTER_ACTOR_CLASS (gmc_button_parent_class)->allocate (actor, box, flags);

  clutter_actor_box_get_size (box,
                              &width,
                              &height);

  // TODO if icon_h < label_h, center icon
  //      if icon_h > label_h, center label
  if (priv->icon) {
    clutter_actor_get_preferred_width (priv->icon, height, &icon_w_min, &icon_w_nat);
    clutter_actor_get_preferred_height (priv->icon, width, &icon_h_min, &icon_h_nat);
    actor_box = clutter_actor_box_new (0, 0, icon_w_min, icon_h_min);
    clutter_actor_allocate (priv->icon, actor_box, flags);
    clutter_actor_box_free (actor_box);
    spacing = priv->spacing;
  }
  if (priv->label) {
    clutter_actor_get_preferred_width (priv->label, height, &label_w_min, &label_w_nat);
    clutter_actor_get_preferred_height (priv->label, width, &label_h_min, &label_h_nat);
    actor_box = clutter_actor_box_new (icon_w_min + spacing, 0, label_w_min, 25);
    clutter_actor_allocate (priv->label, actor_box, flags);
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
  if (priv->label) {
    clutter_actor_paint (priv->label);
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
  if (priv->label) {
    clutter_actor_paint (priv->label);
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
  if (priv->label) {
    clutter_actor_map (priv->label);
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
  if (priv->label) {
    clutter_actor_unmap (priv->label);
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
  actor_class->key_release_event*/
  actor_class->enter_event = gmc_button_enter_event;
  actor_class->leave_event = gmc_button_leave_event;

  actor_class->get_preferred_width = gmc_button_get_preferred_width;
  actor_class->get_preferred_height = gmc_button_get_preferred_height;
  actor_class->allocate = gmc_button_allocate;
  actor_class->paint = gmc_button_paint;
  actor_class->pick = gmc_button_pick;
  actor_class->map = gmc_button_map;
  actor_class->unmap = gmc_button_unmap;

  g_object_class_install_property (object_class,
                                   PROP_LABEL,
                                   g_param_spec_string ("label",
                                                      "Label",
                                                      "The button label",
                                                      NULL,
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE));

  g_object_class_install_property (object_class,
                                   PROP_ICON,
                                   g_param_spec_string ("icon",
                                                        "Icon",
                                                        "The button icon",
                                                        NULL,
                                                        G_PARAM_WRITABLE));

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
  priv->is_hover = FALSE;

  priv->spacing = 10;
}

ClutterActor *
gmc_button_new (void)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_BUTTON, NULL);

  return CLUTTER_ACTOR (self);
}

ClutterActor *
gmc_button_new_with_label (const gchar *label)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_BUTTON,
                       "label", label,
                       NULL);

  return CLUTTER_ACTOR (self);
}

ClutterActor *
gmc_button_new_with_icon (const gchar *filename)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_BUTTON,
                       "icon", filename,
                       NULL);

  return CLUTTER_ACTOR (self);
}

ClutterActor *
gmc_button_new_full (const gchar *label, const gchar *filename)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_BUTTON,
                       "label", label,
                       "icon", filename,
                       NULL);

  return CLUTTER_ACTOR (self);
}
