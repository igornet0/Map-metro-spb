#include <gtk/gtk.h>

typedef struct _Edge Edge;
typedef struct _Transition Transition;

typedef struct _Vertex {
    int index;
    char *name;
    int time_on;
    int time_exit;
    GdkRGBA color;
    int x, y; // Координаты для отрисовки
    int size; // Размер
    Edge **edges; // Список рёбер
    Transition **transitions; // Список переходов
    int num_edges; // Количество рёбер
    int num_transitions; // Количество переходов
    int last_index;
} Vertex;

typedef struct _Edge {
    Vertex *from;
    Vertex *to;
    int time;
} Edge;

typedef struct _Transition {
    Vertex *from;
    Vertex *to;
    int time;
} Transition;

Vertex *create_vertex(int index, char *name, GdkRGBA color, int x, int y, int time_on, int time_exit);

void add_edge(Vertex *v1, Vertex *v2, int time);

void findFastestPath(Vertex *current, Vertex *destination, int currentTime, int *minTime, int *visited, GList **path, GList **min_path);

void find_path(Vertex *start, Vertex *end);

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data);

static gboolean on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

static gboolean on_mouse_press(GtkWidget *widget, GdkEventButton *event, gpointer data);

static gboolean on_key_release(GtkWidget *widget, GdkEventKey *event, gpointer user_data);

static gboolean on_mouse_release(GtkWidget *widget, GdkEventButton *event, gpointer user_data);

static gboolean on_motion(GtkWidget *widget, GdkEventMotion *event, gpointer user_data);

static gboolean on_scroll(GtkWidget *widget, GdkEventScroll *event, gpointer user_data);

static gboolean on_draw(GtkWidget *widget, cairo_t *cr, gpointer user_data);

static gboolean on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer user_data);
