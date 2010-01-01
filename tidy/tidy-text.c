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

/* TODO: undo/redo hooks?
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <clutter/clutter.h>
#include "tidy-text.h"
#include <string.h>



static gboolean tidy_text_key_press          (ClutterActor    *actor,
                                              ClutterKeyEvent *kev);
static gboolean tidy_text_position_to_coords (TidyText        *ttext,
                                              gint             position,
                                              gint            *x,
                                              gint            *y,
                                              gint            *cursor_height);
static gint     tidy_text_coords_to_position (TidyText        *text,
                                              gint             x,
                                              gint             y);
static void     tidy_text_set_property       (GObject         *gobject,
                                              guint            prop_id,
                                              const GValue    *value,
                                              GParamSpec      *pspec);
static void     tidy_text_get_property       (GObject         *gobject,
                                              guint            prop_id,
                                              GValue          *value,
                                              GParamSpec      *pspec);
static void     tidy_text_finalize           (GObject         *gobject);

static void init_commands (TidyText *ttext);
static void init_mappings (TidyText *ttext);

void
tidy_text_delete_text (TidyText *ttext,
                       gssize    start_pos,
                       gssize    end_pos);

static gboolean
tidy_text_truncate_selection (TidyText     *ttext,
                              const gchar  *commandline,
                              ClutterEvent *event);

G_DEFINE_TYPE (TidyText, tidy_text, CLUTTER_TYPE_LABEL);

struct _TidyTextPrivate
{
  gboolean        editable;
  gboolean        cursor_visible;
  gboolean        activatable;
  gboolean        selectable;

  gint            position;   /* current cursor position */
  gint            selection_bound; 
                              /* current 'other end of selection' position */
  gboolean        in_select_drag;
  gint            x_pos;      /* the x position in the pangolayout, used to
                               * avoid drifting when repeatedly moving up|down
                               */
  gboolean        cursor_color_set;
  ClutterColor    cursor_color;
  ClutterGeometry cursor_pos; /* Where to draw the cursor */

  GList          *mappings;
  GList          *commands; /* each instance has it's own set of commands
                               so that actor specific actions can be added
                               to single actor classes
                              */
};

#define TIDY_TEXT_GET_PRIVATE(obj)                 \
              (G_TYPE_INSTANCE_GET_PRIVATE ((obj),  \
               TIDY_TYPE_TEXT,                     \
               TidyTextPrivate))

enum
{
  PROP_0,
  PROP_POSITION,
  PROP_SELECTION_BOUND,
  PROP_CURSOR_VISIBLE,
  PROP_CURSOR_COLOR,
  PROP_CURSOR_COLOR_SET,
  PROP_EDITABLE,
  PROP_SELECTABLE,
  PROP_ACTIVATABLE
};

enum
{
  TEXT_CHANGED,
  CURSOR_EVENT,
  ACTIVATE,
  LAST_SIGNAL
};

static guint label_signals[LAST_SIGNAL] = { 0, };

#define offset_real(text, pos)                                \
   (pos==-1?g_utf8_strlen(text, -1):pos)                          \

#define offset_to_bytes(text,pos)\
   (pos==-1?strlen(text):((gint)(g_utf8_offset_to_pointer (text, pos) - text)))

#define bytes_to_offset(text, pos)                            \
    (g_utf8_pointer_to_offset (text, text + pos))


typedef struct TextCommand {
  const gchar *name;
  gboolean (*func) (TidyText     *ttext,
                    const gchar  *commandline,
                    ClutterEvent *event);
} TextCommand;

typedef struct TidyTextMapping {
  ClutterModifierType    state;
  guint                  keyval;
  const gchar           *action;
} TidyTextMapping;


void
tidy_text_mappings_clear (TidyText *ttext)
{
  TidyTextPrivate *priv = ttext->priv;
  GList *iter;
  for (iter = priv->mappings; iter; iter=iter->next)
    {
      g_free (iter->data);
    }
  g_list_free (priv->mappings);
  priv->mappings = NULL;
}

void tidy_text_add_mapping (TidyText           *ttext,
                            guint               keyval,
                            ClutterModifierType state,
                            const gchar        *commandline)
{
  TidyTextMapping *tmapping = g_new (TidyTextMapping, 1);
  TidyTextPrivate *priv = ttext->priv;
  tmapping->keyval = keyval;
  tmapping->state = state;
  tmapping->action = commandline;
  priv->mappings = g_list_append (priv->mappings, tmapping);
}

void tidy_text_add_action (TidyText    *ttext,
                           const gchar *name,
                           gboolean (*func) (TidyText            *ttext,
                                             const gchar         *commandline,
                                             ClutterEvent        *event))
{
  TextCommand *tcommand = g_new (TextCommand, 1);
  TidyTextPrivate *priv = ttext->priv;
  tcommand->name = name;
  tcommand->func = func;
  priv->commands = g_list_append (priv->commands, tcommand);
}

