/*
insertDataGtk.c

Call the data in the database and inject them in the windows.

*/


void comboBoxTextFill( GtkComboBoxText *comboBoxText, char *firstRow, char *request ){

  MYSQL_ROW row;
  MYSQL *conn = connect_db();
  MYSQL_RES *result;

  //empty the comboBoxText
  gtk_combo_box_text_remove_all (comboBoxText);

  //Add the first row
  gtk_combo_box_text_append(comboBoxText,"0", firstRow);
  gtk_combo_box_set_active_id (GTK_COMBO_BOX(comboBoxText), "0");

  result = query(conn, request);

  while ((row = mysql_fetch_row(result)) != NULL)
    gtk_combo_box_text_append ( comboBoxText, row[0], row[1]);

  mysql_free_result(result);
  mysql_close(conn);

}

// ------------------------

/*
* place - comboBoxText of the places
* room - comboBoxText of the rooms
*/
void fillComboBoxRooms(GtkComboBoxText *place,gpointer room){
  char *id;
  char request[64] = "SELECT id,name FROM ROOM WHERE state = 1 AND place = ";

  id = (char*)gtk_combo_box_get_active_id( GTK_COMBO_BOX(place) );
  strcat(request, id );
  comboBoxTextFill( room, "Choisir une salle", request );

}

// ------------------------


RoomGtkBox * newRoomAvailable(MYSQL_ROW row){
  //char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  RoomGtkBox *room = malloc( sizeof(RoomGtkBox) );
  if( room == NULL ) exit(1);

  room->builder = gtk_builder_new_from_file(GLADE_FILE);
  room->box = GTK_BOX(gtk_builder_get_object(room->builder, "box_available_list_element"));
  room->locationLabel = GTK_LABEL(gtk_builder_get_object(room->builder, "lbl_available_list_element_where"));
  room->timeSlotLabel = GTK_LABEL(gtk_builder_get_object(room->builder, "lbl_available_list_element_time_slot"));
  room->priceHalfDay = GTK_LABEL(gtk_builder_get_object(room->builder, "lbl_available_list_element_price"));
  room->nbPersons = GTK_LABEL(gtk_builder_get_object(room->builder, "lbl_available_list_element_left_nb"));
  room->bookingTimeSlotComboBox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(room->builder, "combo_available_list_element_when"));
  room->bookingButton = GTK_BUTTON(gtk_builder_get_object(room->builder, "button_available_list_element_booking"));
  room->equipments[0] = GTK_IMAGE(gtk_builder_get_object(room->builder, "img_rooms_available_whiteboard"));
  room->equipments[1] = GTK_IMAGE(gtk_builder_get_object(room->builder, "img_rooms_available_monitor"));
  room->equipments[2] = GTK_IMAGE(gtk_builder_get_object(room->builder, "img_rooms_available_projector"));
  room->equipments[3] = GTK_IMAGE(gtk_builder_get_object(room->builder, "img_rooms_available_camera"));

  char location[64];
  char price[8];

  sprintf(location, "%s - %s", row[1], row[2]);
  sprintf(price, "%s€", row[4]);

  gtk_label_set_text( room->locationLabel, location );
  gtk_label_set_text( room->nbPersons, row[3] );
  gtk_label_set_text( room->priceHalfDay, price );

  // time slot available
  //printf("%s\n", time_slots[session->search->time_slot]);
  //style
  background_color( GTK_WIDGET(room->box) , "#FFFFFF");
  background_color( GTK_WIDGET(room->bookingButton) , "#444444");
  return room;
}


// ------------------------

void displayRoomEquipments(RoomGtkBox *room, char *idRoom){

  int *equipments = getRoomsEquipment(idRoom);
  for(int i = 0; i < 4; i++){
    if(equipments[i])
      gtk_widget_show ( GTK_WIDGET( room->equipments[i] ) );
    else
      gtk_widget_hide ( GTK_WIDGET( room->equipments[i] ) );

  }

  free(equipments);
}

int *getRoomsEquipment(char *idRoom){
  // equipment
  MYSQL *conn = connect_db();
  MYSQL_ROW row;
  MYSQL_RES *result;
  char request[1024];
  int *equipments;

  equipments = malloc( sizeof(int) * 4 );
  if(equipments == NULL) exit(1);
  for(int i = 0; i < 4; i++) equipments[i] = 0;

  sprintf(request, "SELECT roe.equipment FROM _room_owns_equipment as roe \
  INNER JOIN EQUIPMENT as E ON roe.equipment = E.id \
  WHERE E.state = 1 AND room = %s ;", idRoom);

  result = query(conn, request);
  while ((row = mysql_fetch_row(result)) != NULL){
    equipments[ atoi(*row)-1 ] = 1;
  }

  mysql_free_result(result);
  mysql_close(conn);

  return equipments;
}

//
