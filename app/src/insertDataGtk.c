/*
insertDataGtk.c

Call the data in the database and inject them in the windows.

*/



// BACK

void back(GtkWidget *widget, gpointer data){
  Session *session = data;
  gtk_widget_destroy( GTK_WIDGET( session->window ) );
  (*session->backFunction)(NULL, session);
}


// ------------------------------------------------

void comboBoxTextFill( GtkComboBoxText *comboBoxText, char *firstRow, char *request ){

  MYSQL_ROW row;
  MYSQL *conn = connect_db();
  MYSQL_RES *result;

  //empty the comboBoxText
  gtk_combo_box_text_remove_all (comboBoxText);

  //Add the first row
  if(firstRow != NULL){
    gtk_combo_box_text_append(comboBoxText,"0", firstRow);
    gtk_combo_box_set_active_id (GTK_COMBO_BOX(comboBoxText), "0");
  }

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
  char equipments[4][16] = {"monitor", "camera", "whiteboard", "projector"};
  char id[32];
  char location[64];
  char price[8];
  RoomGtkBox *room = malloc( sizeof(RoomGtkBox) );
  if( room == NULL ) exit(1);

  GtkBuilder *b = gtk_builder_new_from_file(GLADE_FILE);
  room->builder = b;
  room->box = GTK_BOX(gtk_builder_get_object(b, "box_available_list_element"));
  room->locationLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_available_list_element_where"));
  room->timeSlotLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_available_list_element_time_slot"));
  room->priceHalfDay = GTK_LABEL(gtk_builder_get_object(b, "lbl_available_list_element_price"));
  room->nbPersons = GTK_LABEL(gtk_builder_get_object(b, "lbl_available_list_element_left_nb"));
  room->bookingTimeSlotComboBox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(b, "combo_available_list_element_when"));
  room->bookingButton = GTK_BUTTON(gtk_builder_get_object(b, "button_available_list_element_booking"));
  for(int i = 0; i < 4; i++){
    sprintf(id, "img_rooms_available_%s", equipments[i]);
    room->equipments[i] = GTK_IMAGE(gtk_builder_get_object(b,id));
  }

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

ReservationBox * newReservation(MYSQL_ROW row){
  char location[64];
  char price[8];
  char equipments[4][16] = {"monitor","camera","whiteboard","projector"};
  char id[32];
  ReservationBox *reservation = malloc( sizeof(ReservationBox) );
  if( reservation == NULL ) exit(1);

  GtkBuilder *b = gtk_builder_new_from_file(GLADE_FILE);
  reservation->idBooking = (uint32_t)atoi(row[0]);
  reservation->box = GTK_BOX(gtk_builder_get_object(b, "box_reservation"));
  reservation->locationLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_reservation_where"));
  reservation->dateLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_reservation_when"));
  reservation->timeSlotLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_reservation_time_slot"));
  reservation->priceLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_reservation_price"));
  for(int i = 0; i < 4; i++){
    sprintf(id, "img_reservations_%s", equipments[i]);
    reservation->equipments[i] = GTK_IMAGE(gtk_builder_get_object(b, id));
  }
  reservation->drinks[0] = GTK_IMAGE(gtk_builder_get_object(b, "img_reservations_coffee"));
  reservation->drinks[1] = GTK_IMAGE(gtk_builder_get_object(b, "img_reservations_tea"));
  reservation->edit = GTK_BUTTON(gtk_builder_get_object(b, "button_reservation_edit"));
  reservation->delete = GTK_BUTTON(gtk_builder_get_object(b, "button_reservation_delete"));

  sprintf(location, "%s - %s", row[2], row[3]); // Room name - Place name
  sprintf(price, "%s€", row[5]);

  gtk_label_set_text( reservation->locationLabel, location );
  gtk_label_set_text( reservation->priceLabel, price );
  setDateReservation( row[6], reservation->dateLabel ); // date
  gtk_label_set_text( reservation->timeSlotLabel, row[7] ); // time slot
  displayRoomEquipments( reservation->equipments, row[1] );
  displayReservationDrinks( reservation->drinks, row[0] );

  //style
  background_color( GTK_WIDGET(reservation->box) , "#FFFFFF");
  return reservation;
}

