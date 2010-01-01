/* tidy-finger-toggle.h: Finger toggle button
 *
 * Copyright (C) 2008  OpenedHand
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

#ifndef __TIDY_FINGER_TOGGLE_H__
#define __TIDY_FINGER_TOGGLE_H__

#include <tidy/tidy-frame.h>

G_BEGIN_DECLS

#define TIDY_TYPE_FINGER_TOGGLE                 (tidy_finger_toggle_get_type ())
#define TIDY_FINGER_TOGGLE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), TIDY_TYPE_FINGER_TOGGLE, TidyFingerToggle))
#define TIDY_IS_FINGER_TOGGLE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TIDY_TYPE_FINGER_TOGGLE))
#define TIDY_FINGER_TOGGLE_CLASS(klass)         (G_TYPE_CHECK_CLASS_CAST ((klass), TIDY_TYPE_FINGER_TOGGLE, TidyFingerToggleClass))
#define TIDY_IS_FINGER_TOGGLE_CLASS(klass)      (G_TYPE_CHECK_CLASS_TYPE ((klass), TIDY_TYPE_FINGER_TOGGLE))
#define TIDY_FINGER_TOGGLE_GET_CLASS(obj)       (G_TYPE_INSTANCE_GET_CLASS ((obj), TIDY_TYPE_FINGER_TOGGLE, TidyFingerToggleClass))

typedef struct _TidyFingerToggle                TidyFingerToggle;
typedef struct _TidyFingerTogglePrivate         TidyFingerTogglePrivate;
typedef struct _TidyFingerToggleClass           TidyFingerToggleClass;

struct _TidyFingerToggle
{
  TidyFrame parent_instance;

  TidyFingerTogglePrivate *priv;
};

struct _TidyFingerToggleClass
{
  TidyFrameClass parent_class;

  void (* toggled) (TidyFingerToggle *toggle);
};

GType         tidy_finger_toggle_get_type   (void) G_GNUC_CONST;
ClutterActor *tidy_finger_toggle_new        (void);

void          tidy_finger_toggle_set_handle (TidyFingerToggle *toggle,
                                             ClutterActor     *handle);
ClutterActor *tidy_finger_toggle_get_handle (TidyFingerToggle *toggle);
void          tidy_finger_toggle_set_active (TidyFingerToggle *toggle,
                                             gboolean          is_active);
gboolean      tidy_finger_toggle_get_active (TidyFingerToggle *toggle);

G_END_DECLS

#endif /* __TIDY_FINGER_TOGGLE_H__ */
