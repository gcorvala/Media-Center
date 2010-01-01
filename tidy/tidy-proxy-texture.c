/*
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
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
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:tidy-proxy-texture
 * @short_description: FIXME
 *
 * FIXME
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <clutter/clutter-scriptable.h>
#include <clutter/clutter-texture.h>
#include <clutter/clutter-clone-texture.h>
#include <cogl/cogl.h>

#include "tidy-proxy-texture.h"
#include "tidy-texture-cache.h"


enum
{
  PROP_0,

  PROP_PATH,
  PROP_USE_THUMBS
};

static void clutter_scriptable_iface_init (ClutterScriptableIface *iface);

G_DEFINE_TYPE_WITH_CODE (TidyProxyTexture,
                         tidy_proxy_texture,
                         CLUTTER_TYPE_ACTOR,
                         G_IMPLEMENT_INTERFACE (CLUTTER_TYPE_SCRIPTABLE,
                                                clutter_scriptable_iface_init));

#define TIDY_PROXY_TEXTURE_GET_PRIVATE(obj) \
(G_TYPE_INSTANCE_GET_PRIVATE ((obj), TIDY_TYPE_PROXY_TEXTURE, TidyProxyTexturePrivate))

struct _TidyProxyTexturePrivate
{
  ClutterActor *real_texture;

  gchar        *path;
  gchar        *real_path;
  gulong        loaded_signal_id;
  gboolean      use_thumbs;
};

static gchar *
get_thumbnail_path (TidyProxyTexture *self)
{
  guint size;
  TidyProxyTexturePrivate *priv = self->priv;
  ClutterActor *actor = CLUTTER_ACTOR (self);
  
  if (!priv->path)
    return NULL;
  
  size = MAX (clutter_actor_get_width (actor),
              clutter_actor_get_height (actor));
  
  /* TODO: Extend this and thumbnail spec to handle sizes > 256^2 */
  
  if ((size > 0) && (size <= 256))
    {
      gchar *abs_path, *uri;
      GError *error = NULL;
      
      if (!g_path_is_absolute (priv->path))
        {
          gchar *cwd = g_get_current_dir ();
          abs_path = g_build_filename (cwd, priv->path, NULL);
          g_free (cwd);
        }
      else
        abs_path = NULL;
      
      uri = g_filename_to_uri (abs_path ? abs_path : priv->path, NULL, &error);
      g_free (abs_path);
      
      if (uri)
        {
          gchar *thumbnail, *filename, *md5;
          
          md5 = g_compute_checksum_for_string (G_CHECKSUM_MD5, uri, -1);
          filename = g_strconcat (md5, ".png", NULL);
          
          thumbnail = g_build_filename (g_get_home_dir (),
                                        ".thumbnails",
                                        (size <= 128) ? "normal" : "large",
                                        filename,
                                        NULL);
          
          g_free (md5);
          
          if (thumbnail && g_file_test (thumbnail, G_FILE_TEST_EXISTS))
            return thumbnail;
          
          g_free (thumbnail);
        }
      else
        {
          g_warning ("Error creating URI: %s", error->message);
          g_error_free (error);
        }
    }
  
  return g_strdup (priv->path);
}

static void
texture_cache_loaded_cb (TidyTextureCache *cache,
                         const gchar      *path,
                         ClutterTexture   *texture,
                         TidyProxyTexture *self)
{
  const gchar *real_path;
  TidyProxyTexturePrivate *priv = self->priv;
  
  real_path = priv->real_path ? priv->real_path : priv->path;
  
  if (path && real_path && (strcmp (path, real_path) == 0))
    {
      if (priv->real_texture)
        g_object_unref (priv->real_texture);
      priv->real_texture = clutter_clone_texture_new (texture);
      clutter_actor_queue_redraw (CLUTTER_ACTOR (self));
    }
}

