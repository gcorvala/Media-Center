#include "gmc-list-view.h"

G_DEFINE_TYPE (GmcListView, gmc_list_view, CLUTTER_TYPE_ACTOR);

#define GMC_LIST_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_LIST_VIEW, GmcListViewPrivate))

struct _GmcListViewPrivate
{
  ClutterModel *model;
  guint column;

  GList *children;
};

enum {
  PROP_0,
  PROP_MODEL,
  PROP_COLUMN
};

static void
gmc_list_view_set_property (GObject      *object,
                            guint         prop_id,
                            const GValue *value,
                            GParamSpec   *pspec)
{
  GmcListView *view;
  GmcListViewPrivate *priv;

  view = GMC_LIST_VIEW (object);
  priv = GMC_LIST_VIEW_GET_PRIVATE (view);

  switch (prop_id)
  {
    case PROP_MODEL:
      priv->model = g_value_get_object (value);
      break;

    case PROP_COLUMN:
      priv->column = g_value_get_uint (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gmc_list_view_get_property (GObject    *object,
                            guint       prop_id,
                            GValue     *value,
                            GParamSpec *pspec)
{
  GmcListView *view;
  GmcListViewPrivate *priv;

  view = GMC_LIST_VIEW (object);
  priv = GMC_LIST_VIEW_GET_PRIVATE (view);

  switch (prop_id)
  {
    case PROP_MODEL:
      g_value_set_object (value, priv->model);
      break;

    case PROP_COLUMN:
      g_value_set_uint (value, priv->column);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gmc_list_view_get_preferred_width (ClutterActor *actor,
                                   gfloat        for_height,
                                   gfloat       *min_width_p,
                                   gfloat       *natural_width_p)
{
  
}

static void
gmc_list_view_paint (ClutterActor *actor)
{
  GmcListViewPrivate *priv;
  GmcListView *foo_actor = GMC_LIST_VIEW (actor);
  ClutterActorBox allocation = { 0, };
  gfloat width, height;
  GList *child_item;

  priv = GMC_LIST_VIEW_GET_PRIVATE (actor);

  cogl_set_source_color4ub (0xff, 0x00, 0x00,
                            clutter_actor_get_paint_opacity (actor));

  clutter_actor_get_allocation_box (actor, &allocation);
  clutter_actor_box_get_size (&allocation, &width, &height);

  cogl_path_round_rectangle (0, 0, width, height, 10, 5);

  //cogl_path_fill ();
  for (child_item = priv->children; child_item != NULL; child_item = child_item->next) {
    ClutterActor *child = child_item->data;
    clutter_actor_show (child);
    clutter_actor_paint (child);
  }
}

static void
gmc_list_view_class_init (GmcListViewClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  ClutterActorClass *clutter_actor_class = CLUTTER_ACTOR_CLASS (klass);

  g_type_class_add_private (gobject_class, sizeof (GmcListViewPrivate));

  gobject_class->set_property = gmc_list_view_set_property;
  gobject_class->get_property = gmc_list_view_get_property;

  clutter_actor_class->get_preferred_width = gmc_list_view_get_preferred_width;
  clutter_actor_class->get_preferred_width = gmc_list_view_get_preferred_width;
  clutter_actor_class->paint = gmc_list_view_paint;

  g_object_class_install_property (gobject_class,
                                   PROP_MODEL,
                                   g_param_spec_object ("model",
                                                        "Model",
                                                        "The ClutterModel viewed",
                                                        CLUTTER_TYPE_MODEL,
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));
  g_object_class_install_property (gobject_class,
                                   PROP_COLUMN,
                                   g_param_spec_uint ("column",
                                                      "Column",
                                                      "The model column to display",
                                                      0,
                                                      G_MAXUINT,
                                                      0,
                                                      G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));
}

static void
gmc_list_view_init (GmcListView *self)
{
  GmcListViewPrivate *priv;

  priv = GMC_LIST_VIEW_GET_PRIVATE (self);
}

ClutterActor *
gmc_list_view_new (ClutterModel *model, guint column)
{
  GObject *self;
  GmcListViewPrivate *priv;
  ClutterModelIter *row;
  guint n_rows, i;
  GValue *value;
  gchar *string;

  self = g_object_new (GMC_TYPE_LIST_VIEW,
                       "model", model,
                       "column", column,
                       NULL);

  priv = GMC_LIST_VIEW_GET_PRIVATE (self);

  n_rows = clutter_model_get_n_rows (priv->model);
  g_debug ("n_rows : %d", n_rows);
  for (i = 0; i < n_rows; ++i) {
    ClutterActor *child;
    row = clutter_model_get_iter_at_row (priv->model, i);
    clutter_model_iter_get (row, column, &string, -1);
    child = clutter_text_new_with_text ("caca", string);
    priv->children = g_list_append (priv->children, child);
    clutter_actor_set_parent (child, CLUTTER_ACTOR (self));
    //string = g_value_get_string (value);
    g_debug ("%s", string);
  }

  return CLUTTER_ACTOR (self);
}
