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

void click_button(GtkBuilder *builder, GtkWidget *window, char *idButton, void (*pf)){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(builder, idButton));
  g_signal_connect (button,"clicked",G_CALLBACK(pf),window);
}


//###################
// open windows

/*
function: open_new_res_window
Open window_new_reservation
*/
void open_new_res_window(GtkWidget *Widget,GtkWidget *window){
    GtkBuilder      *builder;
    gtk_widget_destroy(window);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder,GLADE_FILE, NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_new_reservation"));
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);
    gtk_widget_show(window);
}

void open_reservations_window(GtkWidget *Widget,GtkWidget *window){
    GtkBuilder      *builder;
    gtk_widget_destroy(window);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder,GLADE_FILE, NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_reservations"));
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);
    gtk_widget_show(window);
}



void open_place_room_window(GtkWidget *Widget,GtkWidget *window){
    GtkBuilder      *builder;
    gtk_widget_destroy(window);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder,GLADE_FILE, NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_place_room"));
    gtk_builder_connect_signals(builder, NULL);

    g_object_unref(builder);
    gtk_widget_show(window);
}
//#############
