/*
import_glade.c

create the windows from the file glade in ui/glade/home.glade
connet the signals for the navigation between the windows

last modif: 08 décembre 2020
*/

#include <gtk/gtk.h>
#include "../inc/import_glade.h"
#include "../inc/RetrieveDataFromInput.h"

//defines
#define GLADE_FILE "ui/glade/home.glade"


/*
function: homekamaji
Open Window_home
*/
void open_home_window(char *idWindow){

  GtkBuilder *builder = newWindow(GLADE_FILE, idWindow);

  click_button(builder, "button_home_reservations", &open_reservations_window);
  click_button(builder, "button_home_search", &open_new_res_window);
  click_button(builder, "button_home_calendars", &open_place_room_window);

}

/*
click_button
*/
void click_button(GtkBuilder *builder, char *idButton, void (*pf)){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(builder, idButton));
  g_signal_connect (button,"clicked",G_CALLBACK(pf),builder);
}

/*
close_and_open_window
*/
GtkBuilder *close_and_open_window(GtkBuilder *builder, char *idOldWindow, char *idNewWindow){
  gtk_widget_destroy( GTK_WIDGET(gtk_builder_get_object(builder, idOldWindow)) );
  g_object_unref(G_OBJECT(builder));
  return newWindow(GLADE_FILE, idNewWindow);
}

/*
newWindow
*/
GtkBuilder *newWindow(char* file, char* idWindow){
  GtkBuilder      *builder;
  GtkWidget       *window;

  builder = gtk_builder_new();
  gtk_builder_add_from_file (builder,file, NULL);
  window = GTK_WIDGET(gtk_builder_get_object(builder, idWindow));
  gtk_builder_connect_signals(builder, NULL);
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

  //Background color
  background_color(&window, "#444444" );


  gtk_widget_show(window);
  return builder;
}

void background_color( GtkWidget **widget, char *color ){
  GtkCssProvider * cssProvider = gtk_css_provider_new();    //store the css

  char css[64] = "* { background-image:none; background-color:";
  strcat( strcat( css , color ), ";}" );

  gtk_css_provider_load_from_data(cssProvider, css,-1,NULL);
  GtkStyleContext * context = gtk_widget_get_style_context(*widget);   //manage CSS provider
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}

//##### NAVIGATION ####################################################

/*
function: open_new_res_window
Open window_new_reservation
*/
void open_reservations_window(GtkWidget *widget,gpointer builder){
  close_and_open_window(builder,"window_home","window_reservations");
}

void open_new_res_window(GtkWidget *widget,gpointer builder){
  GtkBuilder *newBuilder;
  GtkComboBox *inputplace;
  GtkComboBox *inputTime;
  GtkSpinButton *inputNbPeoples;
  GtkCalendar *date;
  newBuilder = close_and_open_window(builder,"window_home", "window_new_reservation");

  //liste déroulante pour le lieu séléctionner de la réservation
  inputplace = GTK_COMBO_BOX(gtk_builder_get_object(newBuilder, "combo_new_reservation_where"));
  gtk_builder_connect_signals(newBuilder, NULL);
  g_signal_connect(inputplace,"changed",G_CALLBACK(retrieveDataCBox),inputplace);

  //liste déroulante pour le temps passé dans une salle
  inputTime= GTK_COMBO_BOX(gtk_builder_get_object(newBuilder, "combo_new_reservation_when"));
  gtk_builder_connect_signals(newBuilder, NULL);
  g_signal_connect(inputTime,"changed",G_CALLBACK(retrieveDataCBox),inputTime);

  //liste déroulante pour le nombre de personne saisi
  inputNbPeoples = GTK_SPIN_BUTTON(gtk_builder_get_object(newBuilder, "spin_new_reservation_group"));
  gtk_builder_connect_signals(newBuilder, NULL);
  g_signal_connect(inputNbPeoples,"changed",G_CALLBACK(retrieveDataSpin),inputNbPeoples);

  //Calendrier Take Date
  date = GTK_CALENDAR(gtk_builder_get_object(newBuilder, "calendar_new_res"));
  gtk_builder_connect_signals(newBuilder, NULL);
  g_signal_connect(date,"day-selected",G_CALLBACK(retrieveDataCalendar),date);

  click_button(newBuilder, "button_new_res", open_equipment_window);
}

void open_place_room_window(GtkWidget *widget,gpointer builder){
  GtkBuilder *newBuilder;
  newBuilder = close_and_open_window(builder, "window_home","window_place_room");

  click_button(newBuilder, "button_place_room", open_planning_window);
}

void open_equipment_window(GtkWidget *Widget,gpointer builder){
  GtkBuilder *newBuilder;
  newBuilder = close_and_open_window(builder,"window_new_reservation", "window_equipment");

  click_button(newBuilder, "button_equipment_search", open_drink_window);
}

void open_drink_window(GtkWidget *Widget,gpointer builder){
  GtkBuilder *newBuilder;
  newBuilder = close_and_open_window(builder,"window_equipment", "window_drink");

  click_button(newBuilder, "button_drink_next", open_rooms_available_window);
}

void open_rooms_available_window(GtkWidget *Widget,gpointer builder){
  close_and_open_window(builder,"window_drink", "window_rooms_available");
}

void open_planning_window(GtkWidget *Widget,gpointer builder){
  GtkBuilder *newBuilder;
  newBuilder = close_and_open_window(builder, "window_place_room","window_planning");

  click_button(newBuilder, "button_planning_next", open_drink_window_2);
}

void open_drink_window_2(GtkWidget *Widget,gpointer builder){
  GtkBuilder *newBuilder;
  newBuilder = close_and_open_window(builder,"window_planning", "window_drink");

  click_button(newBuilder, "button_drink_next", open_reservations_window2);
}

void open_reservations_window2(GtkWidget *widget,gpointer builder){
  close_and_open_window(builder,"window_drink","window_reservations");
}

//#############
