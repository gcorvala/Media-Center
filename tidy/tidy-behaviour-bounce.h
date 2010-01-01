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

#ifndef __TIDY_BEHAVIOUR_BOUNCE_H__
#define __TIDY_BEHAVIOUR_BOUNCE_H__

#include <clutter/clutter-actor.h>
#include <clutter/clutter-alpha.h>
#include <clutter/clutter-behaviour.h>
#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define TIDY_TYPE_BEHAVIOUR_BOUNCE (tidy_behaviour_bounce_get_type ())

#define TIDY_BEHAVIOUR_BOUNCE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  TIDY_TYPE_BEHAVIOUR_BOUNCE, TidyBehaviourBounce))

#define TIDY_BEHAVIOUR_BOUNCE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  TIDY_TYPE_BEHAVIOUR_BOUNCE, TidyBehaviourBounceClass))

#define TIDY_IS_BEHAVIOUR_BOUNCE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  TIDY_TYPE_BEHAVIOUR_BOUNCE))

#define TIDY_IS_BEHAVIOUR_BOUNCE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  TIDY_TYPE_BEHAVIOUR_BOUNCE))

#define TIDY_BEHAVIOUR_BOUNCE_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  TIDY_TYPE_BEHAVIOUR_BOUNCE, TidyBehaviourBounceClass))

typedef struct _TidyBehaviourBounce        TidyBehaviourBounce;
typedef struct _TidyBehaviourBouncePrivate TidyBehaviourBouncePrivate;
typedef struct _TidyBehaviourBounceClass   TidyBehaviourBounceClass;

struct _TidyBehaviourBounce
{
  ClutterBehaviour parent_instance;

  /*< private >*/
  TidyBehaviourBouncePrivate *priv;
};

struct _TidyBehaviourBounceClass
{
  ClutterBehaviourClass parent_class;
};

GType tidy_behaviour_bounce_get_type (void) G_GNUC_CONST;

ClutterBehaviour *
tidy_behaviour_bounce_new (ClutterAlpha *alpha);

G_END_DECLS

#endif /* __TIDY_BEHAVIOUR_BOUNCE_H__ */
