/* tidy-list-column-default.c: Default list column
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <glib-object.h>

#include "tidy-cell-renderer.h"
#include "tidy-cell-renderer-default.h"
#include "tidy-list-column.h"
#include "tidy-list-column-default.h"

#include "tidy-debug.h"
#include "tidy-enum-types.h"
#include "tidy-private.h"

/**
 * SECTION:tidy-list-column-default
 * @short_description: Default list view column
 *
 * #TidyListColumnDefault is a simple implementation of the
 * #TidyListColumn class. It provides a default column for the #TidyListView
 * actor, using a #TidyCellRendererDefault cell renderer.
 *
 * #TidyListColumnDefault is a final class, and cannot be subclassed.
 */

#define TIDY_LIST_COLUMN_DEFAULT_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST ((klass), \
         TIDY_TYPE_LIST_COLUMN_DEFAULT, \
         TidyListColumnDefaultClass))
#define TIDY_IS_LIST_COLUMN_DEFAULT_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE ((klass), \
         TIDY_TYPE_LIST_COLUMN_DEFAULT))
#define TIDY_LIST_COLUMN_DEFAULT_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS ((obj), \
         TIDY_TYPE_LIST_COLUMN_DEFAULT, \
         TidyListColumnDefaultClass))

typedef struct _TidyListColumnClass     TidyListColumnDefaultClass;

struct _TidyListColumnDefault
{
  TidyListColumn parent_instance;
};

G_DEFINE_TYPE (TidyListColumnDefault,
               tidy_list_column_default,
               TIDY_TYPE_LIST_COLUMN);

static void
tidy_list_column_default_class_init (TidyListColumnDefaultClass *klass)
{

}

static void
tidy_list_column_default_init (TidyListColumnDefault *default_column)
{
  TidyListColumn *column = TIDY_LIST_COLUMN (default_column);

  tidy_list_column_set_min_width (column, 75);
  tidy_list_column_set_cell_renderer (column, tidy_cell_renderer_default_new ());
  tidy_list_column_set_header_renderer (column, tidy_cell_renderer_default_new ());
}

TidyListColumn *
tidy_list_column_default_new (TidyListView *list_view,
                              guint         model_index)
{
  g_return_val_if_fail (TIDY_IS_LIST_VIEW (list_view), NULL);

  return g_object_new (TIDY_TYPE_LIST_COLUMN_DEFAULT,
                       "list-view", list_view,
                       "model-index", model_index,
                       NULL);
}
