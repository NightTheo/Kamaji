/*
import_glade.h

last modif: 30 nov 2020
*/

typedef struct Window{
  GtkWidget   **window;
  GtkBuilder  **builder;
} Window;


void open_home_window(char *idWindow);

void click_button(Window window, char *idButton,void (*pf));
Window close_and_open_window(Window oldWindow, char *idNewWindow);
Window newWindow(char *file, char *idNewWindow);

//open windows
void open_reservations_window(GtkWidget *widget, gpointer window);
void open_new_res_window(GtkWidget *widget, gpointer window);
void open_place_room_window(GtkWidget *widget, gpointer window);
void open_equipment_window(GtkWidget *widget, gpointer window);
void open_planning_window(GtkWidget *widget, gpointer window);

void on_window_main_destroy();
void on_window_new_booking_destroy();





//