void displayReservationDrinks(GtkImage *drinks[2], char *idBooking){

  int *drinksArray = getRoomsEquipment(idBooking);
  for(int i = 0; i < 2; i++){
    if(drinksArray[i])
      gtk_widget_show ( GTK_WIDGET( drinks[i] ) );
    else
      gtk_widget_hide ( GTK_WIDGET( drinks[i] ) );
  }

  free(drinksArray);
}

int *getReservationDrinks(char *idBooking){
  MYSQL *conn = connect_db();
  MYSQL_ROW row;
  MYSQL_RES *result;
  char request[128];
  int *drinks;
  uint8_t nbDrinks = 2;

  drinks = malloc( sizeof(int) * nbDrinks);
  if( drinks == NULL) exit(1);
  for(int i = 0; i < 2; i++) drinks[i] = 0;

  sprintf( request, "SELECT drink FROM _booking_include_drink \
  INNER JOIN BOOKING as B WHERE B.id = %s ;", idBooking);

  result = query(conn, request);
  while ((row = mysql_fetch_row(result)) != NULL)
    drinks[ atoi(*row)-1 ] = 1;

  mysql_free_result(result);
  mysql_close(conn);

  return drinks;
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

  if( search->time_slot != 2 && isRestDayAvailable( search->date, search->time_slot, idRoom ) == 1 ){ // available the rest of the day
    gtk_combo_box_set_active_id (GTK_COMBO_BOX(room->bookingTimeSlotComboBox), idTimeSlot );
    gtk_widget_show( GTK_WIDGET( room->bookingTimeSlotComboBox ) );
  }else if(search->time_slot != 2){
      gtk_combo_box_text_remove_all (room->bookingTimeSlotComboBox);
      gtk_combo_box_text_append(room->bookingTimeSlotComboBox, idTimeSlot, time_slots[search->time_slot] );
      gtk_combo_box_set_active_id (GTK_COMBO_BOX(room->bookingTimeSlotComboBox), idTimeSlot );
    }
}

// ------------------------

void displayTimeSlotLabel(GtkLabel *label, char *idRoom, Date date, int time_slot){
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char idTimeSlot[4];
  char timeSlot[16];
  sprintf( idTimeSlot, "%d", time_slot);

  if( time_slot == 2 || isRestDayAvailable( date, time_slot, idRoom ) == 1 )
    strcpy( timeSlot, time_slots[2] );
  else
    strcpy( timeSlot, time_slots[ time_slot ] );

  gtk_label_set_text( label, timeSlot );
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

  updateButtonsPlanning(session->calendar, session->today);

  free(startDate);
}

// ------------------------

