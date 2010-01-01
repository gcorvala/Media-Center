/* tidy-depth-group.c: Simple ClutterGroup extension that enables depth testing
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <clutter/clutter.h>
#include <cogl/cogl.h>

#include "tidy-depth-group.h"

/**
 * SECTION:tidy-depth-group
 * @short_description: A container with depth testing
 *
 * #TidyDepthGroup is a special subclass of #ClutterGroup that enables
 * GL depth testing when painting its children.
 */

G_DEFINE_TYPE (TidyDepthGroup, tidy_depth_group, CLUTTER_TYPE_GROUP)

static void
tidy_depth_group_paint (ClutterActor *actor)
{
  cogl_enable_depth_test (TRUE);
  
  CLUTTER_ACTOR_CLASS (tidy_depth_group_parent_class)->paint (actor);
  
  cogl_enable_depth_test (FALSE);
}

static void
tidy_depth_group_class_init (TidyDepthGroupClass *klass)
{
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  actor_class->paint = tidy_depth_group_paint;
}

static void
tidy_depth_group_init (TidyDepthGroup *self)
{
}

ClutterActor *
tidy_depth_group_new (void)
{
  return CLUTTER_ACTOR (g_object_new (TIDY_TYPE_DEPTH_GROUP, NULL));
}

