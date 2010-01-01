/* tidy-cell-renderer-default.c: Default cell renderer
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

#include <glib.h>

#include <clutter/clutter-actor.h>
#include <clutter/clutter-container.h>
#include <clutter/clutter-group.h>
#include <clutter/clutter-label.h>
#include <clutter/clutter-rectangle.h>
#include <clutter/clutter-texture.h>

#include "tidy-actor.h"
#include "tidy-cell-renderer.h"
#include "tidy-cell-renderer-default.h"

#include "tidy-debug.h"
#include "tidy-enum-types.h"
#include "tidy-frame.h"
#include "tidy-private.h"
#include "tidy-stylable.h"

/**
 * SECTION:tidy-cell-renderer-default
 * @short_description: Default cell renderer
 *
 * #TidyCellRendererDefault is a simple implementation of the
 * #TidyCellRenderer class. It provides a default renderer for
 * some commonly used data types, like:
 * <itemizedlist>
 *   <listitem><para>strings, using a #ClutterLabel</para></listitem>
 *   <listitem><para>booleans, using a #ClutterRectangle</para></listitem>
 *   <listitem><para>pixbufs, using a #ClutterTexture</para></listitem>
 * </itemizedlist>
 *
 * #TidyListView uses #TidyCellRendererDefault as the default renderer
 * for its columns. See #TidyListColumn documentation for information
 * on how to use your own #TidyCellRenderer implementation.
 *
 * #TidyCellRendererDefault is a final class, and cannot be subclassed.
 */

#define TIDY_CELL_RENDERER_DEFAULT_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_CAST ((klass), \
         TIDY_TYPE_CELL_RENDERER_DEFAULT, \
         TidyCellRendererDefaultClass))
#define TIDY_IS_CELL_RENDERER_DEFAULT_CLASS(klass) \
        (G_TYPE_CHECK_CLASS_TYPE ((klass), \
         TIDY_TYPE_CELL_RENDERER_DEFAULT))
#define TIDY_CELL_RENDERER_DEFAULT_GET_CLASS(obj) \
        (G_TYPE_INSTANCE_GET_CLASS ((obj), \
         TIDY_TYPE_CELL_RENDERER_DEFAULT, \
         TidyCellRendererDefaultClass))

typedef struct _TidyCellRendererClass   TidyCellRendererDefaultClass;

struct _TidyCellRendererDefault
{
  TidyCellRenderer parent_instance;
};

G_DEFINE_TYPE (TidyCellRendererDefault,
               tidy_cell_renderer_default,
               TIDY_TYPE_CELL_RENDERER);

static ClutterActor *
tidy_cell_renderer_default_get_cell_actor (TidyCellRenderer *renderer,
                                           TidyActor        *list_view,
                                           const GValue     *value,
                                           TidyCellState     state,
                                           ClutterGeometry  *size,
                                           gint              row,
                                           gint              column)
{
  ClutterActor *retval = NULL;
  gchar *font_name;
  ClutterColor *background_color, *active_color, *text_color;
  ClutterFixed x_align, y_align;

  tidy_stylable_get (TIDY_STYLABLE (list_view),
                     "font-name", &font_name,
                     "bg-color", &background_color,
                     "active-color", &active_color,
                     "text-color", &text_color,
                     NULL);

  tidy_cell_renderer_get_alignmentx (renderer, &x_align, &y_align);

  /* header or any other row themed like the header */
  if (row == -1 || state == TIDY_CELL_HEADER)
    {
      ClutterActor *label;

      retval = tidy_frame_new ();
      tidy_actor_set_alignmentx (TIDY_ACTOR (retval), x_align, y_align);

      label = g_object_new (CLUTTER_TYPE_LABEL,
                            "font-name", font_name,
                            "text", g_value_get_string (value),
                            "color", text_color,
                            "alignment", PANGO_ALIGN_CENTER,
                            "ellipsize", PANGO_ELLIPSIZE_END,
                            "wrap", FALSE,
                            NULL);
      clutter_container_add_actor (CLUTTER_CONTAINER (retval), label);
      clutter_actor_show (label);

      goto out;
    }

  switch (G_TYPE_FUNDAMENTAL (G_VALUE_TYPE (value)))
    {
    case G_TYPE_BOOLEAN:
      {
        ClutterActor *rect;

        retval = clutter_group_new ();

        if (size->width)
          clutter_actor_set_width (retval, size->width);

        rect = clutter_rectangle_new ();
        clutter_actor_set_size (rect, 12, 12);

        if (g_value_get_boolean (value))
          {
            clutter_rectangle_set_color (CLUTTER_RECTANGLE (rect),
                                         background_color);
            clutter_rectangle_set_border_width (CLUTTER_RECTANGLE (rect), 2);
            clutter_rectangle_set_border_color (CLUTTER_RECTANGLE (rect),
                                                active_color);
          }
        else
          {
            clutter_rectangle_set_color (CLUTTER_RECTANGLE (rect),
                                         active_color);
          }

        clutter_container_add_actor (CLUTTER_CONTAINER (retval), rect);

        if (size)
          clutter_actor_set_position (rect, ((size->width - 12) * 0.5), 0);

        clutter_actor_show (rect);
      }
      break;

    case G_TYPE_ENUM:
      {
        gint ev = g_value_get_enum (value);
        GEnumClass *enum_class = g_type_class_ref (G_VALUE_TYPE (value));
        GEnumValue *enum_value = g_enum_get_value (enum_class, ev);
        
        retval = g_object_new (CLUTTER_TYPE_LABEL,
                               "font-name", font_name,
                               "text", enum_value->value_nick,
                               "color", text_color,
                               "alignment", PANGO_ALIGN_CENTER,
                               "ellipsize", PANGO_ELLIPSIZE_END,
                               "use-markup", TRUE,
                               "wrap", FALSE,
                               NULL);
        
        g_type_class_unref (enum_class);
      }
      break;
    
    case G_TYPE_INT:
      {
        gchar *str = g_strdup_printf ("%d", g_value_get_int (value));

        retval = g_object_new (CLUTTER_TYPE_LABEL,
                               "font-name", font_name,
                               "text", str,
                               "color", text_color,
                               "alignment", PANGO_ALIGN_CENTER,
                               "ellipsize", PANGO_ELLIPSIZE_END,
                               "use-markup", TRUE,
                               "wrap", FALSE,
                               NULL);

        g_free (str);
      }
      break;

    case G_TYPE_STRING:
      retval = g_object_new (CLUTTER_TYPE_LABEL,
                             "font-name", font_name,
                             "text", g_value_get_string (value),
                             "color", text_color,
                             "alignment", PANGO_ALIGN_RIGHT,
                             "ellipsize", PANGO_ELLIPSIZE_END,
                             "use-markup", TRUE,
                             "wrap", FALSE,
                             NULL);
      break;
    }

out:
  g_free (font_name);
  clutter_color_free (background_color);
  clutter_color_free (active_color);
  clutter_color_free (text_color);

  return retval;
}

static void
tidy_cell_renderer_default_class_init (TidyCellRendererDefaultClass *klass)
{
  TidyCellRendererClass *renderer_class = TIDY_CELL_RENDERER_CLASS (klass);

  renderer_class->get_cell_actor = tidy_cell_renderer_default_get_cell_actor;
}

static void
tidy_cell_renderer_default_init (TidyCellRendererDefault *renderer)
{

}

TidyCellRenderer *
tidy_cell_renderer_default_new (void)
{
  return g_object_new (TIDY_TYPE_CELL_RENDERER_DEFAULT, NULL);
}
