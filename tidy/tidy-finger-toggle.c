/* tidy-finger-toggle.c: Finger toggle button
 *
 * Copyright (C) 2008  OpenedHand
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * Written by: Emmanuele Bassi <ebassi@openedhand.com>
 */

/**
 * SECTION:tidy-finger-toggle
 * @short_description: Finger toggle button
 *
 * #TidyFingerToggle is a toggle actor which can have two states, active
 * and not active, and switch between the two.
 *
 * #TidyFingerToggle is an actor optimized for the touchscreen and finger
 * operated usage: the toggle handle is draggable, with a "grace area" for
 * the switch which will not complete the toggle action and keep the
 * current state.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <clutter/clutter.h>
#include <cogl/cogl.h>

#include "tidy-finger-toggle.h"
#include "tidy-adjustment.h"
#include "tidy-marshal.h"
#include "tidy-stylable.h"
#include "tidy-frame.h"
#include "tidy-private.h"

struct _TidyFingerTogglePrivate
{
  ClutterEffectTemplate *tmpl;

  ClutterUnit x_origin;
  ClutterUnit last_x;

  ClutterActor *handle;

  guint is_active : 1;
};

enum
{
  PROP_0,

  PROP_HANDLE,
  PROP_ACTIVE
};

enum
{
  TOGGLED,

  LAST_SIGNAL
};

static ClutterColor default_handle_color = { 0xf0, 0xb3, 0x78, 0xff };

static guint toggle_signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (TidyFingerToggle, tidy_finger_toggle, TIDY_TYPE_FRAME);

static void
toggle_active (ClutterActor *actor,
               gpointer      user_data)
{
  TidyFingerToggle *toggle = user_data;

  toggle->priv->is_active = !toggle->priv->is_active;

  g_signal_emit (toggle, toggle_signals[TOGGLED], 0);
}

static void
move_slider (TidyFingerToggle *toggle,
             gint              x,
             gint              y)
{
  TidyFingerTogglePrivate *priv = toggle->priv;
  ClutterActor *actor = CLUTTER_ACTOR (toggle);
  ClutterUnit ux, width;

  if (!clutter_actor_transform_stage_point (actor,
                                            CLUTTER_UNITS_FROM_DEVICE (x),
                                            CLUTTER_UNITS_FROM_DEVICE (y),
                                            &ux, NULL))
    return;
  
  width = clutter_actor_get_widthu (actor)
        - clutter_actor_get_widthu (priv->handle);
  
  if (width == 0)
    return;
  
  ux -= priv->x_origin;
  ux = CLAMP (ux, 0, width);

  clutter_actor_set_positionu (priv->handle, ux, 0);
}

static gboolean
handle_leave (TidyFingerToggle     *toggle,
              ClutterCrossingEvent *event,
              gpointer              user_data)
{
#if 0
  TidyFingerTogglePrivate *priv = toggle->priv;
  ClutterUnit toggle_w, handle_w;
  ClutterUnit toggle_x, handle_x;
  ClutterUnit pos;
  ClutterEffectCompleteFunc effect_func;
#endif
  clutter_ungrab_pointer ();
#if 0
  g_signal_handlers_disconnect_by_func (toggle, handle_motion, NULL);
  g_signal_handlers_disconnect_by_func (toggle, handle_button_release, NULL);
  g_signal_handlers_disconnect_by_func (toggle, handle_leave, NULL);

  toggle_x = clutter_actor_get_xu (CLUTTER_ACTOR (toggle));
  toggle_w = clutter_actor_get_widthu (CLUTTER_ACTOR (toggle));
  handle_x = clutter_actor_get_xu (priv->handle);
  handle_w = clutter_actor_get_widthu (priv->handle);

  effect_func = toggle_active;

  if (priv->is_active)
    {
      if (handle_x < (toggle_w / 5))
        pos = 0;
      else
        {
          pos = CLUTTER_UNITS_TO_DEVICE (toggle_w - handle_w);
          effect_func = NULL;
        }
    }
  else
    {
      if (handle_x > (toggle_w / 5 * 4))
        pos = CLUTTER_UNITS_TO_DEVICE (toggle_w - handle_w);
      else
        {
          pos = 0;
          effect_func = NULL;
        }
    }

  clutter_effect_move (priv->tmpl, priv->handle,
                       pos, 0,
                       effect_func, toggle);
#endif
  return TRUE;
}

static gboolean
handle_motion (TidyFingerToggle   *toggle,
               ClutterMotionEvent *event,
               gpointer            user_data)
{
  move_slider (toggle, event->x, event->y);

  return TRUE;
}

static gboolean
handle_button_release (TidyFingerToggle   *toggle,
                       ClutterButtonEvent *event,
                       gpointer            user_data)
{
  TidyFingerTogglePrivate *priv = toggle->priv;
  ClutterUnit event_x, event_y;
  ClutterUnit handle_x;
  ClutterUnit toggle_w, handle_w;
  ClutterUnit ux;
  ClutterEffectCompleteFunc effect_func;
  gint pos = 0;

  if (event->button != 1)
    return FALSE;

  clutter_ungrab_pointer ();

  event_x = CLUTTER_UNITS_FROM_DEVICE (event->x);
  event_y = CLUTTER_UNITS_FROM_DEVICE (event->y);
  if (!clutter_actor_transform_stage_point (CLUTTER_ACTOR (toggle),
                                            event_x, event_y,
                                            &ux, NULL))
    return FALSE;

  if (priv->x_origin == ux)
    return FALSE;

  g_signal_handlers_disconnect_by_func (toggle, handle_motion, NULL);
  g_signal_handlers_disconnect_by_func (toggle, handle_button_release, NULL);
  g_signal_handlers_disconnect_by_func (toggle, handle_leave, NULL);

  move_slider (toggle, event->x, event->y);

  toggle_w = clutter_actor_get_widthu (CLUTTER_ACTOR (toggle));

  handle_x = clutter_actor_get_xu (priv->handle);
  handle_w = clutter_actor_get_widthu (priv->handle);

  effect_func = toggle_active;

  if (priv->is_active)
    {
      if (handle_x < (toggle_w / 5))
        pos = 0;
      else
        {
          pos = CLUTTER_UNITS_TO_DEVICE (toggle_w - handle_w);
          effect_func = NULL;
        }
    }
  else
    {
      if ((handle_x + handle_w) > (toggle_w / 5 * 4))
        pos = CLUTTER_UNITS_TO_DEVICE (toggle_w - handle_w);
      else
        {
          pos = 0;
          effect_func = NULL;
        }
    }

  clutter_effect_move (priv->tmpl, priv->handle,
                       pos, 0,
                       effect_func, toggle);

  return TRUE;
}

static gboolean
handle_button_press (ClutterActor       *actor,
                     ClutterButtonEvent *event,
                     TidyFingerToggle   *toggle)
{
  TidyFingerTogglePrivate *priv = toggle->priv;
  ClutterUnit event_x, event_y;

  if (event->button != 1)
    return FALSE;

  event_x = CLUTTER_UNITS_FROM_DEVICE (event->x);
  event_y = CLUTTER_UNITS_FROM_DEVICE (event->y);
  if (!clutter_actor_transform_stage_point (actor, event_x, event_y,
                                            &priv->x_origin, NULL))
    return FALSE;

  g_signal_connect_after (toggle, "motion-event",
                          G_CALLBACK (handle_motion), NULL);
  g_signal_connect_after (toggle, "button-release-event",
                          G_CALLBACK (handle_button_release), NULL);
  g_signal_connect_after (toggle, "leave-event",
                          G_CALLBACK (handle_leave), NULL);

  clutter_grab_pointer (CLUTTER_ACTOR (toggle));

  return TRUE;
}

static void
tidy_finger_toggle_dispose (GObject *gobject)
{
  TidyFingerToggle *toggle = TIDY_FINGER_TOGGLE (gobject);

  tidy_finger_toggle_set_handle (toggle, NULL);

  if (toggle->priv->tmpl)
    {
      g_object_unref (toggle->priv->tmpl);
      toggle->priv->tmpl = NULL;
    }

  G_OBJECT_CLASS (tidy_finger_toggle_parent_class)->dispose (gobject);
}

static void
tidy_finger_toggle_set_property (GObject      *gobject,
                                 guint         prop_id,
                                 const GValue *value,
                                 GParamSpec   *pspec)
{
  TidyFingerToggle *toggle = TIDY_FINGER_TOGGLE (gobject);

  switch (prop_id)
    {
    case PROP_HANDLE:
      tidy_finger_toggle_set_handle (toggle, g_value_get_object (value));
      break;

    case PROP_ACTIVE:
      tidy_finger_toggle_set_active (toggle, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_finger_toggle_get_property (GObject    *gobject,
                                 guint       prop_id,
                                 GValue     *value,
                                 GParamSpec *pspec)
{
  TidyFingerTogglePrivate *priv = TIDY_FINGER_TOGGLE (gobject)->priv;

  switch (prop_id)
    {
    case PROP_HANDLE:
      g_value_set_object (value, priv->handle);
      break;

    case PROP_ACTIVE:
      g_value_set_boolean (value, priv->is_active);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_finger_toggle_constructed (GObject *gobject)
{
  TidyFingerToggle *toggle = TIDY_FINGER_TOGGLE (gobject);
  TidyFingerTogglePrivate *priv = toggle->priv;

  if (!priv->handle)
    {
      ClutterColor *color;
      ClutterActor *rect;

      tidy_stylable_get (TIDY_STYLABLE (toggle), "active-color", &color, NULL);
      if (!color)
        color = &default_handle_color;

      rect = clutter_rectangle_new ();
      clutter_rectangle_set_color (CLUTTER_RECTANGLE (rect), color);

      tidy_finger_toggle_set_handle (toggle, rect);

      if (color != &default_handle_color)
        clutter_color_free (color);
    }
}

static void
tidy_finger_toggle_paint (ClutterActor *actor)
{
  TidyFingerToggle *toggle = TIDY_FINGER_TOGGLE (actor);

  CLUTTER_ACTOR_CLASS (tidy_finger_toggle_parent_class)->paint (actor);

  if (G_LIKELY (toggle->priv->handle))
    clutter_actor_paint (toggle->priv->handle);
}

static void
tidy_finger_toggle_pick (ClutterActor       *actor,
                         const ClutterColor *pick_color)
{
  TidyFingerToggle *toggle = TIDY_FINGER_TOGGLE (actor);

  CLUTTER_ACTOR_CLASS (tidy_finger_toggle_parent_class)->pick (actor, pick_color);

  if (G_LIKELY (toggle->priv->handle))
    clutter_actor_paint (toggle->priv->handle);
}

static void
tidy_finger_toggle_allocate (ClutterActor          *actor,
                             const ClutterActorBox *box,
                             gboolean               origin_changed)
{
  TidyFingerTogglePrivate *priv = TIDY_FINGER_TOGGLE (actor)->priv;
  ClutterActorClass *klass;

  klass = CLUTTER_ACTOR_CLASS (tidy_finger_toggle_parent_class);
  klass->allocate (actor, box, origin_changed);

  if (priv->handle)
    {
      ClutterActorBox handle_box = { 0, };

      handle_box.x1 = clutter_actor_get_xu (priv->handle);
      handle_box.y1 = clutter_actor_get_yu (priv->handle);
      handle_box.x2 = handle_box.x1 + ((box->x2 - box->x1) / 2);
      handle_box.y2 = handle_box.y1 + (box->y2 - box->y1);

      clutter_actor_allocate (priv->handle, &handle_box, origin_changed);
    }
}

static gboolean
tidy_finger_toggle_captured (ClutterActor *actor,
                             ClutterEvent *event)
{
  TidyFingerTogglePrivate *priv = TIDY_FINGER_TOGGLE (actor)->priv;
  gint event_x, event_y;
  ClutterUnit real_x;
  ClutterUnit toggle_w, handle_w;

  if (event->type != CLUTTER_BUTTON_PRESS)
    return FALSE;

  if (((ClutterButtonEvent *) event)->button != 1)
    return FALSE;

  clutter_event_get_coords (event, &event_x, &event_y);
  if (!clutter_actor_transform_stage_point (actor,
                                            CLUTTER_UNITS_FROM_DEVICE (event_x),
                                            CLUTTER_UNITS_FROM_DEVICE (event_y),
                                            &real_x, NULL))
    return FALSE;

  toggle_w = clutter_actor_get_widthu (actor);
  handle_w = clutter_actor_get_widthu (priv->handle);

  if (priv->is_active)
    {
      if (real_x < (toggle_w / 2))
        {
          clutter_effect_move (priv->tmpl, priv->handle,
                               0, 0,
                               toggle_active,
                               actor);

          return TRUE;
        }
    }
  else
    {
      if (real_x > (toggle_w / 2))
        {
          gint pos = CLUTTER_UNITS_TO_DEVICE (toggle_w - handle_w);
          clutter_effect_move (priv->tmpl, priv->handle,
                               pos, 0,
                               toggle_active,
                               actor);

          return TRUE;
        }
    }

  return FALSE;
}

static void
tidy_finger_toggle_class_init (TidyFingerToggleClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  g_type_class_add_private (klass, sizeof (TidyFingerTogglePrivate));

  gobject_class->dispose = tidy_finger_toggle_dispose;
  gobject_class->set_property = tidy_finger_toggle_set_property;
  gobject_class->get_property = tidy_finger_toggle_get_property;
  gobject_class->constructed = tidy_finger_toggle_constructed;

  actor_class->captured_event = tidy_finger_toggle_captured;
  actor_class->allocate = tidy_finger_toggle_allocate;
  actor_class->paint = tidy_finger_toggle_paint;
  actor_class->pick = tidy_finger_toggle_pick;

  /**
   * TidyFingerToggle:handle:
   *
   * The #ClutterActor that should be used as the toggle handle
   */
  g_object_class_install_property (gobject_class,
                                   PROP_HANDLE,
                                   g_param_spec_object ("handle",
                                                        "Handle",
                                                        "The handle actor of the toggle button",
                                                        CLUTTER_TYPE_ACTOR,
                                                        TIDY_PARAM_READWRITE));
  /**
   * TidyFingerToggle:active:
   *
   * Whether the toggle button is active.
   */
  g_object_class_install_property (gobject_class,
                                   PROP_ACTIVE,
                                   g_param_spec_boolean ("active",
                                                         "Active",
                                                         "Whether the toggle button is active",
                                                         FALSE,
                                                         TIDY_PARAM_READWRITE));

  /**
   * TidyFingerToggle::toggled:
   * @toggle: the actor that received the signal
   *
   * The ::toggled signal will be emitted each time the user changes the
   * state of @toggle. This signal will not be emitted when explicitly
   * setting the state using tidy_finger_toggle_set_active().
   */
  toggle_signals[TOGGLED] =
    g_signal_new (I_("toggled"),
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (TidyFingerToggleClass, toggled),
                  NULL, NULL,
                  _tidy_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

static void
tidy_finger_toggle_init (TidyFingerToggle *toggle)
{
  ClutterTimeline *timeline;

  toggle->priv = G_TYPE_INSTANCE_GET_PRIVATE (toggle,
                                              TIDY_TYPE_FINGER_TOGGLE,
                                              TidyFingerTogglePrivate);

  toggle->priv->is_active = FALSE;

  timeline = clutter_timeline_new_for_duration (250);
  toggle->priv->tmpl =
    clutter_effect_template_new (timeline, CLUTTER_ALPHA_RAMP_INC);

  /* let the template take care of the timeline for us */
  g_object_unref (timeline);
}

/**
 * tidy_finger_toggle_new:
 *
 * Creates a new #TidyFingerToggle actor.
 *
 * Return value: the newly created #TidyFingerToggle
 */
ClutterActor *
tidy_finger_toggle_new (void)
{
  return g_object_new (TIDY_TYPE_FINGER_TOGGLE, NULL);
}

/**
 * tidy_finger_toggle_set_handle:
 * @toggle: a #TidyFingerToggle
 * @handle: a #ClutterActor for the handle
 *
 * Sets the actor to use for the handle of the #TidyFingerToggle.
 * The passed actor will be owned by the @toggle.
 */
void
tidy_finger_toggle_set_handle (TidyFingerToggle *toggle,
                               ClutterActor     *handle)
{
  TidyFingerTogglePrivate *priv;

  g_return_if_fail (TIDY_IS_FINGER_TOGGLE (toggle));
  g_return_if_fail (handle == NULL || CLUTTER_IS_ACTOR (handle));

  priv = toggle->priv;

  if (priv->handle == handle)
    return;

  if (priv->handle)
    {
      g_signal_handlers_disconnect_by_func (priv->handle,
                                            G_CALLBACK (handle_button_press),
                                            toggle);

      clutter_actor_destroy (priv->handle);
      priv->handle = NULL;
    }

  if (handle)
    {
      g_signal_connect_after (handle, "button-press-event",
                              G_CALLBACK (handle_button_press),
                              toggle);

      clutter_actor_set_parent (handle, CLUTTER_ACTOR (toggle));
      clutter_actor_set_reactive (handle, TRUE);
      clutter_actor_show (handle);

      priv->handle = handle;
    }

  clutter_actor_queue_relayout (CLUTTER_ACTOR (toggle));

  g_object_notify (G_OBJECT (toggle), "handle");
}

/**
 * tidy_finger_toggle_get_handle:
 * @toggle: a #TidyFingerToggle
 *
 * Retrieves the #ClutterActor used as the handle of @toggle.
 *
 * Return value: the #ClutterActor used for the handle
 */
ClutterActor *
tidy_finger_toggle_get_handle (TidyFingerToggle *toggle)
{
  g_return_val_if_fail (TIDY_IS_FINGER_TOGGLE (toggle), NULL);

  return toggle->priv->handle;
}

/**
 * tidy_finger_toggle_set_active:
 * @toggle: a #TidyFingerToggle
 * @is_active: whether the toggle should be active or not
 *
 * Sets the @toggle to be active or not active. This function will
 * not emit the TidyFingerToggle::toggled signal.
 */
void
tidy_finger_toggle_set_active (TidyFingerToggle *toggle,
                               gboolean          is_active)
{
  TidyFingerTogglePrivate *priv;

  g_return_if_fail (TIDY_IS_FINGER_TOGGLE (toggle));

  priv = toggle->priv;

  if (priv->is_active != is_active)
    {
      ClutterUnit pos;

      priv->is_active = is_active;

      if (priv->is_active)
        pos = clutter_actor_get_widthu (CLUTTER_ACTOR (toggle))
            - clutter_actor_get_widthu (priv->handle);
      else
        pos = 0;

      clutter_actor_set_xu (priv->handle, pos);

      g_object_notify (G_OBJECT (toggle), "active");
    }
}

/**
 * tidy_finger_toggle_get_active:
 * @toggle: a #TidyFingerToggle
 *
 * Retrieves the state of @toggle.
 *
 * Return value: %TRUE if the #TidyFingerToggle is active, %FALSE otherwise
 */
gboolean
tidy_finger_toggle_get_active (TidyFingerToggle *toggle)
{
  g_return_val_if_fail (TIDY_IS_FINGER_TOGGLE (toggle), FALSE);

  return toggle->priv->is_active;
}
