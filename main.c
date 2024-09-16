#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>
#include <cairo.h>
#include <gtk/gtk.h>
#include "header.h"
#include <glib.h>

#define MAX_INT 2147483647

typedef struct _Edge Edge;
typedef struct _Transition Transition;
typedef struct _Vertex Vertex;

static gboolean press_button = false;

Vertex *selected_vertex_start = NULL;
Vertex *selected_vertex_end = NULL;

void findFastestPath(Vertex *current, Vertex *destination, int currentTime, int *minTime, int *visited, GList **path, GList **min_path) {
   visited[current->index] = 1;
   *path = g_list_append(*path, current);

   if (current == destination) {
        if (currentTime < *minTime) {
            *minTime = currentTime;
            if (*min_path) {
                g_list_free(*min_path);
            }
            *min_path = g_list_copy(*path);
        }
    } else {
        // Перебор всех переходов из текущей вершины
        for (int i = 0; i < current->num_transitions; i++) {
            Transition *transition = current->transitions[i];
            if (!visited[transition->to->index]) {
                timetravel = currentTime + transition->time;
                if (press_button){
                    timetravel -= 1.5;
                }
                findFastestPath(transition->to, destination, timetravel, minTime, visited, path, min_path);
            }
        }
        // Перебор всех ребер из текущей вершины
        for (int i = 0; i < current->num_edges; i++) {
            Edge *edge = current->edges[i];
            if (!visited[edge->to->index]) {
                timetravel = currentTime + edge->time
                if (press_button){
                    timetravel -= 1.5;
                }
                findFastestPath(edge->to, destination, timetravel, minTime, visited, path, min_path);
            }
        }
    }

    visited[current->index] = 0;
    *path = g_list_remove(*path, current);
}

Vertex *create_vertex(int index, char *name, GdkRGBA color, int x, int y, int time_on, int time_exit) {
    Vertex *v = malloc(sizeof(Vertex));
    v->index = index;
    v->name = name;
    v->time_on = time_on;
    v->time_exit = time_exit;
    v->color = color;
    v->x = x;
    v->y = y;
    v->size = 10;
    v->edges = NULL;
    v->transitions = NULL;
    v->num_edges = 0;
    v->num_transitions = 0;
    v->last_index = -1;
    return v;
}

void add_edge(Vertex *v1, Vertex *v2, int time) {
    Edge *edge = malloc(sizeof(Edge));
    edge->from = v1;
    edge->to = v2;
    edge->time = time;

    v1->edges = realloc(v1->edges, (v1->num_edges + 1) * sizeof(Edge *));
    v1->edges[v1->num_edges++] = edge;

    Edge *reverse_edge = malloc(sizeof(Edge));
    reverse_edge->from = v2;
    reverse_edge->to = v1;
    reverse_edge->time = time;

    v2->edges = realloc(v2->edges, (v2->num_edges + 1) * sizeof(Edge *));
    v2->edges[v2->num_edges++] = reverse_edge;
}

void add_transition(Vertex *v1, Vertex *v2, int time) {
    Transition *transition = malloc(sizeof(Transition));
    transition->from = v1;
    transition->to = v2;
    transition->time = time;

    v1->transitions = realloc(v1->transitions, (v1->num_transitions + 1) * sizeof(Transition *));
    v1->transitions[v1->num_transitions++] = transition;

    Transition *reverse_transition = malloc(sizeof(Transition));
    reverse_transition->from = v2;
    reverse_transition->to = v1;
    reverse_transition->time = time;

    v2->transitions = realloc(v2->transitions, (v2->num_transitions + 1) * sizeof(Transition *));
    v2->transitions[v2->num_transitions++] = reverse_transition;
}