static void init_mappings (TidyText *ttext)
{
  TidyTextPrivate *priv = ttext->priv;
  if (priv->mappings)
    return;
  tidy_text_add_mapping (ttext, CLUTTER_Left, 0,    "move-left");
  tidy_text_add_mapping (ttext, CLUTTER_KP_Left, 0, "move-left");
  tidy_text_add_mapping (ttext, CLUTTER_Right, 0,   "move-right");
  tidy_text_add_mapping (ttext, CLUTTER_KP_Right, 0,"move-right");
  tidy_text_add_mapping (ttext, CLUTTER_Up, 0,      "move-up");
  tidy_text_add_mapping (ttext, CLUTTER_KP_Up, 0,   "move-up");
  tidy_text_add_mapping (ttext, CLUTTER_Down, 0,    "move-down");
  tidy_text_add_mapping (ttext, CLUTTER_KP_Down, 0, "move-down");
  tidy_text_add_mapping (ttext, CLUTTER_Begin, 0,   "move-start-line");
  tidy_text_add_mapping (ttext, CLUTTER_Home, 0,    "move-start-line");
  tidy_text_add_mapping (ttext, CLUTTER_KP_Home, 0, "move-start-line");
  tidy_text_add_mapping (ttext, CLUTTER_End, 0,     "move-end-line");
  tidy_text_add_mapping (ttext, CLUTTER_KP_End, 0,  "move-end-line");
  tidy_text_add_mapping (ttext, CLUTTER_BackSpace,0,"delete-previous");
  tidy_text_add_mapping (ttext, CLUTTER_Delete, 0,  "delete-next");
  tidy_text_add_mapping (ttext, CLUTTER_KP_Delete,0,"delete-next");
  tidy_text_add_mapping (ttext, CLUTTER_Return, 0,  "activate");
  tidy_text_add_mapping (ttext, CLUTTER_KP_Enter, 0,"activate");
  tidy_text_add_mapping (ttext, CLUTTER_ISO_Enter,0,"activate");
}


static gint
tidy_text_coords_to_position (TidyText *text,
                              gint      x,
                              gint      y)
{
  gint index_;
  gint px, py;
  gint trailing;

  px = x * PANGO_SCALE;
  py = y * PANGO_SCALE;

  pango_layout_xy_to_index (clutter_label_get_layout (CLUTTER_LABEL (text)),
                            px, py, &index_, &trailing);

  return index_ + trailing;
}

static gboolean
tidy_text_position_to_coords (TidyText *ttext,
                              gint      position,
                              gint     *x,
                              gint     *y,
                              gint     *cursor_height)
{
  TidyTextPrivate  *priv;
  gint              index_;
  PangoRectangle    rect;
  const gchar      *text;

  text = clutter_label_get_text (CLUTTER_LABEL (ttext));

  priv = ttext->priv;

  if (position == -1)
    {
      index_ = strlen (text);
    }
  else
    {
      index_ = offset_to_bytes (text, position);
    }

  if (index_ > strlen (text))
   index_ = strlen (text);

  pango_layout_get_cursor_pos (
        clutter_label_get_layout (CLUTTER_LABEL (ttext)),
        index_, &rect, NULL);

  if (x)
    *x = rect.x / PANGO_SCALE;
  if (y)
    *y = (rect.y + rect.height) / PANGO_SCALE;
  if (cursor_height)
    *cursor_height = rect.height / PANGO_SCALE;

  return TRUE; /* FIXME: should return false if coords were outside text */
}

static void
tidy_text_ensure_cursor_position (TidyText *ttext)
{
  gint x,y,cursor_height;
  
  TidyTextPrivate  *priv;
  priv = ttext->priv;

  tidy_text_position_to_coords (ttext, priv->position, &x, &y, &cursor_height);

  priv->cursor_pos.x = x;
  priv->cursor_pos.y = y - cursor_height;
  priv->cursor_pos.width = 2; 
  priv->cursor_pos.height = cursor_height;

  g_signal_emit (ttext, label_signals[CURSOR_EVENT], 0, &priv->cursor_pos);
}

gint
tidy_text_get_cursor_position (TidyText *ttext)
{
  g_return_val_if_fail (TIDY_IS_TEXT (ttext), -1);
  return ttext->priv->position;
}

void
tidy_text_set_cursor_position (TidyText *ttext,
                               gint       position)
{
  ClutterLabel        *label = CLUTTER_LABEL (ttext);
  const gchar         *text;
  TidyTextPrivate *priv;
  gint len;

  g_return_if_fail (TIDY_IS_TEXT (ttext));

  priv = ttext->priv;

  text = clutter_label_get_text (label);
  if (text == NULL)
    return;

  len = g_utf8_strlen (text, -1);

  if (position < 0 || position >= len)
    priv->position = -1;
  else
    priv->position = position;

  if (CLUTTER_ACTOR_IS_VISIBLE (ttext))
    clutter_actor_queue_redraw (CLUTTER_ACTOR (ttext));
}

static gboolean
tidy_text_truncate_selection (TidyText     *ttext,
                              const gchar  *commandline,
                              ClutterEvent *event)
{
  const gchar *utf8 = clutter_label_get_text (CLUTTER_LABEL (ttext));
  TidyTextPrivate *priv;
  gint             start_index;
  gint             end_index;

  priv = ttext->priv;

  g_object_ref (ttext);

  start_index = offset_real (utf8, priv->position);
  end_index = offset_real (utf8, priv->selection_bound);

  if (end_index == start_index)
    return FALSE;

  if (end_index < start_index)
    {
      gint temp = start_index;
      start_index = end_index;
      end_index = temp;
    }

  tidy_text_delete_text (ttext, start_index, end_index);
  priv->position = start_index;
  priv->selection_bound = start_index;
  return TRUE;
}


