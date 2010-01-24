#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GMC_TYPE_LIST_VIEW            (gmc_list_view_get_type ())
#define GMC_LIST_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_LIST_VIEW, GmcListView))
#define GMC_LIST_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_LIST_VIEW, GmcListViewClass))
#define GMC_IS_LIST_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_LIST_VIEW))
#define GMC_IS_LIST_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_LIST_VIEW))
#define GMC_LIST_VIEW_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_LIST_VIEW, GmcListViewClass))

typedef struct _GmcListView        GmcListView;
typedef struct _GmcListViewPrivate GmcListViewPrivate;
typedef struct _GmcListViewClass   GmcListViewClass;

struct _GmcListView
{
  ClutterActor        parent;
  /*< priv >*/
  GmcListViewPrivate *priv;
};

struct _GmcListViewClass 
{
  ClutterActorClass parent_class;
}; 

GType           gmc_list_view_get_type           (void) G_GNUC_CONST;

ClutterActor *  gmc_list_view_new                (ClutterModel *model, guint column);

G_END_DECLS
