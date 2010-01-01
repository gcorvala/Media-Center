/*
 * Tidy.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2006 OpenedHand
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
 */

/**
 * SECTION:tidy-behaviour-grow
 * @short_description: A behaviour controlling grow
 *
 * A #TidyBehaviourGrow interpolates actors size between two values.
 *
 */

#include "tidy-behaviour-grow.h"
#include <math.h>

G_DEFINE_TYPE (TidyBehaviourGrow,   \
               tidy_behaviour_grow, \
	       CLUTTER_TYPE_BEHAVIOUR);

#define CLUTTER_PARAM_READWRITE \
        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_STATIC_NAME | G_PARAM_STATIC_NICK |G_PARAM_STATIC_BLURB

struct _TidyBehaviourGrowPrivate
{
  guint w_start, h_start, w_end, h_end;
};

enum
{
  PROP_0,
  PROP_W_START,
  PROP_H_START,
  PROP_W_END,
  PROP_H_END
};

#define TIDY_BEHAVIOUR_GROW_GET_PRIVATE(obj)        \
              (G_TYPE_INSTANCE_GET_PRIVATE ((obj),    \
               TIDY_TYPE_BEHAVIOUR_GROW,            \
               TidyBehaviourGrowPrivate))

typedef struct {
  guint w,h;
} GrowClosure;

static void
grow_frame_foreach (ClutterBehaviour *behaviour,
                      ClutterActor     *actor,
                      gpointer          data)
{
  GrowClosure *closure = data;

  clutter_actor_set_size (actor, closure->w, closure->h);
}

static void
tidy_behaviour_grow_alpha_notify (ClutterBehaviour *behave,
                                  guint32           alpha_value)
{
  TidyBehaviourGrowPrivate *priv;
  GrowClosure c;

  priv = TIDY_BEHAVIOUR_GROW (behave)->priv;

  c.w = alpha_value * (priv->w_end - priv->w_start) / CLUTTER_ALPHA_MAX_ALPHA
              + priv->w_start;

  c.h = alpha_value * (priv->h_end - priv->h_start) / CLUTTER_ALPHA_MAX_ALPHA
              + priv->h_start;

  clutter_behaviour_actors_foreach (behave, grow_frame_foreach, &c);
}

static void
tidy_behaviour_grow_set_property (GObject      *gobject,
                                  guint         prop_id,
                                  const GValue *value,
                                  GParamSpec   *pspec)
{
  TidyBehaviourGrowPrivate *priv;

  priv = TIDY_BEHAVIOUR_GROW (gobject)->priv;

  switch (prop_id)
    {
    case PROP_W_START:
      priv->w_start = g_value_get_uint (value);
      break;
    case PROP_H_START:
      priv->h_start = g_value_get_uint (value);
      break;
    case PROP_W_END:
      priv->w_end = g_value_get_uint (value);
      break;
    case PROP_H_END:
      priv->h_end = g_value_get_uint (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_behaviour_grow_get_property (GObject    *gobject,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  TidyBehaviourGrowPrivate *priv;

  priv = TIDY_BEHAVIOUR_GROW (gobject)->priv;

  switch (prop_id)
    {
    case PROP_W_START:
      g_value_set_uint (value, priv->w_start);
      break;
    case PROP_H_START:
      g_value_set_uint (value, priv->h_start);
      break;
    case PROP_W_END:
      g_value_set_uint (value, priv->w_end);
      break;
    case PROP_H_END:
      g_value_set_uint (value, priv->h_end);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_behaviour_grow_class_init (TidyBehaviourGrowClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterBehaviourClass *behave_class = CLUTTER_BEHAVIOUR_CLASS (klass);

  behave_class->alpha_notify = tidy_behaviour_grow_alpha_notify;

  gobject_class->set_property = tidy_behaviour_grow_set_property;
  gobject_class->get_property = tidy_behaviour_grow_get_property;

  g_object_class_install_property 
         (gobject_class,
          PROP_H_START,
          g_param_spec_uint ("height-start",
                            "Height Start",
                            "height to grow the actor by",
                             0, G_MAXINT, 0,
                             CLUTTER_PARAM_READWRITE));

  g_object_class_install_property 
          (gobject_class,
           PROP_W_START,
           g_param_spec_uint ("width-start",
                             "Width Start",
                             "width to grow actor by",
                              0, G_MAXINT, 0,
                             CLUTTER_PARAM_READWRITE));

  g_object_class_install_property 
         (gobject_class,
          PROP_H_END,
          g_param_spec_uint ("height-end",
                            "Height End",
                            "height to grow the actor by",
                             0, G_MAXINT, 0,
                             CLUTTER_PARAM_READWRITE));

  g_object_class_install_property 
          (gobject_class,
           PROP_W_END,
           g_param_spec_uint ("width-end",
                             "Width End",
                             "width to grow actor by",
                              0, G_MAXINT, 0,
                             CLUTTER_PARAM_READWRITE));

  g_type_class_add_private (klass, sizeof (TidyBehaviourGrowPrivate));
}

static void
tidy_behaviour_grow_init (TidyBehaviourGrow *self)
{
  TidyBehaviourGrowPrivate *priv;

  self->priv = priv = TIDY_BEHAVIOUR_GROW_GET_PRIVATE (self);

  priv->w_start = priv->h_start = priv->w_end = priv->h_end = 0;
}

/**
 * tidy_behaviour_grow_new:
 * @alpha: a #TidyAlpha
 *
 * Return value: the newly created #TidyBehaviourGrow
 *
 */
ClutterBehaviour *
tidy_behaviour_grow_new (ClutterAlpha *alpha,
                         guint         w_start,
                         guint         h_start,
                         guint         w_end,
                         guint         h_end)
{
  TidyBehaviourGrow *behave;

  g_return_val_if_fail (alpha == NULL || CLUTTER_IS_ALPHA (alpha), NULL);

  behave = g_object_new (TIDY_TYPE_BEHAVIOUR_GROW, 
                         "alpha", alpha, 
                         "width-start", w_start,
                         "height-start", h_start,
                         "width-end", w_end,
                         "height-end", h_end,
                         NULL);

  return CLUTTER_BEHAVIOUR (behave);
}