void
tidy_text_insert_unichar (TidyText *ttext,
                           gunichar   wc)
{
  TidyTextPrivate *priv;
  GString *new = NULL;
  const gchar *old_text;
  glong pos;

  g_return_if_fail (TIDY_IS_TEXT (ttext));
  g_return_if_fail (g_unichar_validate (wc));

  if (wc == 0)
    return;

  tidy_text_truncate_selection (ttext, NULL, 0);

  priv = ttext->priv;

  g_object_ref (ttext);

  old_text = clutter_label_get_text (CLUTTER_LABEL (ttext));


  new = g_string_new (old_text);
  pos = offset_to_bytes (old_text, priv->position);
  new = g_string_insert_unichar (new, pos, wc);

  clutter_label_set_text (CLUTTER_LABEL (ttext), new->str);

  if (priv->position >= 0)
    {
      tidy_text_set_cursor_position (ttext, priv->position + 1);
      tidy_text_set_selection_bound (ttext, priv->position);
    }

  g_string_free (new, TRUE);

  g_object_unref (ttext);

  g_signal_emit (G_OBJECT (ttext), label_signals[TEXT_CHANGED], 0);
}

void
tidy_text_delete_text (TidyText *ttext,
                       gssize    start_pos,
                       gssize    end_pos)
{
  TidyTextPrivate *priv;
  GString *new = NULL;
  gint start_bytes;
  gint end_bytes;
  const gchar *text;

  g_return_if_fail (TIDY_IS_TEXT (ttext));

  priv = ttext->priv;
  text = clutter_label_get_text (CLUTTER_LABEL (ttext));

  if (end_pos == -1)
    {
      start_bytes = offset_to_bytes (text, g_utf8_strlen (text, -1) - 1);
      end_bytes = offset_to_bytes (text, g_utf8_strlen (text, -1));
    }
  else
    {
      start_bytes = offset_to_bytes (text, start_pos);
      end_bytes = offset_to_bytes (text, end_pos);
    }

  new = g_string_new (text);

  new = g_string_erase (new, start_bytes, end_bytes - start_bytes);

  clutter_label_set_text (CLUTTER_LABEL (ttext), new->str);

  g_string_free (new, TRUE);
  g_signal_emit (G_OBJECT (ttext), label_signals[TEXT_CHANGED], 0);
}

static void
tidy_text_finalize (GObject *gobject)
{
  TidyTextPrivate *priv;
  TidyText        *ttext;
  GList           *iter;

  ttext = TIDY_TEXT (gobject);
  priv = ttext->priv;

  tidy_text_mappings_clear (ttext);

  for (iter = priv->commands; iter; iter=iter->next)
      g_free (iter->data);
  g_list_free (priv->commands);
  priv->commands = NULL;
}

