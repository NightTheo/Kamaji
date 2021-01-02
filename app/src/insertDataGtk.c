/*
insertDataGtk.c

Call the data in the database and inject them in the windows.

*/




// INIT SESSION
/*
-----------------------------------------------------------------------------------------------------------
Function : initSession
-------------------------
Allocates memory for the main variables used : session, search and calendar.
Init the linked list of the reservations.
-------------------------
Return value
  Session *session: the session created
*/
Session *initSession(){
  Session *session;
  Search *search;
  Calendar *calendar;

  session = malloc(sizeof(Session));
  search = malloc(sizeof(Search));
  calendar = malloc(sizeof(Calendar));
  if( session == NULL || search == NULL || calendar == NULL)
    exit(1);

  search->startBooking = NULL;

  session->search = search;
  session->calendar = calendar;
  session->nextReservation = NULL;

  return session;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : kamajiQuit
-------------------------
Free memory allocated for main variables search, calendar and session.
Then quit the gtk loop.
-------------------------
GtkWidget *w : widget activated.
Session *session : address of the struct Session
*/
void kamajiQuit(GtkWidget *w, gpointer data){
  Session *session = data;
  /*printf("Delete-event\n");
  printf("Search : %p\n", session->search);
  printf("Calendar : %p\n", session->calendar);*/
  if( session->search != NULL ) free(session->search);
  if( session->calendar != NULL ) free(session->calendar);
  freeDelReservations(&session->nextReservation);
  free(session);
  gtk_main_quit();
}

/*
-----------------------------------------------------------------------------------------------------------
Function : back
-------------------------
Back to the previous window.
-------------------------
GtkWidget *widget : widget activated.
Session *session : address of the struct Session
*/
void back(GtkWidget *widget, gpointer data){
  Session *session = data;
  gtk_widget_destroy( GTK_WIDGET( session->window ) );
  (*session->backFunction)(NULL, session);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : freeDelReservations
-------------------------
Free all the nodes of the linked list of Reservation
-------------------------
delReservation **start : address of the pointer of the last node added.
*/
void freeDelReservations(delReservation **start){
  while(*start != NULL){
    delReservation *remove = *start;
    *start = (*start)->next;
    free(remove);
  }
}

// ------------------------------------------------

/*
-----------------------------------------------------------------------------------------------------------
Function : comboBoxTextFill
-------------------------
Write the first row text and fill the combo box with the result of the select in DB.
-------------------------
GtkComboBoxText *comboBoxText: The comboBox fill.
char *firstRow: Text of the first row
char *request: SQL request
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function : fillComboBoxRooms
-------------------------
Fill the comboBoxText of the rooms in place room window
-------------------------
GtkComboBoxText *place : comboBoxText of the places in place room window
gpointer room : comboBoxText of the rooms in place room window
*/
void fillComboBoxRooms(GtkComboBoxText *place,gpointer room){
  char *id;
  char request[64] = "SELECT id,name FROM ROOM WHERE state = 1 AND place = ";

  id = (char*)gtk_combo_box_get_active_id( GTK_COMBO_BOX(place) );
  strcat(request, id );
  comboBoxTextFill( room, "Choisir une salle", request );

}


/*
-----------------------------------------------------------------------------------------------------------
Function : newRoomAvailable
-------------------------
Create a RoomGtkBox, a struct which contains all the widgets of a room in the list of available rooms.
-------------------------
MYSQL_ROW row: array of strings with the data of the current room from the select in DB.
*/
RoomGtkBox newRoomAvailable(MYSQL_ROW row){
  char equipments[4][16] = {"monitor", "camera", "whiteboard", "projector"};
  char id[32];
  char location[64];
  char price[8];

  RoomGtkBox room;

  GtkBuilder *b = gtk_builder_new_from_file(GLADE_FILE);
  room.builder = b;
  room.box = GTK_BOX(gtk_builder_get_object(b, "box_available_list_element"));
  room.locationLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_available_list_element_where"));
  room.timeSlotLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_available_list_element_time_slot"));
  room.priceHalfDay = GTK_LABEL(gtk_builder_get_object(b, "lbl_available_list_element_price"));
  room.nbPersons = GTK_LABEL(gtk_builder_get_object(b, "lbl_available_list_element_left_nb"));
  room.bookingTimeSlotComboBox = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(b, "combo_available_list_element_when"));
  room.bookingButton = GTK_BUTTON(gtk_builder_get_object(b, "button_available_list_element_booking"));
  for(int i = 0; i < 4; i++){
    sprintf(id, "img_rooms_available_%s", equipments[i]);
    room.equipments[i] = GTK_IMAGE(gtk_builder_get_object(b,id));
  }

  sprintf(location, "%s - %s", row[1], row[2]);
  sprintf(price, "%s€", row[4]);

  gtk_label_set_text( room.locationLabel, location );
  gtk_label_set_text( room.nbPersons, row[3] );
  gtk_label_set_text( room.priceHalfDay, price );

  //style
  background_color( GTK_WIDGET(room.box) , "#FFFFFF");
  background_color( GTK_WIDGET(room.bookingButton) , "#444444");
  return room;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : newReservation
-------------------------
Create a ReservationBox which contains all the widgets of a Reservation in the list of available Reservations.
-------------------------
*/
ReservationBox newReservation(){
  char equipments[4][16] = {"monitor","camera","whiteboard","projector"};
  char id[32];
  ReservationBox reservation;

  GtkBuilder *b = gtk_builder_new_from_file(GLADE_FILE);
  reservation.box = GTK_BOX(gtk_builder_get_object(b, "box_reservation"));
  reservation.locationLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_reservation_where"));
  reservation.dateLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_reservation_when"));
  reservation.timeSlotLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_reservation_time_slot"));
  reservation.priceLabel = GTK_LABEL(gtk_builder_get_object(b, "lbl_reservation_price"));
  for(int i = 0; i < 4; i++){
    sprintf(id, "img_reservations_%s", equipments[i]);
    reservation.equipments[i] = GTK_IMAGE(gtk_builder_get_object(b, id ) );
  }
  reservation.drinks[0] = GTK_IMAGE(gtk_builder_get_object(b, "img_reservations_coffee"));
  reservation.drinks[1] = GTK_IMAGE(gtk_builder_get_object(b, "img_reservations_tea"));
  reservation.delete = GTK_BUTTON(gtk_builder_get_object(b, "button_reservation_delete"));

  background_color( GTK_WIDGET(reservation.box) , "#FFFFFF");
  return reservation;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : displayReservationData
-------------------------
Change the value of the widgets of a reservation, in the reservations list in reservation window.
-------------------------
ReservationBox reservation : struct with all the widgets of a reservation (labels, images, etc)
MYSQL_ROW row : array of strings filled with the data of the curent booking selected in DB.
*/
void displayReservationData(ReservationBox reservation, MYSQL_ROW row){
  char location[64];
  char price[8];

  sprintf(location, "%s - %s", row[2], row[3]);
  sprintf(price, "%s€", row[5]);
  gtk_label_set_text( reservation.locationLabel, location );
  gtk_label_set_text( reservation.priceLabel, price);
  setDateReservation( row[6], reservation.dateLabel ); // date
  gtk_label_set_text( reservation.timeSlotLabel, row[7] ); // time slot
  displayRoomEquipments( reservation.equipments, row[1] );
  displayReservationDrinks( reservation.drinks, row[0] );
}


/*
-----------------------------------------------------------------------------------------------------------
Function : displayReservationDrinks
-------------------------
Display the drinks taken for a reservation. Get the drinks of a reservation by calling
the function getReservationDrinks, then show or hide the corresponding image.
-------------------------
GtkImage *drinks[2]: array of GtkImages* who represente the drinks.
char *idBooking: identifiant of booking.
*/
void displayReservationDrinks(GtkImage *drinks[2], char *idBooking){

  int *drinksArray = getReservationDrinks(idBooking);
  for(int i = 0; i < 2; i++){
    if(drinksArray[i])
      gtk_widget_show ( GTK_WIDGET( drinks[i] ) );
    else
      gtk_widget_hide ( GTK_WIDGET( drinks[i] ) );
  }

  free(drinksArray);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getReservationDrinks
-------------------------
Select in DB the drinks included in a reservation, then fill an int array : 1 if the drink is present, 0 if not.
-------------------------
char *idBooking : id of the table BOOKING
-------------------------
Return value
  int *drinks : address of the array created
*/
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

  sprintf( request, "SELECT drink FROM _booking_include_drink WHERE booking = %s ;", idBooking);

  result = query(conn, request);
  while ((row = mysql_fetch_row(result)) != NULL)
    drinks[ atoi(*row)-1 ] = 1;

  mysql_free_result(result);
  mysql_close(conn);

  return drinks;
}

// ------------------------
/*
-----------------------------------------------------------------------------------------------------------
Function : confirmDeleteReservation
-------------------------
Set the state of reservation in database to 0 for hide the reservation during the display.
Refresh the window for update.
-------------------------
GtkWidget *widget: widget activated.
delReservation *delReservation: address of the struct delReservation.
*/
void confirmDeleteReservation(GtkWidget *widget, gpointer data){
  delReservation *delReservation = data;
  MYSQL *conn = connect_db();
  MYSQL_ROW row;
  MYSQL_RES *result;
  char request[128];

  sprintf(request, "update BOOKING set state = 0 where id = %u;",delReservation->idBooking);
  result = query(conn, request);

  gtk_widget_destroy(GTK_WIDGET(delReservation->dialogWindow));
  open_reservations_window(NULL,delReservation->session);

  mysql_free_result(result);
  mysql_close(conn);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : displayRoomEquipments
-------------------------
Display the equipments in a room. Get the equipments in the room by calling
the function getRoomsEquipment, then show or hide the corresponding image.
-------------------------
GtkImage *equipments[4]: array of GtkImages* who represente the equipments.
char *idRoom: identifiant of room.
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function : getRoomsEquipment
-------------------------
Select in DB the equipments a room has, then fill an int array : 1 if the equipment is present, 0 if not.
-------------------------
char *idRoom : id of the table ROOM
-------------------------
Return value
  int *equipments : address of the array created.
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function : displayTimeSlotComboBox
-------------------------
Fill of room avaible can choose the time of your reservation (morning, afternoon or all day).
It only offers the rooms available at such a time of day.
-------------------------
RoomGtkBox room: struct of the room with all the widgets.
char *idRoom: identifiant of room.
Search *search: address of the struct Search.
*/
void displayTimeSlotComboBox(RoomGtkBox room, char *idRoom, Search *search){
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char idTimeSlot[4];
  sprintf( idTimeSlot, "%d", search->time_slot);

  if( search->time_slot != 2 && isRestDayAvailable( search->date, search->time_slot, idRoom ) == 1 ){ // available the rest of the day
    gtk_combo_box_set_active_id (GTK_COMBO_BOX(room.bookingTimeSlotComboBox), idTimeSlot );
    gtk_widget_show( GTK_WIDGET( room.bookingTimeSlotComboBox ) );
  }else if(search->time_slot != 2){
      gtk_combo_box_text_remove_all (room.bookingTimeSlotComboBox);
      gtk_combo_box_text_append(room.bookingTimeSlotComboBox, idTimeSlot, time_slots[search->time_slot] );
      gtk_combo_box_set_active_id (GTK_COMBO_BOX(room.bookingTimeSlotComboBox), idTimeSlot );
    }
}


/*
-----------------------------------------------------------------------------------------------------------
Function : displayTimeSlotLabel
-------------------------
Display the time slot of availability of a room, if it is all the day, the time slot displayed is "8h - 20h",
else it is "8h - 14h or "14h - 20h".
For that, call the function isRestDayAvailable then set the text in consequence
-------------------------
GtkLabel *label : time slot label
char *idRoom : id of the table ROOM
Date date : date searched
int time_slot : time slot searched
char *idRoom : id of the table ROOM
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function : planningNumbers
-------------------------
Get the current date, transform the week day [0 - 6] to the europeen format.
Update the field planning in Calendar, which store the date of the monday of the week displayed on the planning.
Then call the function updatePlanningNumbers and updateWeekLabel.
-------------------------
Calendar *calendar : address of the calend
struct tm *date : date of today
*/
void planningNumbers(Calendar *calendar, struct tm *date){
  int *startDate;
  int weekDay;

  weekDay = date->tm_wday > 0 ? date->tm_wday -1 : 6 ; // s|m|t|w|t|f|s --> m|t|w|t|f|s|s   /   format angl -> euro
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
  startDate = NULL;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : updatePlanningNumbers
-------------------------
Retrieve the numbers days of all week from the first Monday of the week.
-------------------------
int *startDate: array of three integers{year, month, day} corresponding  of the monday of the week selected.
GtkLabel *days[5]: array of GtkLabel who content the numbers of days.
*/
void updatePlanningNumbers(int *startDate, GtkLabel *days[5]){
  char charNumber[4];
  int *intNumber;
  int i;

  for(i = 0; i < 5; i++){
    intNumber = moveInCalendar(startDate[0], startDate[1], startDate[2], i);
    sprintf(charNumber, "%d", intNumber[2] );
    gtk_label_set_text( days[i], charNumber );
    free(intNumber);
    intNumber = NULL;
  }
}

/*
-----------------------------------------------------------------------------------------------------------
Function : moveInCalendar
-------------------------
From a given date, return the date of N days after or before.
-------------------------
int year : year of the start date
int month : [0 - 11]
int day : [ 1 - 31 ]
int move : -N to days passed, +N to days to come
-------------------------
Return value
  int *date : date after calcul of N days before or after
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



/*
-----------------------------------------------------------------------------------------------------------
Function : click_button_planning
-------------------------
When the user click on the button, the function call planningChangeWeek function.
-------------------------
Session *session: address of the struct Session.
char *idButton: identifiant of the button
*/
void click_button_planning(Session *session, char *idButton){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(session->builder, idButton));

  g_signal_connect (button,"clicked",G_CALLBACK(planningChangeWeek),session);

}


/*
-----------------------------------------------------------------------------------------------------------
Function : planningChangeWeek
-------------------------
Update the calendar when the user change the week.
Call functions for update:
  -updatePlanningNumbers
  -updateWeekLabel
  -updateButtonsPlanning
-------------------------
Session *session: address of the struct Session.
GtkWidget *widget: widget activated.
*/
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
  startDate = NULL;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : updateWeekLabel
-------------------------
Update the label text of the calendar.
-------------------------
int *startDate: array of three integers{year, month, day} corresponding  of the monday of the week selected.
GtkLabel *week: The current week display on the calendar.
*/
void updateWeekLabel( int *startDate, GtkLabel *week){
  char months[12][5] = {"jan", "fev", "mar", "avr", "mai", "juin", "juil", "aou", "sep", "oct", "nov", "dec"};
  char weekLabel[32];
  int *endDate;

  endDate = moveInCalendar(startDate[0], startDate[1], startDate[2], 4 );
  sprintf(weekLabel, "%d %s - %d %s %d",startDate[2],months[startDate[1]],endDate[2],months[endDate[1]], endDate[0] );
  gtk_label_set_text(week, weekLabel);

  free(endDate);
  endDate = NULL;
}


/*
-----------------------------------------------------------------------------------------------------------
Function: setRoomInfo
-------------------------
Retrive data (room name, place name, room price half day) from database.
Edit label.
Edit equipment.
Function call when user open planning window.
-------------------------
Calendar *calendar: address of the struct calendar.
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function: updateButtonsPlanning
-------------------------
Update the buttons in the planning. check if a button is clickable or not by
calling isTimeSlotAvailable and by checking the date.
-------------------------
Calendar *calendar: address of the struct calendar.
struct tm *td : current day
*/
void updateButtonsPlanning( Calendar *calendar, struct tm *td){
  char timeSlots[2][12] = {"8h - 14h", "14h - 20h"};
  char date[16];
  char idRoom[4];
  int i, j;
  int isAvailable, isFuture = 1;
  Date planning = calendar->planning;
  GtkWidget *button;

  sprintf(idRoom, "%d", calendar->id_room);
  for(i = 0; i < 2; i++){
    for(j = 0; j < 5; j++){
      button = GTK_WIDGET(calendar->buttonsBooking[i][j]);
      background_color(button, "#ffffff");

      int *curDate = moveInCalendar(planning.year, planning.month, planning.day, j);
      sprintf(date, "%d-%d-%d", curDate[0], curDate[1]+1, curDate[2]);
      isAvailable = isTimeSlotAvailable(timeSlots[i], date, idRoom);

      if( curDate[0] >= td->tm_year + 1900 )
        if( curDate[1] >= td->tm_mon )
          if( curDate[2] > td->tm_mday ) isFuture = 1;
          else isFuture = 0;
        else isFuture = 0;
      else isFuture = 0;

      isAvailable = isAvailable == 1 && isFuture == 1 ? 1 : -1;

      showOrHidePlanningButton(isAvailable, button);
      free(curDate);
      curDate = NULL;
    }
  }
}


/*
-----------------------------------------------------------------------------------------------------------
Function: showOrHidePlanningButton
-------------------------
It allows to make visible or not the button according to the availability of the time slot.
-------------------------
int isAvailable: state of timeslot avaible or not. (1 or -1)
GtkWidget *button: The button to change (hide or show).
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function: updateTimeSlotLabels
-------------------------
Update the time slot which confirm which time slot is selected
-------------------------
Calendar *calendar: address of the struct calendar.
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function: setDateReservation
-------------------------
Set the new date.
-------------------------
char *dateSQL: date in format SQL yyyy-[1-12]-[1-31]
GtkLabel *label: The current date display on the calendar.
*/
void setDateReservation(char *dateSQL, GtkLabel *label){
  char months[12][8] = {"janv", "fev", "mars", "avr", "mai", "juin",\
                        "juill", "aout", "sept", "oct", "nov", "dec"};
  Date d;
  char date[32];

  sscanf(dateSQL, "%d-%d-%d", &d.year, &d.month, &d.day);
  sprintf(date, "%d %s %d", d.day, months[d.month - 1], d.year);

  gtk_label_set_text(label, date);
}


/*
-----------------------------------------------------------------------------------------------------------
Function: focusDateCalendar
-------------------------
In new res window, set the selected date to tomorow
-------------------------
Calendar *calendar: address of the struct calendar.
*/
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
  tomorrow = NULL;
}


/*
-----------------------------------------------------------------------------------------------------------
Function: checkDataInputPlace
-------------------------
Check if a place is selected in the new res window.
-------------------------
GtkComboBox *place : combobox of the place selected
-------------------------
Return value
  uint8_t : boolean if the place is ok
*/
uint8_t checkDataInputPlace(GtkComboBox *place){
    uint32_t id;

    if( gtk_combo_box_get_active_id(place) != NULL ){
      id = (uint32_t)atoi( gtk_combo_box_get_active_id(place) );
      return  id > 0;
    }
  return 0;
}


/*
-----------------------------------------------------------------------------------------------------------
Function: checkDataCalendar
-------------------------
Check if the date selected in the calendar of new res window is not passed.
-------------------------
GtkCalendar *calendar : calendar in new res window
-------------------------
Return value
  uint8_t : boolean if the date is ok
*/
uint8_t checkDataCalendar(GtkCalendar *calendar){
  Date d;
  time_t now;
  struct tm *td;

  gtk_calendar_get_date ( calendar, (guint *)&d.year, (guint *)&d.month, (guint *)&d.day);
  time( &now );
  td = localtime( &now );

  if( d.year >= td->tm_year + 1900 )
    if( d.month >= td->tm_mon )
      if( d.day > td->tm_mday ) return 1;
      else return 0;
    else return 0;
  else return 0;
}


/*
-----------------------------------------------------------------------------------------------------------
Function: checkDataNewRes
-------------------------
Hide or show the date avaible for new reservation.
-------------------------
GtkWidget *widget: widget activated.
*/
void checkDataNewRes(GtkWidget *widget, gpointer data){
  GtkWidget **check = data;
  /*printf("place: %u\n",checkDataInputPlace( GTK_COMBO_BOX(check[1]) ) );
  printf("calendar: %u\n",checkDataCalendar( GTK_CALENDAR(check[2] )));*/

  if( checkDataInputPlace( GTK_COMBO_BOX(check[1]) ) && checkDataCalendar( GTK_CALENDAR(check[2] )) )
    gtk_widget_set_sensitive( GTK_WIDGET(check[0]), TRUE );
  else
    gtk_widget_set_sensitive( GTK_WIDGET(check[0]), FALSE );
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






//
