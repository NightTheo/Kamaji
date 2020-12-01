/*
import_glade.h

last modif: 30 nov 2020
*/

void open_home_window(char *idWindow);

void open_new_res_window(GtkWidget *Widget,GtkWidget *window);
void open_reservations_window(GtkWidget *Widget,GtkWidget *window);
void open_place_room_window(GtkWidget *Widget,GtkWidget *window);

void on_window_main_destroy();
void on_window_new_booking_destroy();

void click_button(GtkBuilder *builder, GtkWidget *window, char *idButton,void (*pf));




//
