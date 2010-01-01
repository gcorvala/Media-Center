/* tidy-texture-cache.h: Cached textures object
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


#ifndef _TIDY_TEXTURE_CACHE
#define _TIDY_TEXTURE_CACHE

#include <glib-object.h>
#include <clutter/clutter.h>

G_BEGIN_DECLS

#define TIDY_TYPE_TEXTURE_CACHE tidy_texture_cache_get_type()

#define TIDY_TEXTURE_CACHE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  TIDY_TYPE_TEXTURE_CACHE, TidyTextureCache))

#define TIDY_TEXTURE_CACHE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  TIDY_TYPE_TEXTURE_CACHE, TidyTextureCacheClass))

#define TIDY_IS_TEXTURE_CACHE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  TIDY_TYPE_TEXTURE_CACHE))

#define TIDY_IS_TEXTURE_CACHE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  TIDY_TYPE_TEXTURE_CACHE))

#define TIDY_TEXTURE_CACHE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  TIDY_TYPE_TEXTURE_CACHE, TidyTextureCacheClass))

typedef struct {
  GObject parent;
} TidyTextureCache;

typedef struct {
  GObjectClass parent_class;

  void (* loaded)        (TidyTextureCache *self,
                          const gchar      *path,
                          ClutterTexture   *texture);

  void (* error_loading) (TidyTextureCache *self,
                          GError           *error);
} TidyTextureCacheClass;

GType tidy_texture_cache_get_type (void);

TidyTextureCache* tidy_texture_cache_new (void);

TidyTextureCache*
tidy_texture_cache_get_default (void);

ClutterActor*
tidy_texture_cache_get_texture (TidyTextureCache *self,
				const gchar      *path, 
				gboolean          want_clone);

void
tidy_texture_cache_get_texture_async (TidyTextureCache *self,
                                      const gchar      *path);

gint
tidy_texture_cache_get_size (TidyTextureCache *self);

G_END_DECLS

#endif /* _TIDY_TEXTURE_CACHE */
