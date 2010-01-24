#include "gmc-list-view.h"

G_DEFINE_TYPE (GmcListView, gmc_list_view, CLUTTER_TYPE_ACTOR);

#define GMC_LIST_VIEW_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_LIST_VIEW, GmcListViewPrivate))

struct _GmcListViewPrivate
{
  GList *elements;
};

static void
gmc_list_view_class_init (GmcListViewClass *klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

  g_type_class_add_private (gobject_class, sizeof (GmcListViewPrivate));
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

  self = g_object_new (GMC_TYPE_LIST_VIEW,
                       NULL);

  return CLUTTER_ACTOR (self);
}
