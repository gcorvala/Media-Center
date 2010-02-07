#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GMC_TYPE_COVER_FLOW_LAYOUT            (gmc_cover_flow_layout_get_type ())
#define GMC_COVER_FLOW_LAYOUT(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_COVER_FLOW_LAYOUT, GmcCoverFlowLayout))
#define GMC_COVER_FLOW_LAYOUT_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_COVER_FLOW_LAYOUT, GmcCoverFlowLayoutClass))
#define GMC_IS_COVER_FLOW_LAYOUT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_COVER_FLOW_LAYOUT))
#define GMC_IS_COVER_FLOW_LAYOUT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_COVER_FLOW_LAYOUT))
#define GMC_COVER_FLOW_LAYOUT_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_COVER_FLOW_LAYOUT, GmcCoverFlowLayoutClass))

typedef struct _GmcCoverFlowLayout        GmcCoverFlowLayout;
typedef struct _GmcCoverFlowLayoutPrivate GmcCoverFlowLayoutPrivate;
typedef struct _GmcCoverFlowLayoutClass   GmcCoverFlowLayoutClass;

struct _GmcCoverFlowLayout
{
  ClutterLayoutManager  parent;
  /*< priv >*/
  GmcCoverFlowLayoutPrivate *priv;
};

struct _GmcCoverFlowLayoutClass 
{
  ClutterLayoutManagerClass parent_class;
}; 

GType                   gmc_cover_flow_layout_get_type           (void) G_GNUC_CONST;

ClutterLayoutManager *  gmc_cover_flow_layout_new                (void);

G_END_DECLS
