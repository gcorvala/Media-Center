#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GMC_TYPE_CAROUSEL            (gmc_carousel_get_type ())
#define GMC_CAROUSEL(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_CAROUSEL, GmcCarousel))
#define GMC_CAROUSEL_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_CAROUSEL, GmcCarouselClass))
#define GMC_IS_CAROUSEL(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_CAROUSEL))
#define GMC_IS_CAROUSEL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_CAROUSEL))
#define GMC_CAROUSEL_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_CAROUSEL, GmcCarouselClass))

typedef struct _GmcCarousel        GmcCarousel;
typedef struct _GmcCarouselPrivate GmcCarouselPrivate;
typedef struct _GmcCarouselClass   GmcCarouselClass;

struct _GmcCarousel
{
  ClutterGroup              parent;
  /*< priv >*/
  GmcCarouselPrivate *priv;
};

struct _GmcCarouselClass 
{
  ClutterGroupClass parent_class;
}; 

GType           gmc_carousel_get_type           (void) G_GNUC_CONST;

ClutterActor *  gmc_carousel_new                ();

G_END_DECLS