static void
tidy_proxy_texture_refresh (TidyProxyTexture *self)
{
  TidyProxyTexturePrivate *priv = self->priv;

  if (!priv->path)
    return;
  
  g_free (priv->real_path);
  priv->real_path = priv->use_thumbs ? get_thumbnail_path (self) : NULL;
  tidy_texture_cache_get_texture_async (tidy_texture_cache_get_default (),
                                        priv->real_path ?
                                          priv->real_path : priv->path);
}

static void
tidy_proxy_texture_set_path (TidyProxyTexture *self,
                             const gchar      *path)
{
  TidyProxyTexturePrivate *priv = self->priv;
  gchar *real_path;

  if (!path || path[0] == '\0')
    return;

  if (g_path_is_absolute (path))
    real_path = g_strdup (path);
  else
    {
      gchar *dir = g_get_current_dir ();

      real_path = g_build_filename (dir, path, NULL);
      g_free (dir);
    }

  if (priv->path && (strcmp (priv->path, real_path) == 0))
    {
      g_free (real_path);
      return;
    }

  g_free (priv->path);
  priv->path = real_path;

  tidy_proxy_texture_refresh (self);

  g_object_notify (G_OBJECT (self), "path");
}

static void
tidy_proxy_texture_paint (ClutterActor *self)
{
  TidyProxyTexture        *proxy = TIDY_PROXY_TEXTURE(self);
  TidyProxyTexturePrivate *priv = proxy->priv;  

  ClutterCloneTexture     *real_texture;
  ClutterTexture          *parent_texture;
  gint                     x_1, y_1, x_2, y_2;
  ClutterColor             col = { 0xff, 0xff, 0xff, 0xff };
  CoglHandle               cogl_texture;
  ClutterFixed             t_w, t_h;
  guint                    tex_width, tex_height;

  if (priv->real_texture == NULL)
    return;

  real_texture = CLUTTER_CLONE_TEXTURE (priv->real_texture);
  if (!CLUTTER_ACTOR_IS_REALIZED (priv->real_texture))
    clutter_actor_realize (priv->real_texture);

  cogl_push_matrix ();

  col.alpha = clutter_actor_get_paint_opacity (self);
  cogl_color (&col);

  parent_texture = clutter_clone_texture_get_parent_texture (real_texture);
  if (!parent_texture)
    return;

  cogl_texture = clutter_texture_get_cogl_texture (parent_texture);
  if (cogl_texture == COGL_INVALID_HANDLE)
    return;

  clutter_actor_get_allocation_coords (self, &x_1, &y_1, &x_2, &y_2);

  tex_width = cogl_texture_get_width (cogl_texture);
  tex_height = cogl_texture_get_height (cogl_texture);

  t_w = CFX_ONE;
  t_h = CFX_ONE;

  /* Parent paint translated us into position */
  cogl_texture_rectangle (cogl_texture,
                          0,
                          0,
                          CLUTTER_INT_TO_FIXED (x_2 - x_1),
                          CLUTTER_INT_TO_FIXED (y_2 - y_1),
                          0, 0,
                          t_w, t_h);

  cogl_pop_matrix ();
}

static void
tidy_proxy_texture_get_preferred_width (ClutterActor *self,
                                        ClutterUnit   for_height,
                                        ClutterUnit  *min_width_p,
                                        ClutterUnit  *natural_width_p)
{
  TidyProxyTexture        *proxy = TIDY_PROXY_TEXTURE (self);
  TidyProxyTexturePrivate *priv = proxy->priv;
  ClutterCloneTexture     *real_texture;
  ClutterTexture          *parent_texture;
  ClutterActorClass       *parent_texture_class;

  if (priv->real_texture == NULL)
    parent_texture = NULL;
  else
    {
      real_texture = CLUTTER_CLONE_TEXTURE (priv->real_texture);
      parent_texture = clutter_clone_texture_get_parent_texture (real_texture);
    }

  if (!parent_texture)
    {
      if (min_width_p)
        *min_width_p = 0;

      if (natural_width_p)
        *natural_width_p = 0;

      return;
    }

  parent_texture_class =
    CLUTTER_ACTOR_GET_CLASS (CLUTTER_ACTOR (parent_texture));
  parent_texture_class->get_preferred_width (CLUTTER_ACTOR (parent_texture),
                                             -1,
                                             min_width_p,
                                             natural_width_p);
}

