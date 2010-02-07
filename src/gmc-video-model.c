#include "gmc-video-model.h"
#include <sqlite3.h>
#include <string.h>

static const gchar *TABLE = "Movie";

#define GMC_TYPE_VIDEO_MODEL_ITER            (gmc_video_model_iter_get_type ())
#define GMC_VIDEO_MODEL_ITER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GMC_TYPE_VIDEO_MODEL_ITER, GmcVideoModelIter))
#define GMC_VIDEO_MODEL_ITER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GMC_TYPE_VIDEO_MODEL_ITER, GmcVideoModelIterClass))
#define GMC_IS_VIDEO_MODEL_ITER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GMC_TYPE_VIDEO_MODEL_ITER))
#define GMC_IS_VIDEO_MODEL_ITER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GMC_TYPE_VIDEO_MODEL_ITER))
#define GMC_VIDEO_MODEL_ITER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj), GMC_TYPE_VIDEO_MODEL_ITER, GmcVideoModelIterClass))

typedef struct _GmcVideoModelIter        GmcVideoModelIter;
typedef struct _GmcVideoModelIterClass   GmcVideoModelIterClass;

#define GMC_VIDEO_MODEL_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), GMC_TYPE_VIDEO_MODEL, GmcVideoModelPrivate))

struct _GmcVideoModelPrivate
{
  gchar *filename;
  sqlite3 *sql;
};

struct _GmcVideoModelIter
{
  ClutterModelIter parent;
};

struct _GmcVideoModelIterClass 
{
  ClutterModelIterClass parent_class;
};

const gchar *names [] = {
//  "Actors",
//  "Duration",
//  "Genre",
//  "Resume",
  "Title",
  "Year"
};

GType types [] = {
//  G_TYPE_POINTER,
//  G_TYPE_UINT,
//  G_TYPE_STRING,
//  G_TYPE_STRING,
  G_TYPE_STRING,
  G_TYPE_UINT
};

/*
 * GmcVideoModelIter
 */

G_DEFINE_TYPE (GmcVideoModelIter, gmc_video_model_iter, CLUTTER_TYPE_MODEL_ITER);

static void
gmc_video_model_iter_get_value (ClutterModelIter *iter, 
                                guint             column, 
                                GValue           *value)
{
  ClutterModel *model;
  GmcVideoModelPrivate *priv_model;
  gchar *query, *string_value, *tmp;
  guint row, uint_value, sort_column;
  sqlite3_stmt *statement;
  gint error_code;
  GType type;

  model = clutter_model_iter_get_model (iter);
  row = clutter_model_iter_get_row (iter);
  priv_model = GMC_VIDEO_MODEL_GET_PRIVATE (model);

  query = g_strdup_printf ("SELECT %s FROM %s", clutter_model_get_column_name (model, column), TABLE);

  sort_column = clutter_model_get_sorting_column (model);
  if (sort_column != -1) {
    tmp = g_strdup_printf ("%s ORDER BY %s", query, clutter_model_get_column_name (model, sort_column));
    g_free (query);
    query = tmp;
  }

  tmp = g_strdup_printf ("%s LIMIT %d, 1", query, row);
  g_free (query);
  query = tmp;

  error_code = sqlite3_prepare_v2 (priv_model->sql, query, strlen (query), &statement, NULL);
  g_free (query);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [prepare] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv_model->sql));
    return;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_ROW) {
    g_critical ("%s [step] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv_model->sql));
    return;
  }

  type = clutter_model_get_column_type (model, column);

  switch (type) {
    case G_TYPE_STRING:
      string_value = (gchar *) sqlite3_column_text (statement, 0);
      g_value_set_string (value, string_value);
      break;

    case G_TYPE_UINT:
      uint_value = sqlite3_column_int (statement, 0);
      g_value_set_uint (value, uint_value);
      break;
  }

  sqlite3_finalize (statement);
}

static void
gmc_video_model_iter_set_value (ClutterModelIter *iter, 
                                guint             column, 
                                const GValue     *value)
{
  ClutterModel *model;
  GmcVideoModelPrivate *priv_model;
  const gchar *name;
  gchar *query, *str_value;
  sqlite3_stmt *statement;
  gint error_code;
  GType type;
  GValue title_value = {0, };

  model = clutter_model_iter_get_model (iter);
  priv_model = GMC_VIDEO_MODEL_GET_PRIVATE (model);

  name = clutter_model_get_column_name (model, column);
  type = clutter_model_get_column_type (model, column);

  switch (type) {
    case G_TYPE_STRING:
      str_value = g_strdup (g_value_get_string (value));
      break;
 
    case G_TYPE_UINT:
      str_value = g_strdup_printf ("%u", g_value_get_uint (value));
      break;

    default:
      g_warning ("%s : Invalid type", G_STRFUNC);
      break;
  }

  clutter_model_iter_get_value (iter, 0, &title_value);
  query = g_strdup_printf ("UPDATE %s SET %s = '%s' WHERE Title = '%s'", TABLE, name, str_value, g_value_get_string (&title_value));

  error_code = sqlite3_prepare_v2 (priv_model->sql, query, strlen (query), &statement, NULL);
  g_free (query);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [prepare] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv_model->sql));
    return;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_DONE) {
    g_critical ("%s [step] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv_model->sql));
    return;
  }

  sqlite3_finalize (statement);
}

