/*
import_glade.h

last modif: 30 nov 2020
*/

void open_home_window(char *idWindow);

void click_button(GtkBuilder *builder, char *idButton,void (*pf));
GtkBuilder *close_and_open_window(GtkBuilder *builder, char *idOldWindow, char *idNewWindow);
GtkBuilder *newWindow(char* file, char* idWindow);
void background_color( GtkWidget **widget, char *color );

//NAVIGATION
void open_reservations_window(GtkWidget *widget,gpointer builder);
void open_new_res_window(GtkWidget *widget,gpointer builder);
void open_place_room_window(GtkWidget *widget,gpointer builder);
void open_equipment_window(GtkWidget *widget,gpointer builder);
void open_drink_window(GtkWidget *Widget,gpointer builder);
void open_planning_window(GtkWidget *widget,gpointer builder);
void open_rooms_available_window(GtkWidget *Widget,gpointer builder);
void open_drink_window_2(GtkWidget *Widget,gpointer builder);
void open_reservations_window2(GtkWidget *widget,gpointer builder);

void on_window_main_destroy();
void on_window_new_booking_destroy();





//