static void
tidy_proxy_texture_get_preferred_height (ClutterActor *self,
                                         ClutterUnit   for_width,
                                         ClutterUnit  *min_height_p,
                                         ClutterUnit  *natural_height_p)
{
  TidyProxyTexture        *proxy = TIDY_PROXY_TEXTURE (self);
  TidyProxyTexturePrivate *priv = proxy->priv;
  ClutterCloneTexture     *real_texture;
  ClutterTexture          *parent_texture;
  ClutterActorClass       *parent_texture_class;

  if (priv->real_texture == NULL)
    parent_texture = NULL;
  else
    {
      real_texture = CLUTTER_CLONE_TEXTURE (priv->real_texture);
      parent_texture = clutter_clone_texture_get_parent_texture (real_texture);
    }

  if (!parent_texture)
    {
      if (min_height_p)
        *min_height_p = 0;

      if (natural_height_p)
        *natural_height_p = 0;

      return;
    }

  parent_texture_class =
    CLUTTER_ACTOR_GET_CLASS (CLUTTER_ACTOR (parent_texture));
  parent_texture_class->get_preferred_height (CLUTTER_ACTOR (parent_texture),
                                              for_width,
                                              min_height_p,
                                              natural_height_p);
}

static void 
tidy_proxy_texture_dispose (GObject *object)
{
  TidyProxyTexture         *self = TIDY_PROXY_TEXTURE(object);
  TidyProxyTexturePrivate  *priv = self->priv;  

  if (priv->real_texture)
    g_object_unref (priv->real_texture);

  priv->real_texture = NULL;

  if (priv->loaded_signal_id)
    {
      g_signal_handler_disconnect (tidy_texture_cache_get_default (),
                                   priv->loaded_signal_id);
      priv->loaded_signal_id = 0;
    }

  G_OBJECT_CLASS (tidy_proxy_texture_parent_class)->dispose (object);
}

static void 
tidy_proxy_texture_finalize (GObject *object)
{
  TidyProxyTexture         *self = TIDY_PROXY_TEXTURE(object);
  TidyProxyTexturePrivate  *priv = self->priv;  

  g_free (priv->path);
  g_free (priv->real_path);
  
  G_OBJECT_CLASS (tidy_proxy_texture_parent_class)->finalize (object);
}

