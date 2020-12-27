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

// ------------------------

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

// ----------------------

void displayTimeSlotComboBox(RoomGtkBox *room, char *idRoom, Search *search){
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char idTimeSlot[4];
  sprintf( idTimeSlot, "%d", search->time_slot);

  if( search->time_slot != 2 && isRestDayAvailable( search, idRoom ) ){ // available the rest of the day
    gtk_combo_box_set_active_id (GTK_COMBO_BOX(room->bookingTimeSlotComboBox), idTimeSlot );
    gtk_widget_show( GTK_WIDGET( room->bookingTimeSlotComboBox ) );
  }else if(search->time_slot != 2){
      gtk_combo_box_text_remove_all (room->bookingTimeSlotComboBox);
      gtk_combo_box_text_append(room->bookingTimeSlotComboBox, idTimeSlot, time_slots[search->time_slot] );
      gtk_combo_box_set_active_id (GTK_COMBO_BOX(room->bookingTimeSlotComboBox), idTimeSlot );
    }
}

// ------------------------

void displayTimeSlotLabel(RoomGtkBox *room, char *idRoom, Search *search){
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char idTimeSlot[4];
  char timeSlot[16];
  sprintf( idTimeSlot, "%d", search->time_slot);

  if( search->time_slot == 2 || isRestDayAvailable( search, idRoom ) )
    strcpy( timeSlot, time_slots[2] );
  else{
    strcpy( timeSlot, time_slots[ search->time_slot ] );
  }
  gtk_label_set_text( room->timeSlotLabel, timeSlot );

}

// ------------------------

void planningNumbers(Calendar *calendar, struct tm *date){
  int *startDate;
  int weekDay;

  weekDay = date->tm_wday > 0 ? date->tm_wday -1 : 6 ; // s|m|t|w|t|f|s --> m|t|w|t|f|s|s   / format angl -> euro
  if( weekDay < 5 ) // monday -> friday
    startDate = moveInCalendar(date->tm_year + 1900,date->tm_mon, date->tm_mday, - weekDay);
  else
    startDate = moveInCalendar(date->tm_year + 1900,date->tm_mon, date->tm_mday, 7 - weekDay);

  calendar->planning.year = startDate[0];
  calendar->planning.month = startDate[1];
  calendar->planning.day = startDate[2];
  updatePlanningNumbers(startDate, calendar->days);

  free(startDate);
}

void updatePlanningNumbers(int *startDate, GtkLabel *days[5]){
  char charNumber[4];
  int *intNumber;
  int i;

  for(i = 0; i < 5; i++){
    intNumber = moveInCalendar(startDate[0], startDate[1], startDate[2], i);
    sprintf(charNumber, "%d", intNumber[2] );
    gtk_label_set_text( days[i], charNumber );
    free(intNumber);
  }
}

/*
month [ 0 - 11 ]
day   [ 1 - 31 ]
*/
int *moveInCalendar(int year, int month, int day, int move){
  int daysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int *date;

  date = malloc(sizeof(int) * 3);
  if( date == NULL) exit(1);

  date[0] = year; date[1] = month; date[2] = day;
  if( !move ) return date;
  if( month < 0 || month > 11 || day < 1 || day > 31 ) return NULL;

  if( year % 400 == 0 || ( year % 100 != 0 && year % 4 == 0) )
      daysInMonth[1] = 29;  // Leap year | année bissextile

  day += move;
  // move > 0
  while( day > daysInMonth[ month ]){
    day -= daysInMonth[ month ];
    month++;
    if( month > 11){
      month = 0;
      year++;
    }
  }

  //move < 0
  while( day < 1 ){
    if( month == 0 ) month = 12;
    day = daysInMonth[ month - 1 ] + day;
    month--;
    if( month == 11){
      year--;
    }
  }

  date[0] = year; date[1] = month; date[2] = day;
  return date;
}


// ------------------------

void click_button_planning(Session *session, char *idButton){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(session->builder, idButton));

  g_signal_connect (button,"clicked",G_CALLBACK(planningChangeWeek),session);

}

void planningChangeWeek(GtkWidget *widget, gpointer data){
  Session *session = data;
  Date planning;
  GtkWidget *prev;
  int *startDate;
  char *labelButton;
  int move = 0;

  planning = session->calendar->planning;
  labelButton = (char*)gtk_button_get_label( GTK_BUTTON(widget) );

  if( strcmp(labelButton, ">") == 0 )
    move = 7;
  else if(strcmp(labelButton, "<") == 0)
    move = -7;
  else
    exit(1);

  startDate = moveInCalendar( planning.year, planning.month, planning.day, move );
  updatePlanningNumbers(startDate, session->calendar->days);

  session->calendar->planning.year = startDate[0];
  session->calendar->planning.month = startDate[1];
  session->calendar->planning.day = startDate[2];

  prev = GTK_WIDGET( gtk_builder_get_object(session->builder, "button_planning_weeks_back") );
  if( planning.year <= session->today->tm_year + 1900 && planning.month <= session->today->tm_mon && planning.day <= session->today->tm_mday)
    gtk_widget_hide(prev);
  else
    gtk_widget_show(prev);

  free(startDate);
}








//
