/* tidy-list-column-default.h: Default list column
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

#ifndef __TIDY_LIST_COLUMN_DEFAULT_H__
#define __TIDY_LIST_COLUMN_DEFAULT_H__

#include <tidy/tidy-list-column.h>
#include <tidy/tidy-list-view.h>

G_BEGIN_DECLS

#define TIDY_TYPE_LIST_COLUMN_DEFAULT           (tidy_list_column_default_get_type ())
#define TIDY_LIST_COLUMN_DEFAULT(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), TIDY_TYPE_LIST_COLUMN_DEFAULT, TidyListColumnDefault))
#define TIDY_IS_LIST_COLUMN_DEFAULT(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TIDY_TYPE_LIST_COLUMN_DEFAULT))

typedef struct _TidyListColumnDefault           TidyListColumnDefault;

GType           tidy_list_column_default_get_type (void) G_GNUC_CONST;
TidyListColumn *tidy_list_column_default_new      (TidyListView *list_view,
                                                   guint         model_index);

G_END_DECLS

#endif /* __TIDY_LIST_COLUMN_DEFAULT_H__ */
