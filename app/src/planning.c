/*
planning.c
-------------------------
The user can navigate in the reservation planning of a room.
He choose which room of wich place then see the planning and
the data corresponding of the room and the time slot selected.
He can also book a time slot.
*/

#include "kamaji.h"

/*
******************************
*      PLACE ROOM WINDOW     *
******************************
*/

/*
-----------------------------------------------------------------------------------------------------------
Function : open_place_room_window
-------------------------
Open the window with the selection of place and room as you want for reservation
-------------------------
 GtkWidget *widget: widget activated to open the window, here the button_home_calendars button
 Session *session : address of the struct Session
*/
void open_place_room_window(GtkWidget *widget, gpointer data){
  Session *session = data;
  GtkComboBoxText *place, *room;
  GtkButton *nextButton, *backButton;

  close_and_open_window(session,"window_place_room");

  session->backFunction = open_home_window;
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_place_room") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  place = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(session->builder, "combo_place_room_place"));
  room = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(session->builder, "combo_place_room_room"));

  // fill the comboBoxText with places from db
  comboBoxTextFill( place,"Choisir un lieu", "SELECT id, name FROM PLACE WHERE state = 1" );
  g_signal_connect( place,"changed",G_CALLBACK(fillComboBoxRooms),room);

  //check data
  nextButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_place_room") );
  gtk_widget_set_sensitive(GTK_WIDGET(nextButton), FALSE);
  g_signal_connect(room, "changed", G_CALLBACK(checkDataPlaceRoom),nextButton);

  click_button(session, "button_place_room", getIdRoom);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getIdRoom
-------------------------
Retrieve id of room selected in the place room window
-------------------------
 GtkWidget *widget: widget activated, here the validation button in place room window
 Session *session : address of the struct Session
*/
void getIdRoom(GtkWidget *widget, Session *session){
  GtkComboBox *place = GTK_COMBO_BOX( gtk_builder_get_object( session->builder, "combo_place_room_place" ) );
  GtkComboBox *room = GTK_COMBO_BOX( gtk_builder_get_object( session->builder, "combo_place_room_room" ) );
  Calendar *calendar = session->calendar;

  calendar->id_room = atoi( gtk_combo_box_get_active_id(room) );
  calendar->id_place = atoi( gtk_combo_box_get_active_id(place) );

  open_planning_window(NULL, session);
}


/*
-----------------------------------------------------------------------------------------------------------
Function: checkDataPlaceRoom
-------------------------
Set the button to validate in place room window on sensitive or not if aroom is selected.
-------------------------
GtkWidget *widget: widget activated.
*/
void checkDataPlaceRoom(GtkWidget *widget, gpointer data){
  GtkWidget *button = data;
  GtkComboBox *room = GTK_COMBO_BOX( widget );
  uint8_t id;

  if( gtk_combo_box_get_active_id(room) != NULL){
    id = (uint8_t)atoi(gtk_combo_box_get_active_id(room));
    if( id > 0 )
      gtk_widget_set_sensitive(button, TRUE);
    else
      gtk_widget_set_sensitive(button, FALSE);
  }
}




/*
******************************
*       PLANNING WINDOW      *
******************************
*/