void draw_vertices(cairo_t *cr, Vertex *v, double current_angle, double angle_step) {
    gdk_cairo_set_source_rgba(cr, &v->color);
    cairo_move_to(cr, v->x, v->y);
    cairo_arc(cr, v->x, v->y, v->size, current_angle, angle_step);
    cairo_line_to(cr, v->x, v->y);
    cairo_fill(cr);
}

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
    Vertex **vertices = (Vertex **)data;

    int *row = (int *)malloc(sizeof(int) * 80);

    for (int i = 0; i < 80; i++) {
        row[i] = 0;
    }
    
    for (int i = 0; i < 69; i++) {
        Vertex *v = vertices[i];
        int num_vertices = v->num_transitions;
        
        if (num_vertices > 0 && row[v->index] != 1) {
            double angle_step = 2 * G_PI / (num_vertices+1);
            double current_angle = 1.5 * G_PI;
            int n = 1; 
            if (num_vertices == 2){
                n = -6;
            }

            for (int j = 0; j < num_vertices; j++) {
                Transition *t = v->transitions[j];
                Vertex *v1 = t->to;
                Vertex *v2 = t->from;
                if (j > 0) {
                    draw_vertices(cr, v1, current_angle, current_angle + angle_step);
                    cairo_move_to(cr, v->x + n*15, v->y + 4 * (j+4));
                    cairo_show_text(cr, v1->name);
                    row[v1->index] = 1;
                } else {
                    draw_vertices(cr, v1, current_angle, current_angle + angle_step);
                    cairo_move_to(cr, v->x + n*15, v->y + 4 * (j-2));
                    cairo_show_text(cr, v1->name);
                    row[v1->index] = 1;
                    current_angle += angle_step;
                    draw_vertices(cr, v2, current_angle, current_angle + angle_step);
                    cairo_move_to(cr, v->x + n*15, v->y + 4 * (j+3));
                    cairo_show_text(cr, v2->name);
                    row[v2->index] = 1;
                }
                current_angle += angle_step;
            }
        } else if (row[v->index] != 1) {
            draw_vertices(cr, v, 0, 2 * G_PI);
            row[v->index] = 1;
            cairo_move_to(cr, v->x + 15, v->y + 5);
            cairo_show_text(cr, v->name);
        }

        for (int j = 0; j < v->num_edges; j++) {
            Edge *e = v->edges[j];
            Vertex *adj_to = e->to;
            gdk_cairo_set_source_rgba(cr, &v->color);
            cairo_move_to(cr, v->x, v->y);
            cairo_line_to(cr, adj_to->x, adj_to->y);
            cairo_stroke(cr);
        }
    }
    return TRUE;
}

static bool is_transition(Vertex *v1, Vertex *v2) {
    for (int i = 0; i < v1->num_transitions; i++) {
        Transition *t = v1->transitions[i];
        if (t->to == v2) {
            return true;
        } 
    }
    return false;
}

void transition_size(Vertex *v) {
    for (int i = 0; i < v->num_transitions; i++) {
        Transition *t = v->transitions[i];
        t->from->size = 10;
        t->to->size = 10;
    }
    v->size = 15;
}

// Функция для получения вершины по координатам
Vertex *selectVertex(Vertex **vertices, int x, int y) {
    const double threshold = 10.0; // Радиус, в пределах которого ищем вершину

    // Поиск вершины в данной точке
    for (int i = 0; i < 69; i++) {
        Vertex *v = vertices[i];
        double distance = sqrt(pow(v->x - x, 2) + pow(v->y - y, 2));
        if (distance <= threshold) {
            if (v->last_index == -1){
                v->last_index = 0;
                return v;
            } else {
                if (v->last_index == v->num_transitions) {
                    v->last_index = -1;
                    return NULL;
                }
                v->transitions[v->last_index]->to->size = 15;
                ++v->last_index;
                return v->transitions[v->last_index-1]->to;
            }
        }
    }
    return NULL;
}