static ClutterModelIter *
gmc_video_model_iter_next (ClutterModelIter *iter)
{
  ClutterModel *model;
  guint row, n_rows;

  model = clutter_model_iter_get_model (iter);
  row = clutter_model_iter_get_row (iter);
  n_rows = clutter_model_get_n_rows (model);

  g_object_unref (iter);

  if (row == (n_rows - 1)) {
    return NULL;
  }
  iter = g_object_new (GMC_TYPE_VIDEO_MODEL_ITER,
                       "model", model,
                       "row", row + 1,
                       NULL);
  return iter;
}

static ClutterModelIter *
gmc_video_model_iter_prev (ClutterModelIter *iter)
{
  return NULL;
}

static void
gmc_video_model_iter_class_init (GmcVideoModelIterClass *klass)
{
  ClutterModelIterClass *model_iter_class = CLUTTER_MODEL_ITER_CLASS (klass);

  model_iter_class->get_value = gmc_video_model_iter_get_value;
  model_iter_class->set_value = gmc_video_model_iter_set_value;
  /*
  gboolean          (* is_first)  (ClutterModelIter *iter);
  gboolean          (* is_last)   (ClutterModelIter *iter);
  */

  model_iter_class->next = gmc_video_model_iter_next;
  model_iter_class->prev = gmc_video_model_iter_prev;
}

static void
gmc_video_model_iter_init (GmcVideoModelIter *self)
{
  ;
}

/*
 * GmcVideoModel
 */

G_DEFINE_TYPE (GmcVideoModel, gmc_video_model, CLUTTER_TYPE_MODEL);

enum {
  PROP_0,
  PROP_FILENAME
};

