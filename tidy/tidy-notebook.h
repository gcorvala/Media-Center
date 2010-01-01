/* tidy-notebook.h: Notebook actor
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

#ifndef __TIDY_NOTEBOOK_H__
#define __TIDY_NOTEBOOK_H__

#include <tidy/tidy-frame.h>

G_BEGIN_DECLS

#define TIDY_TYPE_NOTEBOOK                (tidy_notebook_get_type ())
#define TIDY_NOTEBOOK(obj)                (G_TYPE_CHECK_INSTANCE_CAST ((obj), TIDY_TYPE_NOTEBOOK, TidyNotebook))
#define TIDY_IS_NOTEBOOK(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TIDY_TYPE_NOTEBOOK))
#define TIDY_NOTEBOOK_CLASS(klass)        (G_TYPE_CHECK_CLASS_CAST ((klass), TIDY_TYPE_NOTEBOOK, TidyNotebookClass))
#define TIDY_IS_NOTEBOOK_CLASS(klass)     (G_TYPE_CHECK_CLASS_TYPE ((klass), TIDY_TYPE_NOTEBOOK))
#define TIDY_NOTEBOOK_GET_CLASS(obj)      (G_TYPE_INSTANCE_GET_CLASS ((obj), TIDY_TYPE_NOTEBOOK, TidyNotebookClass))

typedef enum {
  TIDY_POS_TOP,
  TIDY_POS_BOTTOM,
  TIDY_POS_LEFT,
  TIDY_POS_RIGHT,
} TidyPosition;

typedef struct _TidyNotebook              TidyNotebook;
typedef struct _TidyNotebookPrivate       TidyNotebookPrivate;
typedef struct _TidyNotebookClass         TidyNotebookClass;

struct _TidyNotebook
{
  TidyActor parent_instance;

  TidyNotebookPrivate *priv;
};

struct _TidyNotebookClass
{
  TidyActorClass parent_class;

  /* vfuncs */
  void (* transition)          (TidyNotebook *notebook,
                                ClutterActor *old_page,
                                ClutterActor *new_page);

  /* signals */
  void (* page_added)          (TidyNotebook *notebook,
                                guint         page_num);

  void (* page_removed)        (TidyNotebook *notebook,
                                guint         page_num);

  void (* page_reordered)      (TidyNotebook *notebook,
                                guint         old_page_num,
                                guint         new_page_num);
};

GType tidy_notebook_get_type (void) G_GNUC_CONST;

ClutterActor *    tidy_notebook_new                (void);

void              tidy_notebook_insert_page        (TidyNotebook *notebook,
                                                    ClutterActor *label,
                                                    ClutterActor *page,
                                                    gint          position);

void              tidy_notebook_reorder_page       (TidyNotebook *notebook,
                                                    gint          position,
                                                    gint          new_position);

ClutterActor *    tidy_notebook_get_nth_page       (TidyNotebook *notebook,
                                                    gint          page_num);

ClutterActor *    tidy_notebook_get_nth_label      (TidyNotebook *notebook,
                                                    gint          page_num);

gint              tidy_notebook_get_n_pages        (TidyNotebook *notebook);

void              tidy_notebook_remove_page        (TidyNotebook *notebook,
                                                    gint          page_num);

void              tidy_notebook_set_tab_position   (TidyNotebook *notebook,
                                                    TidyPosition  position);

void              tidy_notebook_set_tab_alignment  (TidyNotebook *notebook,
                                                    gdouble       alignment);

void              tidy_notebook_set_scrollable     (TidyNotebook *notebook,
                                                    gboolean      scrollable);

TidyPosition      tidy_notebook_get_tab_position   (TidyNotebook *notebook);

gdouble           tidy_notebook_get_tab_alignment  (TidyNotebook *notebook);

gboolean          tidy_notebook_get_scrollable     (TidyNotebook *notebook);

void              tidy_notebook_set_current_page   (TidyNotebook *notebook,
                                                    gint          page_num);

gint              tidy_notebook_get_current_page   (TidyNotebook *notebook);

void              tidy_notebook_transition_complete(TidyNotebook *notebook,
                                                    ClutterActor *old_page);

G_END_DECLS

#endif /* __TIDY_NOTEBOOK_H__ */