static gboolean on_mouse_press(GtkWidget *widget, GdkEventButton *event, gpointer data) {
    Vertex **vertices = (Vertex **)data;
    Vertex *selected_vertex = selectVertex(vertices, event->x, event->y);
    if (event->button == GDK_BUTTON_PRIMARY) {
        if (selected_vertex_start != NULL) selected_vertex_start->size = 10;
        if (selected_vertex != NULL) {
            selected_vertex->size = 15;
            selected_vertex_start = selected_vertex;
        } else {
            selected_vertex_start = NULL;
        } 

    } else if (event->button == GDK_BUTTON_SECONDARY) {
        if (selected_vertex != NULL) {
            selected_vertex->size = 15;
            selected_vertex_end = selected_vertex;
        } else {
            selected_vertex_end = NULL;
        }
    }
    gtk_widget_queue_draw(widget);  // Перерисовка виджета
    if(selected_vertex_start != NULL && selected_vertex_end != NULL) {
        find_path(selected_vertex_start, selected_vertex_end);
        printf("Start: %s, End: %s\n", selected_vertex_start->name, selected_vertex_end->name);
        for (int i=0; i<selected_vertex_start->num_transitions; i++) {
            Transition *t = selected_vertex_start->transitions[i];
            t->to->size = 10;
            t->from->size = 10;
        }
        selected_vertex_start = NULL;
        selected_vertex_end = NULL;
    }
    return true;
}

void find_path(Vertex *start, Vertex *end) {
    GList *path = NULL, *min_path = NULL;
    int min_time = MAX_INT;
    int *visited = (int *)calloc(69, sizeof(int));

    gint64 start_time = g_get_monotonic_time(); // Засекаем время перед запуском алгоритма

    findFastestPath(start, end, 0, &min_time, visited, &path, &min_path);
    
    gint64 end_time = g_get_monotonic_time(); // Засекаем время после выполнения алгоритма
    gint64 elapsed_time = end_time - start_time;

    g_print("Время работы алгоритма: %lld микросекунд\n", elapsed_time);

    free(visited);

    // Вывод результата в GUI
    if (min_time != MAX_INT) {
        GString *path_str = g_string_new("");
        for (GList *l = min_path; l != NULL; l = l->next) {
            Vertex *v = l->data;
            if (l->next != NULL)
                g_string_append_printf(path_str, "%s -> ", v->name);
            else
                g_string_append_printf(path_str, "%s", v->name);
        }

        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                   "Путь от %s до %s: %s \nВремя пути: %d мин.", 
                                                   start->name, end->name, path_str->str, min_time);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
        GtkWidget *dialog = gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
                                                   GTK_MESSAGE_INFO, GTK_BUTTONS_OK,
                                                   "Путь от %s до %s не найден.",
                                                   start->name, end->name);
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
    start->size = 10;
    end->size = 10;
    g_list_free(min_path);
    g_list_free(path);
}

static void get_vertices_from_db(sqlite3 *db, Vertex **vertices) {
    // Определение цветов
    GdkRGBA colors[] = {
        {0.8, 0.0, 0.0, 1.0},  // Красный
        {0.0, 0.0, 1.0, 1.0},  // Синий
        {0.0, 0.8, 0.0, 1.0},  // Зелёный
        {0.9, 0.5, 0.0, 1.0},  // Оранжевый
        {0.5, 0.0, 0.5, 1.0}   // Фиолетовый
    };
    sqlite3_stmt *res;
    char *sql = "SELECT * FROM Station;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);    
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
    }    

    while ((rc = sqlite3_step(res)) == SQLITE_ROW) {
        int id = sqlite3_column_int(res, 0);
        const unsigned  char *name = sqlite3_column_text(res, 1);
        int time_on = sqlite3_column_int(res, 2);
        int time_exit = sqlite3_column_int(res, 3);
        int x = sqlite3_column_int(res, 5);
        int y = sqlite3_column_int(res, 6);
        int color_link = sqlite3_column_int(res, 4);
        if (color_link >= 1 && color_link <= 5) {
            vertices[id-1] = create_vertex(id, g_strdup_printf("%s", name), colors[color_link - 1], x, y, time_on, time_exit);
        }
        
        //printf("ID: %d, Name: %s\n", id, name);
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error in SQL execution: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);
}

