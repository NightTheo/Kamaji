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

void displayRoomEquipments(GtkImage *equipments[4], char *idRoom){

  int *equipmentsArray = getRoomsEquipment(idRoom);
  for(int i = 0; i < 4; i++){
    if(equipmentsArray[i])
      gtk_widget_show ( GTK_WIDGET( equipments[i] ) );
    else
      gtk_widget_hide ( GTK_WIDGET( equipments[i] ) );
  }

  free(equipmentsArray);
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

  if( search->time_slot != 2 && isRestDayAvailable( search, idRoom ) == 1 ){ // available the rest of the day
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

  if( search->time_slot == 2 || isRestDayAvailable( search, idRoom ) == 1 )
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

  updateWeekLabel(startDate, calendar->week);

  free(startDate);
}

// ------------------------

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

// ------------------------
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

// ------------------------

void planningChangeWeek(GtkWidget *widget, gpointer data){
  Session *session = data;
  Date planning;
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
  updateWeekLabel(startDate, session->calendar->week);

  session->calendar->planning.year = startDate[0];
  session->calendar->planning.month = startDate[1];
  session->calendar->planning.day = startDate[2];

  updateButtonsPlanning(session->calendar);

  free(startDate);
}

// ------------------------

void updateWeekLabel( int *startDate, GtkLabel *week){
  char months[12][5] = {"jan", "fev", "mar", "avr", "mai", "juin", "juil", "aou", "sep", "oct", "nov", "dec"};
  char weekLabel[32];
  int *endDate;

  endDate = moveInCalendar(startDate[0], startDate[1], startDate[2], 5 );
  sprintf(weekLabel, "%d %s - %d %s %d",startDate[2],months[startDate[1]],endDate[2],months[endDate[1]], endDate[0] );
  gtk_label_set_text(week, weekLabel);

  free(endDate);
}

// ------------------------

void setRoomInfo(Calendar *calendar){
  MYSQL *conn = connect_db();
  MYSQL_ROW row;
  MYSQL_RES *result;
  char request[128];
  char id[4];
  char price[12];

  // get room name, place name and price half day
  sprintf(request, "select r.name, p.name, r.price_half_day from ROOM as r \
  inner join PLACE as p on r.place = p.id where r.state = 1 and r.id = %d", calendar->id_room);
  result = query(conn, request);
  if( (row = mysql_fetch_row(result)) != NULL){
    gtk_label_set_text(calendar->room, row[0]);
    gtk_label_set_text(calendar->place, row[1]);
    sprintf(price, "%s,00€", row[2] );
    gtk_label_set_text(calendar->price, price);
  }

  sprintf(id, "%d", calendar->id_room);
  displayRoomEquipments(calendar->equipments, id);

  mysql_free_result(result);
  mysql_close(conn);
}

// ------------------------

void updateButtonsPlanning(Calendar *calendar){
  char timeSlots[2][12] = {"8h - 14h", "14h - 20h"};
  char date[16];
  char idRoom[4];
  int *startDate;
  int i, j;
  int isAvailable;
  Date planning = calendar->planning;
  GtkWidget *button;

  sprintf(idRoom, "%d", calendar->id_room);
  for(i = 0; i < 2; i++){
    for(j = 0; j < 5; j++){
      startDate = moveInCalendar(planning.year, planning.month, planning.day, j);
      sprintf(date, "%d-%d-%d", startDate[0], startDate[1]+1, startDate[2]);
      isAvailable = isTimeSlotAvailable(timeSlots[i], date, idRoom);
      button = GTK_WIDGET(calendar->buttonsBooking[i][j]);
      if( isAvailable == 1 ){
        gtk_widget_set_opacity( button, 1 );
        gtk_widget_set_sensitive( button, TRUE );
      }
      else if( isAvailable == -1 ){
        gtk_widget_set_opacity( button, 0 );
        gtk_widget_set_sensitive( button, FALSE );
      }else{
        printf("Error\n");
        exit(0);
      }

      free(startDate);
    }
  }
}








//
