#include <clutter/clutter.h>

G_BEGIN_DECLS

#define GMC_TYPE_BUTTON            (gmc_button_get_type ())
#define GMC_BUTTON(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_BUTTON, GmcButton))
#define GMC_BUTTON_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_BUTTON, GmcButtonClass))
#define GMC_IS_BUTTON(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_BUTTON))
#define GMC_IS_BUTTON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_BUTTON))
#define GMC_BUTTON_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_BUTTON, GmcButtonClass))

typedef struct _GmcButton        GmcButton;
typedef struct _GmcButtonPrivate GmcButtonPrivate;
typedef struct _GmcButtonClass   GmcButtonClass;

struct _GmcButton
{
  ClutterActor              parent;
  /*< priv >*/
  GmcButtonPrivate *priv;
};

struct _GmcButtonClass 
{
  ClutterActorClass parent_class;
  
  void (* clicked) (GmcButton *button);
}; 

GType           gmc_button_get_type           (void) G_GNUC_CONST;

ClutterActor *  gmc_button_new                (const gchar *text);
ClutterActor *  gmc_button_new_with_icon      (const gchar *text, ClutterActor *icon);
void            gmc_button_set_text           (GmcButton *button, const gchar *text);
const gchar *   gmc_button_get_text           (GmcButton *button);
void            gmc_button_set_icon           (GmcButton *button, ClutterActor *icon);

G_END_DECLS
