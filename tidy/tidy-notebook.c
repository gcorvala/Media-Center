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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <string.h>

#include <glib.h>

#include "tidy-notebook.h"

#include <clutter/clutter.h>
#include "tidy-button.h"
#include "tidy-debug.h"
#include "tidy-enum-types.h"
#include "tidy-marshal.h"
#include "tidy-stylable.h"
#include "tidy-viewport.h"

enum
{
  PROP_0,

  PROP_CURRENT_PAGE,
  PROP_TAB_POSITION,
  PROP_TAB_ALIGNMENT,
  PROP_SCROLLABLE,
};

enum
{
  PAGE_ADDED,
  PAGE_REMOVED,
  PAGE_REORDERED,

  LAST_SIGNAL
};

#define TIDY_NOTEBOOK_GET_PRIVATE(obj) \
        (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
        TIDY_TYPE_NOTEBOOK, \
        TidyNotebookPrivate))

typedef struct {
  ClutterActor *page;
  ClutterActor *label;
  ClutterActor *selector;
  gboolean      draw;
} TidyNotebookChild;

struct _TidyNotebookPrivate
{
  TidyPosition           tab_position;
  ClutterFixed           tab_alignment;
  gboolean               scrollable;
  
  guint                  current_page;
  
  GList                 *children;
  
  ClutterActor          *internal_group;
  ClutterActor          *label_viewport;
  ClutterActor          *back_button;
  ClutterActor          *forward_button;
  
  ClutterActorBox        allocation;
  ClutterActorBox        child_allocation;
  
  GHashTable            *transition_timelines;
  ClutterEffectTemplate *effect_template;
};

static guint signals[LAST_SIGNAL] = { 0, };

G_DEFINE_TYPE (TidyNotebook, tidy_notebook, TIDY_TYPE_ACTOR);