static void
tidy_text_set_property (GObject      *gobject,
                        guint         prop_id,
                        const GValue *value,
                        GParamSpec   *pspec)
{
  TidyTextPrivate *priv;
  TidyText        *ttext;

  ttext = TIDY_TEXT (gobject);
  priv = ttext->priv;

  switch (prop_id)
    {
    case PROP_POSITION:
      tidy_text_set_cursor_position (ttext, g_value_get_int (value));
      break;
    case PROP_SELECTION_BOUND:
      tidy_text_set_selection_bound (ttext, g_value_get_int (value));
      break;
    case PROP_CURSOR_VISIBLE:
      tidy_text_set_cursor_visible (ttext, g_value_get_boolean (value));
      break;
    case PROP_CURSOR_COLOR:
      tidy_text_set_cursor_color (ttext, g_value_get_boxed (value));
      break;
    case PROP_EDITABLE:
      tidy_text_set_editable (ttext, g_value_get_boolean (value));
      break;
    case PROP_ACTIVATABLE:
      tidy_text_set_activatable (ttext, g_value_get_boolean (value));
      break;
    case PROP_SELECTABLE:
      tidy_text_set_selectable (ttext, g_value_get_boolean (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
tidy_text_get_property (GObject    *gobject,
                        guint       prop_id,
                        GValue     *value,
                        GParamSpec *pspec)
{
  TidyTextPrivate *priv;

  priv = TIDY_TEXT (gobject)->priv;

  switch (prop_id)
    {
    case PROP_CURSOR_VISIBLE:
      g_value_set_boolean (value, priv->cursor_visible);
      break;
    case PROP_CURSOR_COLOR:
      {
        ClutterColor color;
        tidy_text_get_cursor_color (TIDY_TEXT (gobject), &color);
        g_value_set_boxed (value, &color);
      }
      break;
    case PROP_POSITION:
      g_value_set_int (value, CLUTTER_FIXED_TO_FLOAT (priv->position));
      break;
    case PROP_SELECTION_BOUND:
      g_value_set_int (value, CLUTTER_FIXED_TO_FLOAT (priv->selection_bound));
      break;
    case PROP_EDITABLE:
      g_value_set_boolean (value, priv->editable);
      break;
    case PROP_SELECTABLE:
      g_value_set_boolean (value, priv->selectable);
      break;
    case PROP_ACTIVATABLE:
      g_value_set_boolean (value, priv->activatable);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, prop_id, pspec);
      break;
    }
}

static void
cursor_paint (ClutterActor *actor,
              gpointer      user_data)
{
  TidyText        *ttext = TIDY_TEXT (actor);
  TidyTextPrivate *priv   = ttext->priv;

  if (priv->editable &&
      priv->cursor_visible)
    {
      if (priv->cursor_color_set)
        {
          cogl_color (&(priv->cursor_color));
        }
      else
        {
          ClutterColor color;
          clutter_label_get_color (CLUTTER_LABEL (actor), &color);
          cogl_color (&color);
        }

      tidy_text_ensure_cursor_position (ttext);

      if (priv->position == 0)
        priv->cursor_pos.x -= 2;

      if (priv->position == priv->selection_bound)
        {
          cogl_rectangle (priv->cursor_pos.x,
                          priv->cursor_pos.y,
                          priv->cursor_pos.width,
                          priv->cursor_pos.height);
        }
      else
        {
          gint lines;
          gint start_index;
          gint end_index;
          const gchar *utf8 = clutter_label_get_text (CLUTTER_LABEL (ttext));

          start_index = offset_to_bytes (utf8, priv->position);
          end_index = offset_to_bytes (utf8, priv->selection_bound);

          if (start_index > end_index)
            {
              gint temp = start_index;
              start_index = end_index;
              end_index = temp;
            }
          
          PangoLayout *layout = clutter_label_get_layout (CLUTTER_LABEL (ttext));
          lines = pango_layout_get_line_count (layout);
          gint line_no;
          for (line_no = 0; line_no < lines; line_no++)
            {
              PangoLayoutLine *line;
              gint  n_ranges;
              gint *ranges;
              gint  i;
              gint y;
              gint height;
              gint index;
              gint maxindex;

              line = pango_layout_get_line_readonly (layout, line_no);
              pango_layout_line_x_to_index (line, G_MAXINT, &maxindex, NULL);
              if (maxindex < start_index)
                continue;

              pango_layout_line_get_x_ranges (line, start_index, end_index, &ranges, &n_ranges);
              pango_layout_line_x_to_index (line, 0, &index, NULL);

              tidy_text_position_to_coords (ttext, bytes_to_offset (utf8, index), NULL, &y, &height);

              for (i=0;i<n_ranges;i++)
                {
                  cogl_rectangle (ranges[i*2+0]/PANGO_SCALE,
                                  y-height,
                                  (ranges[i*2+1]-ranges[i*2+0])/PANGO_SCALE,
                                  height);
                }

              g_free (ranges);

            }
      }
    }
}



static gboolean
tidy_text_press (ClutterActor       *actor,
                 ClutterButtonEvent *bev)
{
  TidyText *ttext = TIDY_TEXT (actor);
  TidyTextPrivate *priv = ttext->priv;
  ClutterUnit           x, y;
  gint                  index_;
  const gchar          *text;

  text = clutter_label_get_text (CLUTTER_LABEL (actor));

  x = CLUTTER_UNITS_FROM_INT (bev->x);
  y = CLUTTER_UNITS_FROM_INT (bev->y);

  clutter_actor_transform_stage_point (actor, x, y, &x, &y);

  index_ = tidy_text_coords_to_position (ttext, CLUTTER_UNITS_TO_INT (x),
                                                CLUTTER_UNITS_TO_INT (y));

  tidy_text_set_cursor_position (ttext, bytes_to_offset (text, index_));
  tidy_text_set_selection_bound (ttext, bytes_to_offset (text, index_)
    );

  /* we'll steal keyfocus if we do not have it */
  {
    ClutterActor *stage;
    for (stage = actor;
         clutter_actor_get_parent (stage);
         stage = clutter_actor_get_parent (stage));
    if (stage && CLUTTER_IS_STAGE (stage))
      clutter_stage_set_key_focus (CLUTTER_STAGE (stage), actor);
  }

  priv->in_select_drag = TRUE;
  clutter_grab_pointer (actor);

  return TRUE;
}


static gboolean
tidy_text_motion (ClutterActor       *actor,
                  ClutterMotionEvent *mev)
{
  TidyText *ttext = TIDY_TEXT (actor);
  TidyTextPrivate *priv = ttext->priv;
  ClutterUnit           x, y;
  gint                  index_;
  const gchar          *text;

  if (!priv->in_select_drag)
    {
      return FALSE;
    }

  text = clutter_label_get_text (CLUTTER_LABEL (actor));

  x = CLUTTER_UNITS_FROM_INT (mev->x);
  y = CLUTTER_UNITS_FROM_INT (mev->y);

  clutter_actor_transform_stage_point (actor, x, y, &x, &y);

  index_ = tidy_text_coords_to_position (ttext, CLUTTER_UNITS_TO_INT (x),
                                                CLUTTER_UNITS_TO_INT (y));

  if (priv->selectable)
    {
      tidy_text_set_cursor_position (ttext, bytes_to_offset (text, index_));
    }
  else
    {
      tidy_text_set_cursor_position (ttext, bytes_to_offset (text, index_));
      tidy_text_set_selection_bound (ttext, bytes_to_offset (text, index_));
    }

  return TRUE;
}

static gboolean
tidy_text_release (ClutterActor       *actor,
                   ClutterButtonEvent *bev)
{
  TidyText *ttext = TIDY_TEXT (actor);
  TidyTextPrivate *priv = ttext->priv;
  if (priv->in_select_drag)
    {
      clutter_ungrab_pointer ();
      priv->in_select_drag = FALSE;
      return TRUE;
    }
  return FALSE;
}

static void
tidy_text_constructed (GObject *object)
{
  g_signal_connect (CLUTTER_ACTOR (object),
                    "paint", G_CALLBACK (cursor_paint),
                    NULL);

  if (G_OBJECT_CLASS (tidy_text_parent_class)->constructed != NULL)
    G_OBJECT_CLASS (tidy_text_parent_class)->constructed (object);
}

static void
tidy_text_class_init (TidyTextClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *actor_class = CLUTTER_ACTOR_CLASS (klass);

  gobject_class->set_property = tidy_text_set_property;
  gobject_class->get_property = tidy_text_get_property;
  gobject_class->constructed = tidy_text_constructed;
  gobject_class->finalize = tidy_text_finalize;

  actor_class->key_press_event = tidy_text_key_press;
  actor_class->button_press_event = tidy_text_press;
  actor_class->button_release_event = tidy_text_release;
  actor_class->motion_event = tidy_text_motion;

  /**
   * TidyText:editable:
   *
   * Whether key events delivered to the actor causes editing.
   */
  g_object_class_install_property
    (gobject_class, PROP_EDITABLE,
     g_param_spec_boolean ("editable",
			"Editable",
			"Whether the text is editable",
			TRUE,
			G_PARAM_READWRITE));


  /**
   * TidyText:selectable:
   *
   * Whether it is possible to select text.
   */
  g_object_class_install_property
    (gobject_class, PROP_SELECTABLE,
     g_param_spec_boolean ("selectable",
			"Editable",
			"Whether the text is selectable",
			TRUE,
			G_PARAM_READWRITE));

  /**
   * TidyText:activatable:
   *
   * Toggles whether return invokes the activate signal or not.
   */
  g_object_class_install_property
    (gobject_class, PROP_ACTIVATABLE,
     g_param_spec_boolean ("activatable",
			"Editable",
			"Whether return causes the activate signal to be fired",
			TRUE,
			G_PARAM_READWRITE));

  /**
   * TidyText:cursor-visible:
   *
   * Whether the input cursor is visible or not, it will only be visible
   * if both cursor-visible is set and editable is set at the same time,
   * the value defaults to TRUE.
   */
  g_object_class_install_property
    (gobject_class, PROP_CURSOR_VISIBLE,
     g_param_spec_boolean ("cursor-visible",
			"Cursor Visible",
			"Whether the input cursor is visible",
			TRUE,
			G_PARAM_READWRITE));


  g_object_class_install_property
    (gobject_class, PROP_CURSOR_COLOR,
     g_param_spec_boxed ("cursor-color",
			 "Cursor Colour",
			 "Cursor  Colour",
			 CLUTTER_TYPE_COLOR,
			 G_PARAM_READWRITE));

  /**
   * TidyText:position:
   *
   * The current input cursor position. -1 is taken to be the end of the text
   */
  g_object_class_install_property
    (gobject_class, PROP_POSITION,
     g_param_spec_int ("position",
                       "Position",
                       "The cursor position",
                       -1, G_MAXINT,
                       -1,
                       G_PARAM_READWRITE));


  /**
   * TidyText:selection-bound:
   *
   * The current input cursor position. -1 is taken to be the end of the text
   */
  g_object_class_install_property
    (gobject_class, PROP_SELECTION_BOUND,
     g_param_spec_int ("selection-bound",
                       "Selection-bound",
                       "The cursor position of the other end of the selection.",
                       -1, G_MAXINT,
                       -1,
                       G_PARAM_READWRITE));

 /**
   * TidyText::text-changed:
   * @actor: the actor which received the event
   *
   * The ::text-changed signal is emitted after @entry's text changes
   */
  label_signals[TEXT_CHANGED] =
    g_signal_new ("text-changed",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterEntryClass, text_changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);


  label_signals[CURSOR_EVENT] =
    g_signal_new ("cursor-event",
		  G_TYPE_FROM_CLASS (gobject_class),
		  G_SIGNAL_RUN_LAST,
		  G_STRUCT_OFFSET (ClutterEntryClass, cursor_event),
		  NULL, NULL,
		  g_cclosure_marshal_VOID__BOXED,
		  G_TYPE_NONE, 1,
		  CLUTTER_TYPE_GEOMETRY | G_SIGNAL_TYPE_STATIC_SCOPE);


 /**
   * TidyText::activate
   * @actor: the actor which received the event
   *
   * The ::activate signal is emitted each time the entry is 'activated'
   * by the user, normally by pressing the 'Enter' key.
   *
   * Since: 0.4
   */
  label_signals[ACTIVATE] =
    g_signal_new ("activate",
                  G_TYPE_FROM_CLASS (gobject_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (ClutterEntryClass, activate),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);


  g_type_class_add_private (klass, sizeof (TidyTextPrivate));
}

static void
tidy_text_init (TidyText *self)
{
  TidyTextPrivate *priv;

  self->priv = priv = TIDY_TEXT_GET_PRIVATE (self);
  priv->x_pos = -1;
  priv->cursor_visible = TRUE;
  priv->editable = FALSE;

  priv->cursor_color_set = FALSE;
  init_commands (self); /* FIXME: free */
  init_mappings (self); /* FIXME: free */
}

ClutterActor *
tidy_text_new_full (const gchar        *font_name,
                     const gchar        *text,
                     const ClutterColor *color)
{
  return g_object_new (TIDY_TYPE_TEXT,
                       "font-name", font_name,
                       "text", text,
                       "color", color,
                       NULL);
}

ClutterActor *
tidy_text_new_with_text (const gchar *font_name,
                         const gchar *text)
{
  return g_object_new (TIDY_TYPE_TEXT,
                       "font-name", font_name,
                       "text", text,
                       NULL);
}


void
tidy_text_set_editable (TidyText *label,
                        gboolean  editable)
{
  TidyTextPrivate *priv;

  priv = label->priv;
  priv->editable = editable;
  clutter_actor_queue_redraw (CLUTTER_ACTOR (label));
}

gboolean
tidy_text_get_editable (TidyText *label)
{
  TidyTextPrivate *priv;
  priv = label->priv;
  return priv->editable;
}


void
tidy_text_set_selectable (TidyText *label,
                          gboolean  selectable)
{
  TidyTextPrivate *priv;

  priv = label->priv;
  priv->selectable = selectable;
  clutter_actor_queue_redraw (CLUTTER_ACTOR (label));
}

gboolean
tidy_text_get_selectable (TidyText *label)
{
  TidyTextPrivate *priv;
  priv = label->priv;
  return priv->selectable;
}


void
tidy_text_set_activatable (TidyText *label,
                           gboolean  activatable)
{
  TidyTextPrivate *priv;

  priv = label->priv;
  priv->activatable = activatable;
  clutter_actor_queue_redraw (CLUTTER_ACTOR (label));
}

gboolean
tidy_text_get_activatable (TidyText *label)
{
  TidyTextPrivate *priv;
  priv = label->priv;
  return priv->activatable;
}


void
tidy_text_set_cursor_visible (TidyText *label,
                              gboolean  cursor_visible)
{
  TidyTextPrivate *priv;

  priv = label->priv;
  priv->cursor_visible = cursor_visible;
  clutter_actor_queue_redraw (CLUTTER_ACTOR (label));
}

gboolean
tidy_text_get_cursor_visible (TidyText *label)
{
  TidyTextPrivate *priv;
  priv = label->priv;
  return priv->cursor_visible;
}

void
tidy_text_set_cursor_color (TidyText           *text,
		            const ClutterColor *color)
{
  TidyTextPrivate *priv;

  g_return_if_fail (CLUTTER_IS_LABEL (text));
  g_return_if_fail (color != NULL);

  priv = text->priv;

  g_object_ref (text);

  if (color)
    {
      priv->cursor_color = *color;
      priv->cursor_color_set = TRUE;
    }
  else
    {
      priv->cursor_color_set = FALSE;
    }
}


void
tidy_text_get_cursor_color (TidyText     *text,
                            ClutterColor *color)
{
  TidyTextPrivate *priv;

  g_return_if_fail (CLUTTER_IS_LABEL (text));
  g_return_if_fail (color != NULL);

  priv = text->priv;

  *color = priv->cursor_color;
}


gint
tidy_text_get_selection_bound (TidyText *text)
{
  TidyTextPrivate *priv;
 
  priv = text->priv;

  return priv->selection_bound;
}

gchar *
tidy_text_get_selection (TidyText *text)
{
  TidyTextPrivate *priv;
 
  const gchar *utf8 = clutter_label_get_text (CLUTTER_LABEL (text));
  gchar       *str;
  gint         len;
  gint         start_index;
  gint         end_index;
  gint         start_offset;
  gint         end_offset;

  priv = text->priv;

  start_index = priv->position;
  end_index = priv->selection_bound;

  if (end_index == start_index)
    return g_strdup ("");
  if (end_index < start_index)
    {
      gint temp = start_index;
      start_index = end_index;
      end_index = temp;
    }

  start_offset = offset_to_bytes (utf8, start_index);
  end_offset = offset_to_bytes (utf8, end_index);
  len = end_offset - start_offset;

  str = g_malloc (len + 1);
  g_utf8_strncpy (str, utf8 + start_offset, end_index-start_index);
  return str;
}



void
tidy_text_set_selection_bound (TidyText *text,
                               gint      selection_bound)
{
  TidyTextPrivate *priv;

  priv = text->priv;
  priv->selection_bound = selection_bound;

  clutter_actor_queue_redraw (CLUTTER_ACTOR (text));
}


/****************************************************************/
/* The following are the commands available for keybinding when */
/* using the entry, these can also be invoked programmatically  */
/* through tidy_text_action()                                   */
/****************************************************************/

static gboolean
tidy_text_action_activate (TidyText            *ttext,
                           const gchar         *commandline,
                           ClutterEvent *event)
{
  g_signal_emit (G_OBJECT (ttext), label_signals[ACTIVATE], 0);
  return TRUE;
}

static void
tidy_text_clear_selection (TidyText *ttext)
{
  TidyTextPrivate *priv = ttext->priv;
  priv->selection_bound = priv->position;
}

static gboolean
tidy_text_action_move_left (TidyText     *ttext,
                            const gchar  *commandline,
                            ClutterEvent *event)
{
  TidyTextPrivate *priv = ttext->priv;
  gint pos = priv->position;
  gint len;
  len = g_utf8_strlen (clutter_label_get_text (CLUTTER_LABEL (ttext)), -1);

  if (pos != 0 && len !=0)
    {
      if (pos == -1)
        {
          tidy_text_set_cursor_position (ttext, len - 1);
        }
      else
        {
          tidy_text_set_cursor_position (ttext, pos - 1);
        }
    }

  if (!(priv->selectable && event &&
      (event->key.modifier_state & CLUTTER_SHIFT_MASK)))
    tidy_text_clear_selection (ttext);

  return TRUE;
}


static gboolean
tidy_text_action_move_right (TidyText            *ttext,
                             const gchar         *commandline,
                             ClutterEvent *event)
{
  TidyTextPrivate *priv = ttext->priv;
  gint pos;
 
  gint len;
  len = g_utf8_strlen (clutter_label_get_text (CLUTTER_LABEL (ttext)), -1);

  pos = priv->position;
  if (pos != -1 && len !=0)
    {
      if (pos != len)
        {
          tidy_text_set_cursor_position (ttext, pos + 1);
        }
    }

  if (!(priv->selectable &&
        event &&
       (event->key.modifier_state & CLUTTER_SHIFT_MASK)))
    {
      tidy_text_clear_selection (ttext);
    }

  return TRUE;
}

static gboolean
tidy_text_action_move_up (TidyText            *ttext,
                          const gchar         *commandline,
                          ClutterEvent *event)
{
  TidyTextPrivate *priv = ttext->priv;
  gint                          line_no;
  gint                          index_;
  gint                          x;
  const gchar                  *text;
  PangoLayoutLine              *layout_line;

  text = clutter_label_get_text (CLUTTER_LABEL (ttext));

  pango_layout_index_to_line_x (
        clutter_label_get_layout (CLUTTER_LABEL (ttext)),
        offset_to_bytes (text, priv->position),
        0,
        &line_no,
        &x);
  
  if (priv->x_pos != -1)
    x = priv->x_pos;
  else
    priv->x_pos = x;

  line_no -= 1;
  if (line_no < 0)
    return FALSE;

  layout_line = pango_layout_get_line_readonly (
                    clutter_label_get_layout (CLUTTER_LABEL (ttext)),
                    line_no);

  if (!layout_line)
    return TRUE;

  pango_layout_line_x_to_index (layout_line, x, &index_, NULL);

  {
    gint pos = bytes_to_offset (text, index_);
    tidy_text_set_cursor_position (ttext, pos);
  }

  if (!(priv->selectable && event &&
      (event->key.modifier_state & CLUTTER_SHIFT_MASK)))
    tidy_text_clear_selection (ttext);

  return TRUE;
}

static gboolean
tidy_text_action_move_down (TidyText            *ttext,
                            const gchar         *commandline,
                            ClutterEvent *event)
{
  TidyTextPrivate *priv = ttext->priv;
  gint                          line_no;
  gint                          index_;
  gint                          x;
  const gchar                  *text;
  PangoLayoutLine              *layout_line;

  text = clutter_label_get_text (CLUTTER_LABEL (ttext));

  pango_layout_index_to_line_x (
        clutter_label_get_layout (CLUTTER_LABEL (ttext)),
        offset_to_bytes (text, priv->position),
        0,
        &line_no,
        &x);
  
  if (priv->x_pos != -1)
    x = priv->x_pos;
  else
    priv->x_pos = x;

  layout_line = pango_layout_get_line_readonly (
                    clutter_label_get_layout (CLUTTER_LABEL (ttext)),
                    line_no + 1);

  if (!layout_line)
    {
      return FALSE;
    }

  pango_layout_line_x_to_index (layout_line, x, &index_, NULL);

    {
      gint pos = bytes_to_offset (text, index_);
      tidy_text_set_cursor_position (ttext, pos);
    }
  if (!(priv->selectable && event &&
      (event->key.modifier_state & CLUTTER_SHIFT_MASK)))
    tidy_text_clear_selection (ttext);
  return TRUE;
}

static gboolean
tidy_text_action_move_start (TidyText            *ttext,
                             const gchar         *commandline,
                             ClutterEvent *event)
{
  TidyTextPrivate *priv = ttext->priv;

  tidy_text_set_cursor_position (ttext, 0);
  if (!(priv->selectable && event &&
      (event->key.modifier_state & CLUTTER_SHIFT_MASK)))
    tidy_text_clear_selection (ttext);
  return TRUE;
}

static gboolean
tidy_text_action_move_end (TidyText            *ttext,
                           const gchar         *commandline,
                           ClutterEvent *event)
{
  TidyTextPrivate *priv = ttext->priv;

  tidy_text_set_cursor_position (ttext, -1);
  if (!(priv->selectable && event &&
      (event->key.modifier_state & CLUTTER_SHIFT_MASK)))
    tidy_text_clear_selection (ttext);

  return TRUE;
}

static gboolean
tidy_text_action_move_start_line (TidyText            *ttext,
                                  const gchar         *commandline,
                                  ClutterEvent *event)
{
  TidyTextPrivate *priv = ttext->priv;
  gint                          line_no;
  gint                          index_;
  const gchar                  *text;
  PangoLayoutLine              *layout_line;
  gint                          position;

  text = clutter_label_get_text (CLUTTER_LABEL (ttext));


  pango_layout_index_to_line_x (
        clutter_label_get_layout (CLUTTER_LABEL (ttext)),
        offset_to_bytes (text, priv->position),
        0,
        &line_no,
        NULL);

  layout_line = pango_layout_get_line_readonly (
                    clutter_label_get_layout (CLUTTER_LABEL (ttext)),
                    line_no);

  pango_layout_line_x_to_index (layout_line, 0, &index_, NULL);

  position = bytes_to_offset (text, index_);
  tidy_text_set_cursor_position (ttext, position);

  if (!(priv->selectable && event &&
      (event->key.modifier_state & CLUTTER_SHIFT_MASK)))
    tidy_text_clear_selection (ttext);

  return TRUE;
}

static gboolean
tidy_text_action_move_end_line (TidyText            *ttext,
                                const gchar         *commandline,
                                ClutterEvent *event)
{
  TidyTextPrivate *priv = ttext->priv;
  gint                          line_no;
  gint                          index_;
  gint                          trailing;
  const gchar                  *text;
  PangoLayoutLine              *layout_line;
  gint                          position;

  text = clutter_label_get_text (CLUTTER_LABEL (ttext));

  index_ = offset_to_bytes (text, priv->position);

  pango_layout_index_to_line_x (
        clutter_label_get_layout (CLUTTER_LABEL (ttext)),
        index_,
        0,
        &line_no,
        NULL);

  layout_line = pango_layout_get_line_readonly (
                    clutter_label_get_layout (CLUTTER_LABEL (ttext)),
                    line_no);

  pango_layout_line_x_to_index (layout_line, G_MAXINT, &index_, &trailing);
  index_ += trailing;

  position = bytes_to_offset (text, index_);

  tidy_text_set_cursor_position (ttext, position);

  if (!(priv->selectable && event &&
      (event->key.modifier_state & CLUTTER_SHIFT_MASK)))
    tidy_text_clear_selection (ttext);

  return TRUE;
}

static gboolean
tidy_text_action_delete_next (TidyText *ttext,
                              const gchar         *commandline,
                              ClutterEvent *event)
{
  TidyTextPrivate *priv;
  gint pos;
  gint len;
 
  if (tidy_text_truncate_selection (ttext, NULL, 0))
    return TRUE;
  priv = ttext->priv;
  pos = priv->position;
  len = g_utf8_strlen (clutter_label_get_text (CLUTTER_LABEL (ttext)), -1);

  if (len && pos != -1 && pos < len)
    tidy_text_delete_text (ttext, pos, pos+1);;
  return TRUE;
}

static gboolean
tidy_text_action_delete_previous (TidyText            *ttext,
                                  const gchar         *commandline,
                                  ClutterEvent *event)
{
  TidyTextPrivate *priv;
  gint pos;
  gint len;
 
  if (tidy_text_truncate_selection (ttext, NULL, 0))
    return TRUE;
  priv = ttext->priv;
  pos = priv->position;
  len = g_utf8_strlen (clutter_label_get_text (CLUTTER_LABEL (ttext)), -1);

  if (pos != 0 && len !=0)
    {
      if (pos == -1)
        {
          tidy_text_set_cursor_position (ttext, len - 1);
          tidy_text_set_selection_bound (ttext, len - 1);
        }
      else
        {
          tidy_text_set_cursor_position (ttext, pos - 1);
          tidy_text_set_selection_bound (ttext, pos - 1);
        }
      tidy_text_delete_text (ttext, pos-1, pos);;
    }
  return TRUE;
}


static void init_commands (TidyText *ttext)
{
  TidyTextPrivate *priv = ttext->priv;
  if (priv->commands)
    return;
  tidy_text_add_action (ttext, "move-left",       tidy_text_action_move_left);
  tidy_text_add_action (ttext, "move-right",      tidy_text_action_move_right);
  tidy_text_add_action (ttext, "move-up",         tidy_text_action_move_up);
  tidy_text_add_action (ttext, "move-down",       tidy_text_action_move_down);
  tidy_text_add_action (ttext, "move-start",      tidy_text_action_move_start);
  tidy_text_add_action (ttext, "move-end",        tidy_text_action_move_end);
  tidy_text_add_action (ttext, "move-start-line", tidy_text_action_move_start_line);
  tidy_text_add_action (ttext, "move-end-line",   tidy_text_action_move_end_line);
  tidy_text_add_action (ttext, "delete-previous", tidy_text_action_delete_previous);
  tidy_text_add_action (ttext, "delete-next",     tidy_text_action_delete_next);
  tidy_text_add_action (ttext, "activate",        tidy_text_action_activate);
  tidy_text_add_action (ttext, "truncate-selection", tidy_text_truncate_selection);
}

gboolean
tidy_text_action (TidyText     *ttext,
                  const gchar  *command,
                  ClutterEvent *event)
{
  gchar command2[64];
  gint i;
  
  GList *iter;
  TidyTextPrivate *priv = ttext->priv;

  for (i=0; command[i] &&
            command[i]!=' '&&
            i<62; i++)
    {
      command2[i]=command[i];
    }
  command2[i]='\0';

  if (!g_str_equal (command2, "move-up") &&
      !g_str_equal (command2, "move-down"))
    priv->x_pos = -1;

  for (iter=priv->commands;iter;iter=iter->next)
    {
      TextCommand *tcommand = iter->data;
      if (g_str_equal (command2, tcommand->name))
        return tcommand->func (ttext, command, event);
    }

  g_warning ("unhandled text command %s", command);
  return FALSE;
}

static gboolean
tidy_text_key_press (ClutterActor    *actor,
                     ClutterKeyEvent *kev)
{
  TidyTextPrivate *priv   = TIDY_TEXT (actor)->priv;
  gint             keyval = clutter_key_event_symbol (kev);
  GList           *iter;

  if (!priv->editable)
    return FALSE;

  for (iter=priv->mappings;iter;iter=iter->next)
    {
      TidyTextMapping *mapping = iter->data;
      
      if (
          (mapping->keyval == keyval) &&
            (
             (mapping->state == 0) || 
             (mapping->state && (kev->modifier_state & mapping->state)) 
            ) 
         )
        {
          if (!g_str_equal (mapping->action, "activate") ||
              priv->activatable)
            return tidy_text_action (TIDY_TEXT (actor), mapping->action, (ClutterEvent*)kev);
        }
    }

    {
      gunichar key_unichar = clutter_key_event_unicode (kev);

      if (key_unichar == '\r')  /* return is reported as CR we want LF */
        key_unichar = '\n';
      if (g_unichar_validate (key_unichar))
        {
          tidy_text_insert_unichar (TIDY_TEXT (actor), key_unichar);
          return TRUE;
        }
    }
  return FALSE;
}