void
gmc_video_model_set_filename (GmcVideoModel *self, const gchar *filename)
{
  GmcVideoModelPrivate *priv;
  sqlite3_stmt *statement;
  gchar *query;
  gint error_code;
  gboolean create = FALSE;

  priv = GMC_VIDEO_MODEL_GET_PRIVATE (self);

  if (g_strcmp0 (priv->filename, filename) == 0) {
    return;
  }

  if (priv->filename) {
    g_free (priv->filename);
  }
  priv->filename = g_strdup (filename);

  if (priv->sql) {
    error_code = sqlite3_close (priv->sql);
    if (error_code != SQLITE_OK) {
      g_critical ("%s [close] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    }
  }

  error_code = sqlite3_open_v2 (priv->filename, &priv->sql, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [open] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return;
  }

  query = g_strdup_printf ("SELECT COUNT (*) FROM sqlite_master WHERE name LIKE '%s'", TABLE);
  error_code = sqlite3_prepare_v2 (priv->sql, query, strlen (query), &statement, NULL);
  g_free (query);
  if (error_code != SQLITE_OK) {
    g_critical ("%s prepare : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_ROW) {
    g_critical ("%s step : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return;
  }

  if (sqlite3_column_int (statement, 0) == 0) {
    create = TRUE;
  }

  sqlite3_finalize (statement);

  if (create == FALSE) {
    return;
  }

  query = g_strdup_printf ("CREATE TABLE %s (Title CHAR, Year INTEGER)", TABLE);
  error_code = sqlite3_prepare_v2 (priv->sql, query, strlen (query), &statement, NULL);
  g_free (query);
  if (error_code != SQLITE_OK) {
    g_critical ("%s prepare : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_DONE) {
    g_critical ("%s step : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return;
  }

  sqlite3_finalize (statement);
}

static void
gmc_video_model_set_property (GObject      *object,
                              guint         prop_id,
                              const GValue *value,
                              GParamSpec   *pspec)
{
  GmcVideoModel *self;

  self = GMC_VIDEO_MODEL (object);

  switch (prop_id)
  {
    case PROP_FILENAME:
      gmc_video_model_set_filename (self, g_value_get_string (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gmc_video_model_get_property (GObject    *object,
                              guint       prop_id,
                              GValue     *value,
                              GParamSpec *pspec)
{
  GmcVideoModel *self;
  GmcVideoModelPrivate *priv;

  self = GMC_VIDEO_MODEL (object);
  priv = GMC_VIDEO_MODEL_GET_PRIVATE (priv);

  switch (prop_id)
  {
    case PROP_FILENAME:
      g_value_set_string (value, priv->filename);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static guint
gmc_video_model_get_n_rows (ClutterModel *model)
{
  GmcVideoModel *self;
  GmcVideoModelPrivate *priv;
  sqlite3_stmt *statement;
  gint error_code;
  guint n_rows;
  gchar *query;

  self = GMC_VIDEO_MODEL (model);
  priv = GMC_VIDEO_MODEL_GET_PRIVATE (self);

  query = g_strdup_printf ("SELECT COUNT (*) FROM %s", TABLE);
  error_code = sqlite3_prepare_v2 (priv->sql, query, strlen (query), &statement, NULL);
  g_free (query);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [prepare] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_ROW) {
    g_critical ("%s [step] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  n_rows = sqlite3_column_int (statement, 0);

  sqlite3_finalize (statement);

  return n_rows;
}

static ClutterModelIter *
gmc_video_model_insert_row (ClutterModel *model,
                            gint          index)
{
  GmcVideoModelIter *iter;
  GmcVideoModelPrivate *priv;
  sqlite3_stmt *statement;
  guint n_rows;
  gint error_code;
  gchar *query;

  priv = GMC_VIDEO_MODEL_GET_PRIVATE (model);

  n_rows = clutter_model_get_n_rows (model);

  query = g_strdup_printf ("INSERT INTO %s (Title) VALUES ('_')", TABLE);

  error_code = sqlite3_prepare_v2 (priv->sql, query, strlen (query), &statement, NULL);
  g_free (query);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [prepare] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }
  
  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_DONE) {
    g_critical ("%s [step] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  sqlite3_finalize (statement);

  iter = g_object_new (GMC_TYPE_VIDEO_MODEL_ITER,
                       "model", model,
                       "row", n_rows,
                       NULL);

  return CLUTTER_MODEL_ITER (iter);
}

static void
gmc_video_model_remove_row (ClutterModel *model,
                            guint         row)
{
}

static ClutterModelIter *
gmc_video_model_get_iter_at_row (ClutterModel *model,
                                 guint         row)
{
  GmcVideoModelPrivate *priv;
  GmcVideoModelIter *iter;
  gchar *query, *tmp;
  gint column;
  sqlite3_stmt *statement;
  gint error_code;

  priv = GMC_VIDEO_MODEL_GET_PRIVATE (model);

  query = g_strdup_printf ("SELECT Title FROM %s", TABLE);

  column = clutter_model_get_sorting_column (model);
  if (column != -1) {
    tmp = g_strdup_printf ("%s ORDER BY %s", query, clutter_model_get_column_name (model, column));
    g_free (query);
    query = tmp;
  }

  tmp = g_strdup_printf ("%s LIMIT %d, 1", query, row);
  g_free (query);
  query = tmp;

  error_code = sqlite3_prepare_v2 (priv->sql, query, strlen (query), &statement, NULL);
  g_free (query);
  if (error_code != SQLITE_OK) {
    g_critical ("%s [prepare] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  error_code = sqlite3_step (statement);
  if (error_code != SQLITE_ROW) {
    g_critical ("%s [step] : %d - %s", G_STRFUNC, error_code, sqlite3_errmsg (priv->sql));
    return 0;
  }

  sqlite3_finalize (statement);

  iter = g_object_new (GMC_TYPE_VIDEO_MODEL_ITER,
                       "model", model,
                       "row", row,
                       NULL);

  return CLUTTER_MODEL_ITER (iter);
}

static void
gmc_video_model_resort (ClutterModel         *model,
                        ClutterModelSortFunc  func,
                        gpointer              data)
{
}

static void
gmc_video_model_class_init (GmcVideoModelClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);
  ClutterModelClass *model_class = CLUTTER_MODEL_CLASS (klass);

  g_type_class_add_private (object_class, sizeof (GmcVideoModelPrivate));

  object_class->set_property = gmc_video_model_set_property;
  object_class->get_property = gmc_video_model_get_property;

  model_class->get_n_rows = gmc_video_model_get_n_rows;
  model_class->insert_row = gmc_video_model_insert_row;
  model_class->remove_row = gmc_video_model_remove_row;
  model_class->get_iter_at_row = gmc_video_model_get_iter_at_row;
  model_class->resort = gmc_video_model_resort;

  g_object_class_install_property (object_class,
                                   PROP_FILENAME,
                                   g_param_spec_string ("filename",
                                                        "Filename",
                                                        "The database filename",
                                                        "movies_default.db",
                                                        G_PARAM_READABLE | G_PARAM_WRITABLE | G_PARAM_CONSTRUCT));

}

static void
gmc_video_model_init (GmcVideoModel *self)
{
  clutter_model_set_names (CLUTTER_MODEL (self),
                           G_N_ELEMENTS (names),
                           names);
  clutter_model_set_types (CLUTTER_MODEL (self),
                           G_N_ELEMENTS (types),
                           types);
}

ClutterModel *
gmc_video_model_new (const gchar *filename)
{
  GObject *self;

  self = g_object_new (GMC_TYPE_VIDEO_MODEL,
                       "filename", filename,
                       NULL);

  return CLUTTER_MODEL (self);
}
