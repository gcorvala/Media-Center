/* tidy-depth-group.h: Simple ClutterGroup extension that enables depth testing
 *
 * Copyright (C) 2008 OpenedHand
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
 * Written by: Chris Lord <chris@openedhand.com>
 */

#ifndef __TIDY_DEPTH_GROUP_H__
#define __TIDY_DEPTH_GROUP_H__

#include <glib-object.h>
#include <clutter/clutter-group.h>

G_BEGIN_DECLS

#define TIDY_TYPE_DEPTH_GROUP            (tidy_depth_group_get_type())
#define TIDY_DEPTH_GROUP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), TIDY_TYPE_DEPTH_GROUP, TidyDepthGroup))
#define TIDY_IS_DEPTH_GROUP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TIDY_TYPE_DEPTH_GROUP))
#define TIDY_DEPTH_GROUP_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), TIDY_TYPE_DEPTH_GROUP, TidyDepthGroupClass))
#define TIDY_IS_DEPTH_GROUP_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TIDY_TYPE_DEPTH_GROUP))
#define TIDY_DEPTH_GROUP_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), TIDY_TYPE_DEPTH_GROUP, TidyDepthGroupClass))

typedef struct _TidyDepthGroup          TidyDepthGroup;
typedef struct _TidyDepthGroupClass     TidyDepthGroupClass;

struct _TidyDepthGroup
{
  /*< private >*/
  ClutterGroup parent_instance;
};

struct _TidyDepthGroupClass
{
  ClutterGroupClass parent_class;
};

GType tidy_depth_group_get_type (void) G_GNUC_CONST;

ClutterActor *tidy_depth_group_new ();

G_END_DECLS

#endif /* __TIDY_DEPTH_GROUP_H__ */

