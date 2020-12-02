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
function: on_window_main_destroy
*/
void on_window_main_destroy(){
    gtk_main_quit();
}


/*
function: homekamaji
Open Window_home
*/
void open_home_window(char *idWindow){
    GtkBuilder      *builder;
    GtkWidget       *window;

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder,GLADE_FILE, NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, idWindow));
    gtk_builder_connect_signals(builder, NULL);

    click_button(builder, window, "button_home_reservations", &open_reservations_window);
    click_button(builder, window, "button_home_search", &open_new_res_window);
    click_button(builder, window, "button_home_calendars", &open_place_room_window);

    g_object_unref(builder);
    gtk_widget_show(window);


    gtk_main();

}

/*
click_button
*/
void click_button(GtkBuilder *builder, GtkWidget *window, char *idButton, void (*pf)){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(builder, idButton));
  g_signal_connect (button,"clicked",G_CALLBACK(pf),window);
}

/*
close_and_open_window
*/
GtkBuilder *close_and_open_window(GtkWidget *oldWindow, char *idNewWindow){
  GtkBuilder      *builder;
  gtk_widget_destroy(oldWindow);

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder,GLADE_FILE, NULL);
  oldWindow = GTK_WIDGET(gtk_builder_get_object(builder, idNewWindow));
  gtk_builder_connect_signals(builder, NULL);

  gtk_widget_show(oldWindow);
  return builder;
}


//###################
// open windows

/*
function: open_new_res_window
Open window_new_reservation
*/
void open_reservations_window(GtkWidget *Widget,GtkWidget *currentWindow){
  close_and_open_window(currentWindow,"window_reservations");
}

void open_new_res_window(GtkWidget *Widget,GtkWidget *currentWindow){
  GtkBuilder *builder;
  builder = close_and_open_window(currentWindow,"window_new_reservation");
  click_button(builder, currentWindow, "button_new_res", &open_equipment_window);
  g_object_unref(builder);
}

void open_place_room_window(GtkWidget *Widget,GtkWidget *currentWindow){
  GtkBuilder *builder;
  builder = close_and_open_window(currentWindow,"window_place_room");
  click_button(builder, currentWindow, "button_place_room", &open_planning_window);
  g_object_unref(builder);
}

void open_equipment_window(GtkWidget *Widget,GtkWidget *currentWindow){
    close_and_open_window(currentWindow,"window_equipment");
}

void open_planning_window(GtkWidget *Widget,GtkWidget *currentWindow){
    close_and_open_window(currentWindow,"window_planning");
}



//#############
