/*
 * Tidy.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *             Jorn Baayen  <jorn@openedhand.com>
 *             Emmanuele Bassi  <ebassi@openedhand.com>
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

#ifndef __TIDY_BEHAVIOUR_GROW_H__
#define __TIDY_BEHAVIOUR_GROW_H__

#include <clutter/clutter-actor.h>
#include <clutter/clutter-alpha.h>
#include <clutter/clutter-behaviour.h>
#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define TIDY_TYPE_BEHAVIOUR_GROW (tidy_behaviour_grow_get_type ())

#define TIDY_BEHAVIOUR_GROW(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  TIDY_TYPE_BEHAVIOUR_GROW, TidyBehaviourGrow))

#define TIDY_BEHAVIOUR_GROW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  TIDY_TYPE_BEHAVIOUR_GROW, TidyBehaviourGrowClass))

#define TIDY_IS_BEHAVIOUR_GROW(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  TIDY_TYPE_BEHAVIOUR_GROW))

#define TIDY_IS_BEHAVIOUR_GROW_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  TIDY_TYPE_BEHAVIOUR_GROW))

#define TIDY_BEHAVIOUR_GROW_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  TIDY_TYPE_BEHAVIOUR_GROW, TidyBehaviourGrowClass))

typedef struct _TidyBehaviourGrow        TidyBehaviourGrow;
typedef struct _TidyBehaviourGrowPrivate TidyBehaviourGrowPrivate;
typedef struct _TidyBehaviourGrowClass   TidyBehaviourGrowClass;

struct _TidyBehaviourGrow
{
  ClutterBehaviour parent_instance;

  /*< private >*/
  TidyBehaviourGrowPrivate *priv;
};

struct _TidyBehaviourGrowClass
{
  ClutterBehaviourClass parent_class;
};

GType tidy_behaviour_grow_get_type (void) G_GNUC_CONST;

ClutterBehaviour *
tidy_behaviour_grow_new (ClutterAlpha *alpha,
                         guint         w_start,
                         guint         h_start,
                         guint         w_end,
                         guint         h_end);

G_END_DECLS

#endif /* __TIDY_BEHAVIOUR_GROW_H__ */
