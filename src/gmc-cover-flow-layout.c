#include "gmc-cover-flow-layout.h"

G_DEFINE_TYPE (GmcCoverFlowLayout, gmc_cover_flow_layout, CLUTTER_TYPE_LAYOUT_MANAGER);

#define GMC_COVER_FLOW_LAYOUT_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_COVER_FLOW_LAYOUT, GmcCoverFlowLayoutPrivate))

struct _GmcCoverFlowLayoutPrivate
{
  gint lol;
};

static void
gmc_cover_flow_layout_get_preferred_width (ClutterLayoutManager   *manager,
                                           ClutterContainer       *container,
                                           gfloat                  for_height,
                                           gfloat                 *minimum_width_p,
                                           gfloat                 *natural_width_p)
{
  g_debug ("%s", G_STRFUNC);
  *minimum_width_p = 100;
  *natural_width_p = 100;
}

static void
gmc_cover_flow_layout_get_preferred_height (ClutterLayoutManager   *manager,
                                            ClutterContainer       *container,
                                            gfloat                  for_width,
                                            gfloat                 *minimum_height_p,
                                            gfloat                 *natural_height_p)
{
  g_debug ("%s", G_STRFUNC);
  *minimum_height_p = 100;
  *natural_height_p = 100;
}

static void
gmc_cover_flow_layout_allocate (ClutterLayoutManager   *manager,
                                ClutterContainer       *container,
                                const ClutterActorBox  *allocation,
                                ClutterAllocationFlags  flags)
{
  g_debug ("%s", G_STRFUNC);
  GList *children, *l;
  gfloat n = 0;

  children = clutter_container_get_children (container);

  for (l = children; l != NULL; l = l->next) {
    g_debug ("child");
    gfloat min_h, nat_h, min_w, nat_w;
    ClutterActor *child = l->data;

    clutter_actor_get_preferred_width (child, allocation->y2 - allocation->y1, &min_w, &nat_w);
    g_debug ("w: %f - h: %f", min_w, nat_w);

    ClutterActorBox *child_box = clutter_actor_box_new (n, 0, n + 120, 100);
    n = n + 100;
    g_debug ("%f", n);

    clutter_actor_allocate (child, child_box, flags);
  }

  g_list_free (children);
}

static void
gmc_cover_flow_layout_class_init (GmcCoverFlowLayoutClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterLayoutManagerClass *layout_class = CLUTTER_LAYOUT_MANAGER_CLASS (klass);

  g_type_class_add_private (object_class, sizeof (GmcCoverFlowLayoutPrivate));

  layout_class->get_preferred_width     = gmc_cover_flow_layout_get_preferred_width;
  layout_class->get_preferred_height    = gmc_cover_flow_layout_get_preferred_height;
  layout_class->allocate                = gmc_cover_flow_layout_allocate;
  //layout_class->set_container           = gmc_cover_flow_layout_set_container;
  //layout_class->get_child_meta_type     = gmc_cover_flow_layout_get_child_meta_type;
  //layout_class->begin_animation         = gmc_cover_flow_layout_begin_animation;
  //layout_class->get_animation_progress  = gmc_cover_flow_layout_get_animation_progress;
  //layout_class->end_animation           = gmc_cover_flow_layout_end_animation;
  
  /*
  void               (* set_container)          (ClutterLayoutManager   *manager,
                                                 ClutterContainer       *container);

  GType              (* get_child_meta_type)    (ClutterLayoutManager   *manager);
  ClutterLayoutMeta *(* create_child_meta)      (ClutterLayoutManager   *manager,
                                                 ClutterContainer       *container,
                                                 ClutterActor           *actor);

  ClutterAlpha *     (* begin_animation)        (ClutterLayoutManager   *manager,
                                                 guint                   duration,
                                                 gulong                  mode);
  gdouble            (* get_animation_progress) (ClutterLayoutManager   *manager);
  void               (* end_animation)          (ClutterLayoutManager   *manager);
  */
}

static void
added_cb   (ClutterContainer *container,
            ClutterActor     *actor)
{
  g_debug ("%s", G_STRFUNC);
  clutter_actor_set_rotation (actor, CLUTTER_Y_AXIS, 30, 0, 0, 0);
}

static void
gmc_cover_flow_layout_init (GmcCoverFlowLayout *self)
{
  g_signal_connect (self, "actor-added", added_cb, NULL);
}

ClutterLayoutManager *
gmc_cover_flow_layout_new (void)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_COVER_FLOW_LAYOUT, NULL);

  return CLUTTER_LAYOUT_MANAGER (self);
}