/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Copyright (C) 2006-2008 OpenedHand
 *
 * Authored By Øyvind Kolås <pippin@o-hand.com>
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

#ifndef __TIDY_TEXT_H__
#define __TIDY_TEXT_H__

#include <clutter/clutter-label.h>
#include <clutter/clutter-types.h>

G_BEGIN_DECLS

#define TIDY_TYPE_TEXT (tidy_text_get_type ())

#define TIDY_TEXT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
  TIDY_TYPE_TEXT, TidyText))

#define TIDY_TEXT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
  TIDY_TYPE_TEXT, TidyTextClass))

#define TIDY_IS_TEXT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
  TIDY_TYPE_TEXT))

#define TIDY_IS_TEXT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
  TIDY_TYPE_TEXT))

#define TIDY_TEXT_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
  TIDY_TYPE_TEXT, TidyTextClass))

typedef struct _TidyText        TidyText;
typedef struct _TidyTextPrivate TidyTextPrivate;
typedef struct _TidyTextClass   TidyTextClass;

struct _TidyText
{
  ClutterLabel     parent_instance;

  /*< private >*/
  TidyTextPrivate *priv;
};

struct _TidyTextClass
{
  ClutterLabelClass parent_class;

  void (* text_changed) (TidyText        *text);
  void (* activate)     (TidyText        *text);
  void (* cursor_event) (TidyText        *text,
                         ClutterGeometry *geometry);
};

GType tidy_text_get_type (void) G_GNUC_CONST;

ClutterActor *tidy_text_new_full            (const gchar        *font_name,
                                             const gchar        *text,
                                             const ClutterColor *color);
ClutterActor *tidy_text_new_with_text       (const gchar        *font_name,
                                             const gchar        *text);

void          tidy_text_set_editable        (TidyText           *label,
                                             gboolean            editable);
gboolean      tidy_text_get_editable        (TidyText           *label);
void          tidy_text_set_activatable     (TidyText           *label,
                                             gboolean            activatable);
gboolean      tidy_text_get_activatable      (TidyText          *label);

gint          tidy_text_get_cursor_position (TidyText           *label);
void          tidy_text_set_cursor_position (TidyText           *label,
                                             gint                position);
void          tidy_text_set_cursor_visible  (TidyText           *label,
                                             gboolean            cursor_visible);
gboolean      tidy_text_get_cursor_visible  (TidyText           *label);
void          tidy_text_set_cursor_color    (TidyText           *text,
                                             const ClutterColor *color);
void          tidy_text_get_cursor_color    (TidyText           *text,
                                             ClutterColor       *color);
void          tidy_text_set_selectable      (TidyText           *label,
                                             gboolean            selectable);
gboolean      tidy_text_get_selectable      (TidyText           *label);
void          tidy_text_set_selection_bound (TidyText           *text,
                                             gint                selection_bound);
gint          tidy_text_get_selection_bound (TidyText           *text);
gchar *       tidy_text_get_selection       (TidyText           *text);
void          tidy_text_insert_unichar      (TidyText           *ttext,
                                             gunichar            wc);


/* add a custom action that can be used in keybindings */
void tidy_text_add_action (TidyText    *ttext,
                           const gchar *name,
                           gboolean (*func) (TidyText            *ttext,
                                             const gchar         *commandline,
                                             ClutterEvent        *event));

/* invoke an action registered by you or one of the tidy text default actions */
gboolean tidy_text_action      (TidyText            *ttext,
                                const gchar         *commandline,
                                ClutterEvent        *event);

void     tidy_text_mappings_clear (TidyText *ttext);

/* Add a keybinding to handle for the default keypress vfunc handler */
void     tidy_text_add_mapping (TidyText           *ttext,
                                guint               keyval,
                                ClutterModifierType state,
                                const gchar        *commandline);

G_END_DECLS

#endif /* __TIDY_TEXT_H__ */
