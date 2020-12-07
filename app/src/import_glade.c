/*
import_glade.c

create the windows from the file glade in ui/glade/home.glade
connet the signals for the navigation between the windows

last modif: 30 nov 2020
*/

#include <gtk/gtk.h>
#include "../inc/import_glade.h"

//defines
#define GLADE_FILE "ui/glade/home.glade"


/*
function: homekamaji
Open Window_home
*/
void open_home_window(char *idWindow){

    Window window = newWindow(GLADE_FILE, idWindow);
    gtk_widget_show_all(*window.window);
/*
    click_button(window, "button_home_reservations", open_reservations_window);
    click_button(window, "button_home_search", open_new_res_window);
    click_button(window, "button_home_calendars", open_place_room_window);
*/

    gtk_main();

}



/*
click_button
*/
void click_button(Window window, char *idButton, void (*pf)){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(window->builder, idButton));
  g_signal_connect (button,"clicked",G_CALLBACK(pf), &window);

}

/*
close_and_open_window
*/
Window close_and_open_window(Window oldWindow, char *idNewWindow){
  gtk_widget_destroy(*oldWindow.window);
  return newWindow(GLADE_FILE, idNewWindow);
}

/*
newWindow
*/
Window newWindow(char *file, char *idNewWindow){
  GtkWidget       *window;
  GtkBuilder      *builder;

  builder = gtk_builder_new_from_file(file);
  window = GTK_WIDGET(gtk_builder_get_object(builder, idNewWindow));
  gtk_builder_connect_signals(builder, NULL);
  g_signal_connect(window, "delete-event", G_CALLBACK(gtk_main_quit), NULL );

  Window windowStruct = {&window, &builder};
  //Window *ptr = &windowStruct;

  //gtk_widget_show_all(window);
  return windowStruct;
}







//###################
// open windows

/*
function: open_new_res_window
Open window_new_reservation
*/

void open_reservations_window(GtkWidget *widget, gpointer window){
  close_and_open_window(*window,"window_reservations");
}

void open_new_res_window(GtkWidget *widget, gpointer window){
  Window newWindow;
  newWindow = close_and_open_window(*window,"window_new_reservation");
  //click_button(newWindow, "button_new_res", open_equipment_window);

}

void open_place_room_window(GtkWidget *widget, gpointer window){
  Window newWindow;
  newWindow = close_and_open_window(*window,"window_place_room");
  click_button(newWindow, "button_place_room", &open_planning_window);
}

void open_equipment_window(GtkWidget *widget, gpointer window){
  Window newWindow;
  newWindow = close_and_open_window(*window,"window_equipment");
}

void open_planning_window(GtkWidget *widget, gpointer window){
  Window newWindow;
  newWindow = close_and_open_window(*window,"window_planning");
}



//#############