static void get_edges_from_db(sqlite3 *db, Vertex **vertices) {
    sqlite3_stmt *res;
    char *sql = "SELECT * FROM Times;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);    
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
    }    

    while ((rc = sqlite3_step(res)) == SQLITE_ROW) {
        int from_station_id = sqlite3_column_int(res, 0);
        int to_station_id = sqlite3_column_int(res, 1);
        int time = sqlite3_column_int(res, 2);
        int peah_time = sqlite3_column_int(res, 3);
        add_edge(vertices[from_station_id-1], vertices[to_station_id-1], time);
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error in SQL execution: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);
}

static void get_transitions_from_db(sqlite3 *db, Vertex **vertices) {
    sqlite3_stmt *res;
    char *sql = "SELECT * FROM Transfers;";

    int rc = sqlite3_prepare_v2(db, sql, -1, &res, 0);    
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Failed to fetch data: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
    }    

    while ((rc = sqlite3_step(res)) == SQLITE_ROW) {
        int from_station_id = sqlite3_column_int(res, 0);
        int to_station_id = sqlite3_column_int(res, 1);
        int time = sqlite3_column_int(res, 2);
        add_transition(vertices[from_station_id-1], vertices[to_station_id-1], time);
    }

    if (rc != SQLITE_DONE) {
        fprintf(stderr, "Error in SQL execution: %s\n", sqlite3_errmsg(db));
    }
    sqlite3_finalize(res);
}

static void click_peak_button(GtkWidget *widget, gpointer data) {
    GdkRGBA color;
    // Проверяем текущее состояние и меняем цвет
    if (press_button) {
        // Если кнопка зеленая, меняем на красный
        gdk_rgba_parse(&color, "red");
        press_button = FALSE; // Обновляем состояние
    } else {
        // Если кнопка красная, меняем на зеленый
        gdk_rgba_parse(&color, "green");
        press_button = TRUE; // Обновляем состояние
    }
    
    gtk_widget_override_background_color(widget, GTK_STATE_NORMAL, &color);
}

int main(int argc, char **argv) {
    // Создание массива вершин
    Vertex *vertices[69]; // Указатель на массив вершин

    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open("db.db", &db);
    
    if (rc != SQLITE_OK) {
        
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        
        return 1;
    }
    
    get_vertices_from_db(db, vertices);
    get_edges_from_db(db, vertices);
    get_transitions_from_db(db, vertices);
    
    sqlite3_close(db);
    gtk_init(&argc, &argv);

    // Создание окна и виджетов
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *drawing_area = gtk_drawing_area_new();

    // Создаем контейнер для размещения кнопки
    GtkWidget *fixed = gtk_fixed_new();
    gtk_container_add(GTK_CONTAINER(window), fixed);

    GtkWidget *button = gtk_button_new_with_label("Част пик");
    GdkRGBA red;
    gdk_rgba_parse(&red, "red");
    gtk_widget_override_background_color(button, GTK_STATE_NORMAL, &red);
    
    // Добавляем кнопку в нижний левый угол
    gtk_fixed_put(GTK_FIXED(fixed), button, 10, 250); // 10 пикселей от левого края и 250 от верхнего

    //gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), "Metro SPB");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 700);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Подключаем сигнал нажатия кнопки
    g_signal_connect(button, "clicked", G_CALLBACK(click_peak_button), NULL);

    gtk_container_add(GTK_CONTAINER(window), drawing_area);
    g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_callback), vertices);
    g_signal_connect(G_OBJECT(drawing_area), "button-press-event", G_CALLBACK(on_mouse_press), vertices);

    gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area) | GDK_BUTTON_PRESS_MASK);

    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);


    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

