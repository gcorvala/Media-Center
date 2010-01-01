/* tidy-cell-renderer-default.h: Default cell renderer
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
 *
 * Written by: Emmanuele Bassi <ebassi@openedhand.com>
 */

#ifndef __TIDY_CELL_RENDERER_DEFAULT_H__
#define __TIDY_CELL_RENDERER_DEFAULT_H__

#include <glib-object.h>
#include <tidy/tidy-cell-renderer.h>

G_BEGIN_DECLS

#define TIDY_TYPE_CELL_RENDERER_DEFAULT         (tidy_cell_renderer_default_get_type ())
#define TIDY_CELL_RENDERER_DEFAULT(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), TIDY_TYPE_CELL_RENDERER_DEFAULT, TidyCellRendererDefault))
#define TIDY_IS_CELL_RENDERER_DEFAULT(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TIDY_TYPE_CELL_RENDERER_DEFAULT))

typedef struct _TidyCellRendererDefault         TidyCellRendererDefault;

GType             tidy_cell_renderer_default_get_type (void) G_GNUC_CONST;
TidyCellRenderer *tidy_cell_renderer_default_new      (void);

G_END_DECLS

#endif /* __TIDY_CELL_RENDERER_DEFAULT_H__ */