static void
tidy_proxy_texture_set_property (GObject      *object,
				 guint         prop_id,
				 const GValue *value,
				 GParamSpec   *pspec)
{
  TidyProxyTexture *proxy = TIDY_PROXY_TEXTURE (object);

  switch (prop_id)
    {
    case PROP_PATH:
      tidy_proxy_texture_set_path (proxy, g_value_get_string (value));
      break;
    case PROP_USE_THUMBS:
      proxy->priv->use_thumbs = g_value_get_boolean (value);
      tidy_proxy_texture_refresh (proxy);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
tidy_proxy_texture_get_property (GObject    *object,
				 guint       prop_id,
				 GValue     *value,
				 GParamSpec *pspec)
{
  TidyProxyTexture *proxy = TIDY_PROXY_TEXTURE (object);

  switch (prop_id)
    {
    case PROP_PATH:
      g_value_set_string (value, proxy->priv->path);
      break;
    case PROP_USE_THUMBS:
      g_value_set_boolean (value, proxy->priv->use_thumbs);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}

static void
tidy_proxy_texture_class_init (TidyProxyTextureClass *klass)
{
  GObjectClass      *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  actor_class->paint = tidy_proxy_texture_paint;
  actor_class->get_preferred_width = tidy_proxy_texture_get_preferred_width;
  actor_class->get_preferred_height = tidy_proxy_texture_get_preferred_height;

  gobject_class->finalize     = tidy_proxy_texture_finalize;
  gobject_class->dispose      = tidy_proxy_texture_dispose;
  gobject_class->set_property = tidy_proxy_texture_set_property;
  gobject_class->get_property = tidy_proxy_texture_get_property;

  g_object_class_install_property 
             (gobject_class,
	      PROP_PATH,
	      g_param_spec_string ("path",
				   "Path",
				   "Path to texture image",
				   NULL,
				   G_PARAM_READWRITE));

  g_object_class_install_property 
             (gobject_class,
	      PROP_USE_THUMBS,
	      g_param_spec_boolean ("use-thumbs",
				    "Use thumbnails",
				    "Whether to use thumbnails",
				    FALSE,
				    G_PARAM_READWRITE));

  g_type_class_add_private (gobject_class, sizeof (TidyProxyTexturePrivate));
}

static void
tidy_proxy_texture_init (TidyProxyTexture *self)
{
  TidyProxyTexturePrivate *priv;

  self->priv = priv = TIDY_PROXY_TEXTURE_GET_PRIVATE (self);

  priv->real_texture = NULL;
  priv->path = NULL;
  
  priv->loaded_signal_id =
    g_signal_connect (tidy_texture_cache_get_default (), "loaded",
                      G_CALLBACK (texture_cache_loaded_cb), self);
  
  g_signal_connect (self, "notify::width",
                    G_CALLBACK (tidy_proxy_texture_refresh), NULL);
  g_signal_connect (self, "notify::height",
                    G_CALLBACK (tidy_proxy_texture_refresh), NULL);
}

static gboolean
tidy_proxy_texture_parse_custom_node (ClutterScriptable *scriptable,
                                      ClutterScript     *script,
                                      GValue            *value,
                                      const gchar       *name,
                                      JsonNode          *node)
{
  if (strcmp (name, "path") == 0)
    {
      const gchar *path;

      if (JSON_NODE_TYPE (node) != JSON_NODE_VALUE)
        return FALSE;

      path = json_node_get_string (node);
      if (!path || *path == '\0')
        return FALSE;

      g_value_init (value, G_TYPE_STRING);

      if (g_path_is_absolute (path))
        g_value_set_string (value, path);
      else
        {
          gboolean filename_set = FALSE;
          const gchar *filename = NULL;
          gchar *dir, *real_path;

          /* let's check if we have a filename */
          g_object_get (G_OBJECT (script),
                        "filename-set", &filename_set,
                        "filename", &filename,
                        NULL);

          if (filename_set)
            dir = g_path_get_dirname (filename);
          else
            dir = g_get_current_dir ();

          real_path = g_build_filename (dir, path, NULL);

          g_value_set_string (value, real_path);
          
          g_free (real_path);
          g_free (dir);
        }

      return TRUE;
    }

  return FALSE;
}

static void
clutter_scriptable_iface_init (ClutterScriptableIface *iface)
{
  iface->parse_custom_node = tidy_proxy_texture_parse_custom_node;
}

/**
 * tidy_proxy_texture_new:
 * @path: the path of the image to use
 *
 * Creates a proxy texture.
 *
 * Return value: the newly created #TidyProxyTexture
 */
ClutterActor *
tidy_proxy_texture_new (const gchar *path)
{
  g_return_val_if_fail (path != NULL, NULL);

  return g_object_new (TIDY_TYPE_PROXY_TEXTURE, "path", path, NULL);
}

G_CONST_RETURN gchar *
tidy_proxy_texture_get_path (TidyProxyTexture *proxy)
{
  g_return_val_if_fail (TIDY_IS_PROXY_TEXTURE (proxy), NULL);

  return proxy->priv->path;
}
