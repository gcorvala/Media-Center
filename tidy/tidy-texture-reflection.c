/* tidy-texture-reflection.c: adds a reflection on a texture
 *
 * Copyright (C) 2007 OpenedHand
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
 * License along with this library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 * Author: Matthew Allum <mallum@o-hand.com>
 */

/**
 * SECTION:tidy-texture-reflection
 * @short_description: Creates a reflection of a texture
 *
 * #ClutterReflectTexture allows the cloning of existing #ClutterTexture with 
 * a reflection-like effect.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <cogl/cogl.h>
#include <clutter/clutter-util.h>

#include "tidy-texture-reflection.h"
#include "tidy-private.h"

enum
{
  PROP_0,

  PROP_REFLECTION_HEIGHT
};

#define TIDY_TEXTURE_REFLECTION_GET_PRIVATE(obj)        \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj),            \
         TIDY_TYPE_TEXTURE_REFLECTION,                  \
         TidyTextureReflectionPrivate))

struct _TidyTextureReflectionPrivate
{
  gint reflection_height;
};

G_DEFINE_TYPE (TidyTextureReflection,
               tidy_texture_reflection,
               CLUTTER_TYPE_CLONE_TEXTURE);

static void
tidy_texture_reflection_paint (ClutterActor *actor)
{
  TidyTextureReflectionPrivate *priv;
  TidyTextureReflection *texture;
  ClutterCloneTexture   *clone;
  ClutterTexture        *parent;
  guint                  width, height;
  gint                   r_height;  

  CoglHandle        cogl_texture;
  CoglTextureVertex tvert[4];
  ClutterFixed      rty;

  texture = TIDY_TEXTURE_REFLECTION (actor);
  clone = CLUTTER_CLONE_TEXTURE (actor);

  parent = clutter_clone_texture_get_parent_texture (clone);
  if (!parent)
    return;

  if (!CLUTTER_ACTOR_IS_REALIZED (parent))
    clutter_actor_realize (CLUTTER_ACTOR (parent));

  cogl_texture = clutter_texture_get_cogl_texture (parent);
  if (cogl_texture == COGL_INVALID_HANDLE)
    return;

  priv = texture->priv;

  clutter_actor_get_size (actor, &width, &height);

  r_height = priv->reflection_height;

  if (r_height < 0 || r_height > height)
    r_height = height;

#define FX(x) CLUTTER_INT_TO_FIXED(x)

  rty = CFX_DIV(FX(r_height),FX(height));

  /* clockise vertices and tex coords and colors! */

  tvert[0].x = tvert[0].y = tvert[0].z = 0; 
  tvert[0].tx = 0; tvert[0].ty = rty;  
  tvert[0].color.red = tvert[0].color.green = tvert[0].color.blue = 0xff;
  tvert[0].color.alpha = clutter_actor_get_opacity (actor);

  tvert[1].x = FX(width); tvert[1].y = tvert[1].z = 0; 
  tvert[1].tx = CFX_ONE; tvert[1].ty = rty;  
  tvert[1].color.red = tvert[1].color.green = tvert[1].color.blue = 0xff;
  tvert[1].color.alpha = clutter_actor_get_opacity (actor);

  tvert[2].x = FX(width); tvert[2].y = FX(r_height); tvert[2].z = 0; 
  tvert[2].tx = CFX_ONE; tvert[2].ty = 0;  
  tvert[2].color.red = tvert[2].color.green = tvert[2].color.blue = 0xff;
  tvert[2].color.alpha = 0;

  tvert[3].x = 0; tvert[3].y = FX(r_height); tvert[3].z = 0; 
  tvert[3].tx = 0; tvert[3].ty = 0;  
  tvert[3].color.red = tvert[3].color.green = tvert[3].color.blue = 0xff;
  tvert[3].color.alpha = 0;

  cogl_push_matrix ();

  cogl_texture_polygon (cogl_texture, 4, tvert, TRUE);

  cogl_pop_matrix ();
}

static void
tidy_texture_reflection_set_property (GObject      *gobject,
                                      guint         prop_id,
                                      const GValue *value,
                                      GParamSpec   *pspec)
{
  TidyTextureReflection *texture;

  texture = TIDY_TEXTURE_REFLECTION (gobject);

  switch (prop_id)
    {
    case PROP_REFLECTION_HEIGHT:
      tidy_texture_reflection_set_reflection_height (texture,
                                                     g_value_get_int (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_texture_reflection_get_property (GObject    *gobject,
                                      guint       prop_id,
                                      GValue     *value,
                                      GParamSpec *pspec)
{
  TidyTextureReflection *texture;

  texture = TIDY_TEXTURE_REFLECTION (gobject);

  switch (prop_id)
    {
    case PROP_REFLECTION_HEIGHT:
      g_value_set_int (value,
                       tidy_texture_reflection_get_reflection_height (texture));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_texture_reflection_class_init (TidyTextureReflectionClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  g_type_class_add_private (klass, sizeof (TidyTextureReflectionPrivate));

  gobject_class->set_property = tidy_texture_reflection_set_property;
  gobject_class->get_property = tidy_texture_reflection_get_property;

  actor_class->paint = tidy_texture_reflection_paint;

  g_object_class_install_property (gobject_class,
                                   PROP_REFLECTION_HEIGHT,
                                   g_param_spec_int ("reflection-height",
                                                     "Reflection Height",
                                                     "Height of the reflection, or -1",
                                                     -1, G_MAXINT, -1,
                                                     TIDY_PARAM_READWRITE));
}

static void
tidy_texture_reflection_init (TidyTextureReflection *texture)
{
  TidyTextureReflectionPrivate *priv;

  texture->priv = priv = TIDY_TEXTURE_REFLECTION_GET_PRIVATE (texture);
  
  priv->reflection_height = -1;
}

ClutterActor *
tidy_texture_reflection_new (ClutterTexture *texture)
{
  g_return_val_if_fail (texture == NULL || CLUTTER_IS_TEXTURE (texture), NULL);

  return g_object_new (TIDY_TYPE_TEXTURE_REFLECTION,
                       "parent-texture", texture,
                       NULL);
}

void
tidy_texture_reflection_set_reflection_height (TidyTextureReflection *texture,
                                               gint                   height)
{
  TidyTextureReflectionPrivate *priv;

  g_return_if_fail (TIDY_IS_TEXTURE_REFLECTION (texture));

  priv = texture->priv;

  if (priv->reflection_height != height)
    {
      priv->reflection_height = height;

      if (CLUTTER_ACTOR_IS_VISIBLE (texture))
        clutter_actor_queue_redraw (CLUTTER_ACTOR (texture));

      g_object_notify (G_OBJECT (texture), "reflection-height");
    }
}

gint
tidy_texture_reflection_get_reflection_height (TidyTextureReflection *texture)
{
  g_return_val_if_fail (TIDY_IS_TEXTURE_REFLECTION (texture), -1);

  return texture->priv->reflection_height;
}