void updateWeekLabel( int *startDate, GtkLabel *week){
  char months[12][5] = {"jan", "fev", "mar", "avr", "mai", "juin", "juil", "aou", "sep", "oct", "nov", "dec"};
  char weekLabel[32];
  int *endDate;

  endDate = moveInCalendar(startDate[0], startDate[1], startDate[2], 4 );
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

void updateButtonsPlanning( Calendar *calendar, struct tm *td){
  char timeSlots[2][12] = {"8h - 14h", "14h - 20h"};
  char date[16];
  char idRoom[4];
  int i, j;
  int isAvailable;
  Date planning = calendar->planning;
  GtkWidget *button;

  sprintf(idRoom, "%d", calendar->id_room);
  for(i = 0; i < 2; i++){
    for(j = 0; j < 5; j++){
      button = GTK_WIDGET(calendar->buttonsBooking[i][j]);
      background_color(button, "#ffffff");

      int *startDate = moveInCalendar(planning.year, planning.month, planning.day, j);
      sprintf(date, "%d-%d-%d", startDate[0], startDate[1]+1, startDate[2]);
      isAvailable = isTimeSlotAvailable(timeSlots[i], date, idRoom);

      if(startDate[0] <= td->tm_year + 1900 && startDate[1]  <= td->tm_mon && startDate[2]  <= td->tm_mday)
        isAvailable = -1;

      showOrHidePlanningButton(isAvailable, button);
      free(startDate);
    }
  }
}

void showOrHidePlanningButton(int isAvailable, GtkWidget *button){
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
}

// ------------------------

void updateTimeSlotLabels(Calendar *c){
  char days[5][8] = {"Lun", "Mar", "Mer", "Jeu", "Ven"};
  char months[12][8] = {"janv", "fev", "mars", "avr", "mai", "juin",\
                        "juill", "aout", "sept", "oct", "nov", "dec"};
  char date[20];
  char timeSlots[3][12] = {"8h - 14h", "14h - 20h", "8h - 20h"};

  sprintf(date, "%s %d %s %d", days[c->wDaySelected], c->daySelected.day, months[c->daySelected.month -1], c->daySelected.year);
  gtk_label_set_text(c->dateLabel, date);
  gtk_label_set_text(c->timeSlotLabel, timeSlots[c->timeSlotSelected]);

}

// ------------------------

void setDateReservation(char *dateSQL, GtkLabel *label){
  char months[12][8] = {"janv", "fev", "mars", "avr", "mai", "juin",\
                        "juill", "aout", "sept", "oct", "nov", "dec"};
  Date d;
  char date[32];

  sscanf(dateSQL, "%d-%d-%d", &d.year, &d.month, &d.day);
  sprintf(date, "%d %s %d", d.day, months[d.month - 1], d.year);

  gtk_label_set_text(label, date);
}

// ------------------------

void confirmDeleteReservation(GtkWidget *widget, gpointer data){
  ReservationBox *reservation = data;
  MYSQL *conn = connect_db();
  MYSQL_ROW row;
  MYSQL_RES *result;
  char request[128];

  sprintf(request, "update BOOKING set state = 0 where id = %u;",reservation->idBooking);
  result = query(conn, request);

  gtk_widget_destroy(GTK_WIDGET(reservation->dialogWindow));
  gtk_widget_destroy(GTK_WIDGET(reservation->session->window));
  open_reservations_window(NULL,reservation->session);
  mysql_free_result(result);

  free(result);
  mysql_close(conn);
}

// ------------------------


void focusDateCalendar(GtkCalendar *calendar){
  time_t now;
  struct tm *td;
  int *tomorrow;

  time( &now );
  td = localtime( &now );
  tomorrow = moveInCalendar(td->tm_year+1900, td->tm_mon, td->tm_mday,1);
  gtk_calendar_select_month (calendar, tomorrow[1], tomorrow[0] );
  gtk_calendar_select_day ( calendar, tomorrow[2] );

  free(tomorrow);
}

// ------------------------

uint8_t checkDataInputPlace(GtkComboBox *place){
    uint32_t id;

    if( gtk_combo_box_get_active_id(place) != NULL ){
      id = (uint32_t)atoi( gtk_combo_box_get_active_id(place) );
      return  id > 0;
    }
  return 0;
}

// ------------------------

uint8_t checkDataCalendar(GtkCalendar *calendar){
  Date d;
  time_t now;
  struct tm *td;

  gtk_calendar_get_date ( calendar, (guint *)&d.year, (guint *)&d.month, (guint *)&d.day);
  time( &now );
  td = localtime( &now );
  return !(d.year <= td->tm_year+1900 && d.month <= td->tm_mon && d.day <= td->tm_mday);
}


// ------------------------

void checkDataNewRes(GtkWidget *widget, gpointer data){
  GtkWidget **check = data;
  /*printf("place: %u\n",checkDataInputPlace( GTK_COMBO_BOX(check[1]) ) );
  printf("calendar: %u\n",checkDataCalendar( GTK_CALENDAR(check[2] )));*/

  if( checkDataInputPlace( GTK_COMBO_BOX(check[1]) ) && checkDataCalendar( GTK_CALENDAR(check[2] )) )
    gtk_widget_set_sensitive( GTK_WIDGET(check[0]), TRUE );
  else
    gtk_widget_set_sensitive( GTK_WIDGET(check[0]), FALSE );
}

// ------------------------

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






//
