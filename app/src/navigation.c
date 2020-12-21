/*
import_glade.c

create the windows from the file glade in ui/glade/home.glade
connet the signals for the navigation between the windows

*/

#include <gtk/gtk.h>
#include "../inc/navigation.h"
#include "../inc/RetrieveDataFromInput.h"
#include "../inc/insertDataGtk.h"


//defines
#define GLADE_FILE "ui/glade/home.glade"


/*
function: homekamaji
Open Window_home
*/
void open_home_window(char *idWindow){
  Session *session = malloc(sizeof(Session));
  newWindow(GLADE_FILE, idWindow, session);
  click_button(session, "button_home_reservations", &open_reservations_window);
  click_button(session, "button_home_search", &open_new_res_window);
  click_button(session, "button_home_calendars", &open_place_room_window);

}

/*
click_button
*/
void click_button(Session *session, char *idButton, void (*pf)){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(session->builder, idButton));
  g_signal_connect (button,"clicked",G_CALLBACK(pf),session);
}

/*
close_and_open_window
*/

void close_and_open_window(Session *session, char *idNewWindow){
  gtk_widget_destroy( GTK_WIDGET( session->window ) );
  newWindow(GLADE_FILE, idNewWindow, session);
}

/*
newWindow
*/
void newWindow(char* file, char* idWindow, Session *session){
  GtkBuilder      *builder;
  GtkWidget       *window;

  builder = gtk_builder_new_from_file(file);
  window = GTK_WIDGET(gtk_builder_get_object(builder, idWindow));
  gtk_builder_connect_signals(builder, NULL);
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

  session->builder = builder;
  session->window = GTK_WINDOW(window);

  //Background color
  background_color(&window, "#444444" );

  gtk_widget_show_all(window);
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

void open_reservations_window(GtkWidget *widget,gpointer data){
  Session *session = data;
  close_and_open_window(session,"window_reservations");
}

void open_new_res_window(GtkWidget *widget, gpointer data){
  Search *search;

  GtkComboBox *inputplace;
  GtkComboBox *inputTime;
  GtkSpinButton *inputNbPeoples;
  GtkCalendar *date;

  Session *session = data;
  close_and_open_window(session, "window_new_reservation");

  //liste déroulante pour le lieu séléctionner de la réservation
  inputplace = GTK_COMBO_BOX(gtk_builder_get_object(session->builder, "combo_new_reservation_where"));
  comboBoxTextFill( GTK_COMBO_BOX_TEXT(inputplace) ,"Choisir un lieu", "SELECT id, name FROM PLACE WHERE state = 1" );
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect(inputplace,"changed",G_CALLBACK(retrieveDataCBoxText),NULL);

  //liste déroulante pour le temps passé dans une salle
  inputTime= GTK_COMBO_BOX(gtk_builder_get_object(session->builder, "combo_new_reservation_when"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect(inputTime,"changed",G_CALLBACK(retrieveDataCBoxText),NULL);

  //liste déroulante pour le nombre de personne saisi
  inputNbPeoples = GTK_SPIN_BUTTON(gtk_builder_get_object(session->builder, "spin_new_reservation_group"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect(inputNbPeoples,"changed",G_CALLBACK(retrieveDataSpin),NULL);

  //Calendrier Take Date
  date = GTK_CALENDAR(gtk_builder_get_object(session->builder, "calendar_new_res"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect(date,"day-selected",G_CALLBACK(retrieveDataCalendar),NULL);

  click_button(data, "button_new_res", open_equipment_window);
}

//PLACE ROOM
void open_place_room_window(GtkWidget *widget,gpointer data){
  Session *session = data;
  GtkComboBoxText *place, *room;

  close_and_open_window(session,"window_place_room");
  place = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(session->builder, "combo_place_room_place"));
  room = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(session->builder, "combo_place_room_room"));
  gtk_builder_connect_signals(session->builder, NULL);

  // fill the comboBoxText with places from db
  comboBoxTextFill( place,"Choisir un lieu", "SELECT id, name FROM PLACE WHERE state = 1" );
  g_signal_connect( place,"changed",G_CALLBACK(fillRooms),room);


  click_button(session, "button_place_room", open_planning_window);
}

/*
* place - comboBoxText of the places
* room - comboBoxText of the rooms
*/
void fillRooms(GtkComboBoxText *place,gpointer room){
  char *id;
  char request[64] = "SELECT id,name FROM ROOM WHERE state = 1 AND place = ";

  id = retrieveDataCBoxText( place );
  strcat(request, id );
  comboBoxTextFill( room, "Choisir une salle", request );

}

void open_equipment_window(GtkWidget *Widget,gpointer data){
  GtkCheckButton *checkMonitor;
  GtkCheckButton *checkWhiteboard;
  GtkCheckButton *checkCamera;
  GtkCheckButton *checkProjector;

  Session *session = data;

  close_and_open_window(session, "window_equipment");

  //moniteur : id = check_equipment_monitor
  checkMonitor = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_equipment_monitor"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect( GTK_TOGGLE_BUTTON(checkMonitor),"toggled",G_CALLBACK(retrieveDataCheckButton),NULL);

  //whiteboard : id = check_equipment_whiteboard
  checkWhiteboard = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_equipment_whiteboard"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect( GTK_TOGGLE_BUTTON(checkWhiteboard),"toggled",G_CALLBACK(retrieveDataCheckButton),NULL);

  //camera : id = check_equipment_camera
  checkCamera = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_equipment_camera"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect( GTK_TOGGLE_BUTTON(checkCamera),"toggled",G_CALLBACK(retrieveDataCheckButton),NULL);

  //projector : id = check_equipment_projector
  checkProjector = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_equipment_projector"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect( GTK_TOGGLE_BUTTON(checkProjector),"toggled",G_CALLBACK(retrieveDataCheckButton),NULL);



  click_button(session, "button_equipment_search", open_drink_window);

}

void open_drink_window(GtkWidget *Widget,gpointer data){
  Session *session = data;
  GtkCheckButton *checkCoffee;
  GtkCheckButton *checkTea;

  close_and_open_window(session, "window_drink");

  //coffe : id = check_drink_coffee
  checkCoffee = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_drink_coffee"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect( GTK_TOGGLE_BUTTON(checkCoffee),"toggled",G_CALLBACK(retrieveDataCheckButton),NULL);

  //tea : id = check_drink_tea
  checkTea = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_drink_tea"));
  gtk_builder_connect_signals(session->builder, NULL);
  g_signal_connect( GTK_TOGGLE_BUTTON(checkTea),"toggled",G_CALLBACK(retrieveDataCheckButton),NULL);

  click_button(session, "button_drink_next", open_rooms_available_window);
}

void open_rooms_available_window(GtkWidget *Widget,gpointer data){
  Session *session = data;
  close_and_open_window(session,"window_rooms_available");
}

void open_planning_window(GtkWidget *Widget,gpointer data){
  Session *session = data;
  close_and_open_window(session,"window_planning");

  click_button(session, "button_planning_next", open_drink_window_2);
}

void open_drink_window_2(GtkWidget *Widget,gpointer data){
  Session *session = data;
  close_and_open_window(session, "window_drink");

  click_button(session, "button_drink_next", open_reservations_window2);
}

void open_reservations_window2(GtkWidget *widget,gpointer data){
  Session *session = data;
  close_and_open_window(session, "window_reservations");
}

//#############