static void
tidy_notebook_set_property (GObject      *gobject,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  TidyNotebook *notebook = TIDY_NOTEBOOK (gobject);

  switch (prop_id)
    {
    case PROP_CURRENT_PAGE:
      tidy_notebook_set_current_page (notebook, g_value_get_int (value));
      break;
    
    case PROP_TAB_POSITION:
      tidy_notebook_set_tab_position (notebook, g_value_get_enum (value));
      break;

    case PROP_TAB_ALIGNMENT:
      tidy_notebook_set_tab_alignment (notebook, g_value_get_double (value));
      break;

    case PROP_SCROLLABLE:
      tidy_notebook_set_scrollable (notebook, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_notebook_get_property (GObject    *gobject,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  TidyNotebookPrivate *priv = TIDY_NOTEBOOK (gobject)->priv;

  switch (prop_id)
    {
    case PROP_CURRENT_PAGE:
      g_value_set_int (value, (gint)priv->current_page);
      break;
      
    case PROP_TAB_POSITION:
      g_value_set_enum (value, priv->tab_position);
      break;

    case PROP_TAB_ALIGNMENT:
      g_value_set_double (value, CLUTTER_FIXED_TO_FLOAT (priv->tab_alignment));
      break;

    case PROP_SCROLLABLE:
      g_value_set_boolean (value, priv->scrollable);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_notebook_finalize (GObject *gobject)
{
  TidyNotebookPrivate *priv = TIDY_NOTEBOOK (gobject)->priv;
  
  if (priv->transition_timelines)
    g_hash_table_destroy (priv->transition_timelines);
  
  if (priv->effect_template)
    g_object_unref (priv->effect_template);
  
  G_OBJECT_CLASS (tidy_notebook_parent_class)->finalize (gobject);
}

static void
tidy_notebook_dispose (GObject *gobject)
{
  TidyNotebook *notebook = TIDY_NOTEBOOK (gobject);

  while (notebook->priv->children)
    tidy_notebook_remove_page (notebook, 0);

  G_OBJECT_CLASS (tidy_notebook_parent_class)->dispose (gobject);
}

static void
tidy_notebook_paint (ClutterActor *actor)
{
  GList *c;

  TidyNotebookPrivate *priv = TIDY_NOTEBOOK (actor)->priv;
  
  /* Paint the labels, scroll-buttons, focus-marker */
  clutter_actor_paint (priv->internal_group);
  
  /* Paint current page and transitioning pages */
  for (c = priv->children; c; c = c->next)
    {
      TidyNotebookChild *child = (TidyNotebookChild *)c->data;
      if (child->draw && CLUTTER_ACTOR_IS_VISIBLE (child->page))
        clutter_actor_paint (child->page);
    }
}

static void
tidy_notebook_pick (ClutterActor       *self,
                    const ClutterColor *color)
{
  /* Paint children */
  tidy_notebook_paint (self);
}

static void
tidy_notebook_query_coords (ClutterActor *actor, ClutterActorBox *box)
{
  TidyNotebookPrivate *priv = TIDY_NOTEBOOK (actor)->priv;

  *box = priv->allocation;
}

static void
tidy_notebook_request_coords (ClutterActor *actor, ClutterActorBox *box)
{
  gint i;
  GList *c;
  TidyPadding padding;
  ClutterUnit width, height, label_height, label_width,
              cu_label_width, cu_label_height, viewport_width, viewport_height;
  
  TidyNotebookPrivate *priv = TIDY_NOTEBOOK (actor)->priv;
  
  tidy_actor_get_padding (TIDY_ACTOR (actor), &padding);
  
  /* Find the width/height of the tallest label and cumulative width/height of 
   * all labels;
   */
  cu_label_height = 0;
  cu_label_width = 0;
  label_height = 0;
  label_width = 0;
  i = 0;
  for (c = priv->children; c; c = c->next, i++)
    {
      TidyNotebookChild *child = (TidyNotebookChild *)c->data;
      
      clutter_actor_get_sizeu (child->label, &width, &height);
      
      switch (priv->tab_position)
        {
        case TIDY_POS_TOP :
        case TIDY_POS_BOTTOM :
          clutter_actor_set_xu (child->label, cu_label_width);
          break;
        case TIDY_POS_LEFT :
        case TIDY_POS_RIGHT :
          clutter_actor_set_yu (child->label, cu_label_height);
          break;
        }
      
      cu_label_width += width;
      cu_label_height += height;
      
      if (width > label_width)
        label_width = width;
      if (height > label_height)
        label_height = height;
    }
  
  /* If we need back/forward buttons, adjust max label/height values */
  if (priv->scrollable)
    {
      label_width = MAX (label_width,
                         MAX (clutter_actor_get_widthu (priv->back_button),
                              clutter_actor_get_widthu (priv->forward_button)));
      label_height = MAX (label_height,
                         MAX (clutter_actor_get_heightu (priv->back_button),
                             clutter_actor_get_heightu (priv->forward_button)));
    }
  
  /* See if the children push the size any higher */
  for (c = priv->children; c; c = c->next)
    {
      TidyNotebookChild *child = (TidyNotebookChild *)c->data;
      
      /* Put currently available space for child in width/height */
      width = (box->x2 - box->x1) - (padding.left + padding.right);
      height = (box->y2 - box->y1) - (padding.top + padding.bottom);
      if ((priv->tab_position == TIDY_POS_LEFT) ||
          (priv->tab_position == TIDY_POS_RIGHT))
        width -= label_width;
      else
        height -= label_height;

      /* Request it from child and see if it sticks */
      clutter_actor_set_sizeu (child->page, width, height);
      clutter_actor_get_sizeu (child->page, &width, &height);
      
      /* Adjust back for padding/tab labels */
      width += padding.left + padding.right;
      height += padding.top + padding.bottom;

      if ((priv->tab_position == TIDY_POS_LEFT) ||
          (priv->tab_position == TIDY_POS_RIGHT))
        width += label_width;
      else
        height += label_height;

      /* Adjust current allocation if necessary */
      if (width > (box->x2 - box->x1))
        box->x2 = box->x1 + width;
      if (height > (box->y2 - box->y1))
        box->y2 = box->y1 + height;
    }
  
  /* Set the position of the label viewport */
  switch (priv->tab_position)
    {
    case TIDY_POS_TOP :
      clutter_actor_set_positionu (priv->label_viewport,
                                   padding.left, padding.top);
      break;
    case TIDY_POS_BOTTOM :
      clutter_actor_set_positionu (priv->label_viewport, padding.left,
                                   (box->y2 - box->y1) - label_height -
                                   padding.bottom);
      break;
    case TIDY_POS_LEFT :
      clutter_actor_set_positionu (priv->label_viewport,
                                   padding.left, padding.top);
      break;
    case TIDY_POS_RIGHT :
      clutter_actor_set_positionu (priv->label_viewport,
                                   (box->x2 - box->x1) - label_width -
                                   padding.right, padding.top);
      break;
    }
  
  /* Adjust the size for the label widgets */
  if ((priv->tab_position == TIDY_POS_TOP) ||
      (priv->tab_position == TIDY_POS_BOTTOM))
    {
      viewport_width = cu_label_width;
      viewport_height = label_height;
    }
  else
    {
      viewport_width = label_width;
      viewport_height = cu_label_height;
    }
  if (!priv->scrollable)
    {
      if ((priv->tab_position == TIDY_POS_TOP) ||
          (priv->tab_position == TIDY_POS_BOTTOM))
        {
          cu_label_width += padding.left + padding.right;
          if (cu_label_width > (box->x2 - box->x1))
            box->x2 = box->x1 + cu_label_width;
        }
      else
        {
          cu_label_height += padding.top + padding.bottom;
          if (cu_label_height > (box->y2 - box->y1))
            box->y2 = box->y1 + cu_label_height;
        }
    }
  else
    {
      /* Reduce the size of the viewport to make room for the scroll-buttons */
      if ((priv->tab_position == TIDY_POS_TOP) ||
          (priv->tab_position == TIDY_POS_BOTTOM))
        {
          viewport_width -= clutter_actor_get_widthu (priv->back_button) +
                            clutter_actor_get_widthu (priv->forward_button);
        }
      else
        {
          viewport_height -= clutter_actor_get_heightu (priv->back_button) +
                             clutter_actor_get_heightu (priv->forward_button);
        }

      if (viewport_width < 0)
        viewport_width = 0;
      if (viewport_height < 0)
        viewport_height = 0;
      
      /* Set the clipping on the labels viewport */
      clutter_actor_set_clipu (priv->label_viewport, 0, 0, viewport_width,
                               viewport_height);
    }
  
  priv->allocation = *box;
  
  /* Request coords on children */
  priv->child_allocation.x1 = padding.left;
  priv->child_allocation.y1 = padding.top;
  priv->child_allocation.x2 = box->x2 - box->x1 - padding.right;
  priv->child_allocation.y2 = box->y2 - box->y1 - padding.bottom;
  switch (priv->tab_position)
    {
    case TIDY_POS_TOP :
      priv->child_allocation.y1 += viewport_height;
      break;
    case TIDY_POS_BOTTOM :
      priv->child_allocation.y2 -= viewport_height;
      break;
    case TIDY_POS_LEFT :
      priv->child_allocation.x1 += viewport_width;
      break;
    case TIDY_POS_RIGHT :
      priv->child_allocation.x2 -= viewport_width;
      break;
    }
  for (c = priv->children; c; c = c->next)
    {
      ClutterActorBox child_allocation = priv->child_allocation;
      TidyNotebookChild *child = (TidyNotebookChild *)c->data;
      clutter_actor_request_coords (child->page, &child_allocation);
    }

  CLUTTER_ACTOR_CLASS (tidy_notebook_parent_class)->request_coords (actor, box);
}

static void
tidy_notebook_stop_transition (TidyNotebook *notebook,
                               ClutterActor *page)
{
  TidyNotebookPrivate *priv;
  ClutterTimeline *old_transition;

  priv = notebook->priv;
  
  /* transition_timelines will only exist if the page has switched and the 
   * virtual transition function hasn't been overridden.
   */
  if (!priv->transition_timelines)
    return;

  if ((old_transition = g_hash_table_lookup (priv->transition_timelines,
                                             page)))
    {
      clutter_timeline_stop (old_transition);
      g_hash_table_remove (priv->transition_timelines, page);
      g_object_unref (old_transition);
    }
}

static void
tidy_notebook_fade_complete_cb (ClutterActor *actor,
                                gpointer user_data)
{
  TidyNotebook *notebook = TIDY_NOTEBOOK (user_data);
  
  g_hash_table_remove (notebook->priv->transition_timelines, actor);
  tidy_notebook_transition_complete (notebook, actor);
}

static void
tidy_notebook_real_transition (TidyNotebook *notebook,
                               ClutterActor *old_page,
                               ClutterActor *new_page)
{
  TidyNotebookPrivate *priv;
  ClutterTimeline *timeline;
  
  priv = notebook->priv;
  
  /* Initialise hash-table to hold transitions */
  if (!priv->transition_timelines)
    {
      timeline = clutter_timeline_new_for_duration (300);
      priv->effect_template =
        clutter_effect_template_new (timeline,
                                     CLUTTER_ALPHA_RAMP_INC);
      g_object_unref (timeline);
      
      priv->transition_timelines = g_hash_table_new (NULL, NULL);
    }
  
  /* Stop any old transitions and start new ones */
  if (old_page)
    {
      tidy_notebook_stop_transition (notebook, old_page);
      timeline = clutter_effect_fade (priv->effect_template,
                                      old_page,
                                      0,
                                      tidy_notebook_fade_complete_cb,
                                      notebook);
      g_hash_table_insert (priv->transition_timelines, old_page, timeline);
    }
  
  if (new_page)
    {
      tidy_notebook_stop_transition (notebook, new_page);
      timeline = clutter_effect_fade (priv->effect_template,
                                      new_page,
                                      0xFF,
                                      tidy_notebook_fade_complete_cb,
                                      notebook);
      g_hash_table_insert (priv->transition_timelines, new_page, timeline);
    }
}

static void
tidy_notebook_class_init (TidyNotebookClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  g_type_class_add_private (klass, sizeof (TidyNotebookPrivate));

  gobject_class->set_property = tidy_notebook_set_property;
  gobject_class->get_property = tidy_notebook_get_property;
  gobject_class->dispose = tidy_notebook_dispose;
  gobject_class->finalize = tidy_notebook_finalize;
  
  actor_class->paint = tidy_notebook_paint;
  actor_class->pick = tidy_notebook_pick;
  actor_class->query_coords = tidy_notebook_query_coords;
  actor_class->request_coords = tidy_notebook_request_coords;
  
  klass->transition = tidy_notebook_real_transition;

  g_object_class_install_property (gobject_class,
                                   PROP_CURRENT_PAGE,
                                   g_param_spec_int ("current-page",
                                                     "Current page",
                                                     "Current page number",
                                                     0, G_MAXINT, 0,
                                                     G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_TAB_POSITION,
                                   g_param_spec_enum ("tab-position",
                                                      "Tab position",
                                                      "Position of the tab list",
                                                      TIDY_TYPE_POSITION,
                                                      TIDY_POS_TOP,
                                                      G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_TAB_ALIGNMENT,
                                   g_param_spec_double ("tab-alignment",
                                                        "Tab alignment",
                                                        "Alignment of the tab list",
                                                        0.0, 1.0, 0.0,
                                                        G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class,
                                   PROP_SCROLLABLE,
                                   g_param_spec_boolean ("scrollable",
                                                         "Scrollable",
                                                         "Whether the label "
                                                         "area should have "
                                                         "arrows for scrolling "
                                                         "if there are too "
                                                         "many tabs to fit",
                                                         TRUE,
                                                         G_PARAM_READWRITE));

  signals[PAGE_ADDED] =
    g_signal_new ("page-added",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (TidyNotebookClass, page_added),
                  NULL, NULL,
                  _tidy_marshal_VOID__UINT,
                  G_TYPE_NONE, 1, G_TYPE_UINT);

  signals[PAGE_REMOVED] =
    g_signal_new ("page-removed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (TidyNotebookClass, page_removed),
                  NULL, NULL,
                  _tidy_marshal_VOID__UINT,
                  G_TYPE_NONE, 1, G_TYPE_UINT);

  signals[PAGE_REORDERED] =
    g_signal_new ("page-reordered",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (TidyNotebookClass, page_reordered),
                  NULL, NULL,
                  _tidy_marshal_VOID__UINT_UINT,
                  G_TYPE_NONE, 2, G_TYPE_UINT, G_TYPE_UINT);
}

static void
tidy_notebook_init (TidyNotebook *notebook)
{
  TidyNotebookPrivate *priv;
  
  priv = notebook->priv = TIDY_NOTEBOOK_GET_PRIVATE (notebook);
  
  priv->internal_group = clutter_group_new ();
  
  priv->label_viewport = tidy_viewport_new ();
  priv->back_button = tidy_button_new_with_label ("←");
  priv->forward_button = tidy_button_new_with_label ("→");
  
  clutter_container_add (CLUTTER_CONTAINER (priv->internal_group),
                         priv->label_viewport,
                         priv->back_button,
                         priv->forward_button,
                         NULL);
  clutter_actor_set_parent (priv->internal_group, CLUTTER_ACTOR (notebook));
  
  clutter_actor_show (priv->label_viewport);
  clutter_actor_show (priv->internal_group);
}

ClutterActor *
tidy_notebook_new (void)
{
  return g_object_new (TIDY_TYPE_NOTEBOOK, NULL);
}

static gboolean
label_button_press_event_cb (ClutterActor *actor,
                             ClutterButtonEvent *event,
                             TidyNotebook *notebook)
{
  gint n;
  GList *c;
  TidyNotebookPrivate *priv;
  
  priv = notebook->priv;
  
  for (c = priv->children, n = 0; c; c = c->next, n++)
    {
      TidyNotebookChild *child = (TidyNotebookChild *)c->data;
      
      if (child->label == actor)
        {
          tidy_notebook_set_current_page (notebook, n);
          break;
        }
    }
  
  return FALSE;
}

static void
relayout (TidyNotebook *notebook)
{
  ClutterActor *actor = CLUTTER_ACTOR (notebook);
  ClutterActorBox box = { 0, };
  CLUTTER_ACTOR_CLASS (tidy_notebook_parent_class)->request_coords (actor,&box);
  clutter_actor_request_coords (actor, &notebook->priv->allocation);
}

static void
label_size_notify_cb (GObject      *label,
                      GParamSpec   *pspec,
                      TidyNotebook *notebook)
{
  /* Re-layout */
  relayout (notebook);
}

void
tidy_notebook_insert_page (TidyNotebook *notebook,
                           ClutterActor *label,
                           ClutterActor *page,
                           gint          position)
{
  TidyNotebookPrivate *priv;
  TidyNotebookChild *child;
  
  g_return_if_fail (TIDY_IS_NOTEBOOK (notebook) && page && label);
  
  priv = notebook->priv;
  
  child = g_slice_alloc (sizeof (TidyNotebookChild));
  
  child->page = page;
  child->label = label;
  clutter_container_add_actor (CLUTTER_CONTAINER (priv->label_viewport),
                               child->label);
  clutter_actor_set_parent (child->page, CLUTTER_ACTOR (notebook));
  
  priv->children = g_list_insert (priv->children, child, position);
  
  /* Hook onto button-press signal for tab switching */
  g_signal_connect (label,
                    "button-press-event",
                    G_CALLBACK (label_button_press_event_cb),
                    notebook);
  
  /* Hook onto width/height change of label to re-layout */
  g_signal_connect (label, "notify::width",
                    G_CALLBACK (label_size_notify_cb), notebook);
  g_signal_connect (label, "notify::height",
                    G_CALLBACK (label_size_notify_cb), notebook);
  
  /* Re-layout */
  relayout (notebook);
  
  /* Select tab if necessary */
  if (position == priv->current_page)
    {
      clutter_actor_set_reactive (label, FALSE);
      child->draw = TRUE;
    }
  else
    {
      clutter_actor_set_reactive (label, TRUE);
      child->draw = FALSE;
    }
  
  clutter_actor_queue_redraw (CLUTTER_ACTOR (notebook));
  
  g_signal_emit (notebook, signals[PAGE_ADDED], 0, position);
}

void
tidy_notebook_reorder_page (TidyNotebook *notebook,
                            gint          position,
                            gint          new_position)
{
  TidyNotebookPrivate *priv;
  TidyNotebookChild *child;
  GList *link;
  
  g_return_if_fail (TIDY_IS_NOTEBOOK (notebook));
  
  priv = notebook->priv;
  
  link = g_list_nth (priv->children, position);
  if (!link)
    return;
    
  child = (TidyNotebookChild *)link->data;
  
  priv->children = g_list_delete_link (priv->children, link);
  
  if (new_position > position)
    priv->children = g_list_insert (priv->children, child, new_position);
  else
    priv->children = g_list_insert (priv->children, child, new_position + 1);

  /* Re-layout */
  relayout (notebook);
  
  /* Select possibly new tab */
  if (priv->current_page == position)
    tidy_notebook_set_current_page (notebook, new_position);

  clutter_actor_queue_redraw (CLUTTER_ACTOR (notebook));

  g_signal_emit (notebook, signals[PAGE_REORDERED], 0, position, new_position);
}

ClutterActor *
tidy_notebook_get_nth_page (TidyNotebook *notebook,
                            gint          page_num)
{
  TidyNotebookChild *child;
  
  g_return_val_if_fail (TIDY_IS_NOTEBOOK (notebook), NULL);
  
  child = g_list_nth_data (notebook->priv->children, page_num);
  
  if (child)
    return child->page;
  else
    return NULL;
}

ClutterActor *
tidy_notebook_get_nth_label (TidyNotebook *notebook,
                             gint          page_num)
{
  TidyNotebookChild *child;
  
  g_return_val_if_fail (TIDY_IS_NOTEBOOK (notebook), NULL);
  
  child = g_list_nth_data (notebook->priv->children, page_num);
  
  if (child)
    return child->label;
  else
    return NULL;
}

gint
tidy_notebook_get_n_pages (TidyNotebook *notebook)
{
  g_return_val_if_fail (TIDY_IS_NOTEBOOK (notebook), 0);
  return g_list_length (notebook->priv->children);
}

void
tidy_notebook_remove_page (TidyNotebook *notebook,
                           gint          page_num)
{
  GList *link;
  TidyNotebookChild *child;
  TidyNotebookPrivate *priv;

  g_return_if_fail (TIDY_IS_NOTEBOOK (notebook));
  
  priv = notebook->priv;
  
  link = g_list_nth (priv->children, page_num);
  if (!link)
    return;
  
  child = (TidyNotebookChild *)link->data;
  
  tidy_notebook_stop_transition (notebook, child->page);
  
  clutter_actor_unparent (child->page);
  g_signal_handlers_disconnect_by_func (child->label,
                                        label_button_press_event_cb,
                                        notebook);
  g_signal_handlers_disconnect_by_func (child->label,
                                        label_size_notify_cb,
                                        notebook);
  clutter_container_remove_actor (CLUTTER_CONTAINER (priv->label_viewport),
                                  child->label);
  g_slice_free (TidyNotebookChild, child);
  
  priv->children = g_list_delete_link (priv->children, link);
  
  /* Re-layout */
  relayout (notebook);
  
  /* Select new tab if necessary */
  if (page_num == priv->current_page)
    {
      child = g_list_nth_data (priv->children, page_num);
      if (child)
        {
          child->draw = TRUE;
          clutter_actor_set_reactive (child->label, FALSE);
          TIDY_NOTEBOOK_CLASS (G_OBJECT_GET_CLASS (notebook))->
            transition (notebook, NULL, child->page);
        }
      else
          if (page_num != 0)
            tidy_notebook_set_current_page (notebook, page_num - 1);
    }

  clutter_actor_queue_redraw (CLUTTER_ACTOR (notebook));

  g_signal_emit (notebook, signals[PAGE_REMOVED], 0, page_num);
}

void
tidy_notebook_set_tab_position (TidyNotebook *notebook,
                                TidyPosition  position)
{
  g_return_if_fail (TIDY_IS_NOTEBOOK (notebook));
  
  notebook->priv->tab_position = position;

  clutter_actor_queue_redraw (CLUTTER_ACTOR (notebook));
}

void
tidy_notebook_set_tab_alignment (TidyNotebook *notebook,
                                 gdouble       alignment)
{
  g_return_if_fail (TIDY_IS_NOTEBOOK (notebook));
  
  notebook->priv->tab_alignment = CLUTTER_FLOAT_TO_FIXED (alignment);

  clutter_actor_queue_redraw (CLUTTER_ACTOR (notebook));
}

void
tidy_notebook_set_scrollable (TidyNotebook *notebook,
                              gboolean      scrollable)
{
  TidyNotebookPrivate *priv;
  
  g_return_if_fail (TIDY_IS_NOTEBOOK (notebook));
  
  priv = notebook->priv;
  
  if (priv->scrollable == scrollable)
    return;
  
  priv->scrollable = scrollable;

  if (!scrollable)
    {
      ClutterActorBox box;

      clutter_actor_remove_clip (priv->label_viewport);
      clutter_actor_hide (priv->back_button);
      clutter_actor_hide (priv->forward_button);

      /* Possibly re-size */
      clutter_actor_query_coords (CLUTTER_ACTOR (notebook), &box);
      clutter_actor_request_coords (CLUTTER_ACTOR (notebook), &box);
    }
  else
    {
      clutter_actor_show (priv->back_button);
      clutter_actor_show (priv->forward_button);
    }
  
  g_object_notify (G_OBJECT (notebook), "scrollable");
}

TidyPosition
tidy_notebook_get_tab_position (TidyNotebook *notebook)
{
  g_return_val_if_fail (TIDY_IS_NOTEBOOK (notebook), TIDY_POS_TOP);
  
  return notebook->priv->tab_position;
}

gdouble
tidy_notebook_get_tab_alignment (TidyNotebook *notebook)
{
  g_return_val_if_fail (TIDY_IS_NOTEBOOK (notebook), 0.0);
  
  return CLUTTER_FIXED_TO_FLOAT (notebook->priv->tab_alignment);
}

gboolean
tidy_notebook_get_scrollable (TidyNotebook *notebook)
{
  g_return_val_if_fail (TIDY_IS_NOTEBOOK (notebook), FALSE);
  
  return notebook->priv->scrollable;
}

void
tidy_notebook_set_current_page (TidyNotebook *notebook,
                                gint          page_num)
{
  TidyNotebookChild *old_child, *new_child;
  TidyNotebookPrivate *priv;
  
  g_return_if_fail (TIDY_IS_NOTEBOOK (notebook));
  
  priv = notebook->priv;
  
  if (priv->current_page == page_num)
    return;
  
  new_child = (TidyNotebookChild *)g_list_nth_data (priv->children, page_num);
  if (!new_child)
    return;
  
  old_child = (TidyNotebookChild *)g_list_nth_data (priv->children,
                                                    priv->current_page);
  if (old_child)
    {
      clutter_actor_set_reactive (old_child->label, TRUE);
    }
  
  priv->current_page = (guint)page_num;
  clutter_actor_set_reactive (new_child->label, FALSE);
  new_child->draw = TRUE;
  
  TIDY_NOTEBOOK_CLASS (G_OBJECT_GET_CLASS (notebook))->
    transition (notebook, old_child->page, new_child->page);
  
  clutter_actor_queue_redraw (CLUTTER_ACTOR (notebook));
  
  g_object_notify (G_OBJECT (notebook), "current-page");
}

gint
tidy_notebook_get_current_page (TidyNotebook *notebook)
{
  g_return_val_if_fail (TIDY_IS_NOTEBOOK (notebook), -1);
  
  return (gint)notebook->priv->current_page;
}

/* Function only to be used by subclasses of TidyNotebook */
void
tidy_notebook_transition_complete (TidyNotebook *notebook,
                                   ClutterActor *old_page)
{
  GList *c;
  TidyNotebookChild *child;
  TidyNotebookPrivate *priv;
  
  priv = notebook->priv;
  
  /* Don't let old transitions finishing hide the current page */
  child = g_list_nth_data (priv->children, priv->current_page);
  if (child)
    if (child->page == old_page)
      return;
  
  for (c = priv->children; c; c = c->next)
    {
      child = (TidyNotebookChild *)c->data;
      if (child->page == old_page)
        {
          child->draw = FALSE;
          break;
        }
    }
}

