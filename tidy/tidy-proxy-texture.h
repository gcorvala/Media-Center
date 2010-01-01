/*
 * Clutter.
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

#ifndef _HAVE_TIDY_PROXY_TEXTURE_H
#define _HAVE_TIDY_PROXY_TEXTURE_H

#include <clutter/clutter-texture.h>

G_BEGIN_DECLS

#define TIDY_TYPE_PROXY_TEXTURE (tidy_proxy_texture_get_type ())

#define TIDY_PROXY_TEXTURE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  TIDY_TYPE_PROXY_TEXTURE, TidyProxyTexture))

#define TIDY_PROXY_TEXTURE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  TIDY_TYPE_PROXY_TEXTURE, TidyProxyTextureClass))

#define TIDY_IS_PROXY_TEXTURE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  TIDY_TYPE_PROXY_TEXTURE))

#define TIDY_IS_PROXY_TEXTURE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  TIDY_TYPE_PROXY_TEXTURE))

#define TIDY_PROXY_TEXTURE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  TIDY_TYPE_PROXY_TEXTURE, TidyProxyTextureClass))

typedef struct _TidyProxyTexture        TidyProxyTexture;
typedef struct _TidyProxyTexturePrivate TidyProxyTexturePrivate;
typedef struct _TidyProxyTextureClass   TidyProxyTextureClass;

struct _TidyProxyTexture
{
  ClutterActor                 parent;
  
  /*< private >*/
  TidyProxyTexturePrivate    *priv;
};

struct _TidyProxyTextureClass 
{
  /*< private >*/
  ClutterActorClass parent_class;

  /* padding for future expansion */
  void (*_tidy_proxy_1) (void);
  void (*_tidy_proxy_2) (void);
  void (*_tidy_proxy_3) (void);
  void (*_tidy_proxy_4) (void);
}; 

GType                 tidy_proxy_texture_get_type (void) G_GNUC_CONST;

ClutterActor *        tidy_proxy_texture_new      (const gchar *path);
G_CONST_RETURN gchar *tidy_proxy_texture_get_path (TidyProxyTexture *proxy);

G_END_DECLS

#endif /* _HAVE_TIDY_PROXY_TEXTURE_H */
