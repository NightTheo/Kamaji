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
******************************
*       PLANNING WINDOW      *
******************************
*/

/*
-----------------------------------------------------------------------------------------------------------
Function : open_planning_window
-------------------------
Open the planning window.
-------------------------
GtkWidget *widget: widget activated to open the window.
Session *session : address of the struct Session
*/
void open_planning_window(GtkWidget *widget,gpointer data){
  Session *session = data;
  GtkButton *backButton;

  close_and_open_window(session,"window_planning");

  session->backFunction = open_place_room_window;
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_planning") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  getCalendarWidgets(session->calendar, session->builder);
  planningNumbers(session->calendar, session->today);
  setRoomInfo(session->calendar);

  updateButtonsPlanning(session->calendar, session->today);

  click_button_planning(session, "button_planning_weeks_next");  // NEXT
  click_button_planning(session, "button_planning_weeks_back"); // PREV

  stylePlanningRoom(session);
  click_button(session, "button_planning_next", open_drink_window_2);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getCalendarWidgets
-------------------------
Get all the widgets wich will be updated and put them in the Calendar struct
-------------------------
 Calendar *c : address of the struct Calendar
 GtkBuilder *builder : builder wich built the planning window
*/
void getCalendarWidgets(Calendar *c, GtkBuilder *builder){
  int i, j;
  char id[32];
  char time[2][16] = {"morning", "afternoon"};
  char equipments[4][16] = {"whiteboard", "monitor", "projector", "camera"};

  c->week = GTK_LABEL( gtk_builder_get_object(builder, "lbl_planning_weeks") );
  for(i = 0; i < 5; i++){ //days
    sprintf(id, "lbl_planning_nb_%d", i);
    c->days[i] = GTK_LABEL( gtk_builder_get_object(builder, id) );
  }
  for(i = 0; i < 2; i++) //buttonsBooking
    for(j = 0; j < 5; j++){
      sprintf(id, "button_planning_%s_%d",time[i], j);
      c->buttonsBooking[i][j] = GTK_BUTTON( gtk_builder_get_object(builder, id) );
      g_signal_connect(c->buttonsBooking[i][j], "clicked", G_CALLBACK(chooseTimeSlot), c );
    }
  c->room = GTK_LABEL( gtk_builder_get_object(builder, "lbl_planning_room") );
  c->place = GTK_LABEL( gtk_builder_get_object(builder, "lbl_planning_place") );
  c->dateLabel = GTK_LABEL( gtk_builder_get_object(builder, "lbl_planning_infos_date") );
  c->timeSlotLabel = GTK_LABEL( gtk_builder_get_object(builder, "lbl_planning_infos_time_slot") );
  c->price = GTK_LABEL( gtk_builder_get_object(builder, "lbl_planning_infos_price") );
  for(i = 0; i < 4; i++){ // equipments
    sprintf(id, "img_planning_%s", equipments[i] );
    c->equipments[i] = GTK_IMAGE( gtk_builder_get_object(builder, id) );
  }
  c->timeSlotCombo = GTK_COMBO_BOX_TEXT( gtk_builder_get_object(builder, "combo_planning_when") );
  g_signal_connect(c->timeSlotCombo, "changed", G_CALLBACK(onTimeSlotPlanningChanged), c);
  c->next = GTK_BUTTON( gtk_builder_get_object(builder, "button_planning_next") );
}


/*
-----------------------------------------------------------------------------------------------------------
Function : chooseTimeSlot
-------------------------
Called when the user choose a time slot in Calendar.
Update the daySelected in Calendar.
call the functions updateTimeSlotComboPlanning and updateTimeSlotLabels
-------------------------
GtkWidget *widget: widget activated, here one of the calendar button
Calendar *calendar : address of the struct Calendar
*/
void chooseTimeSlot(GtkWidget *widget, Calendar *calendar){
  GtkButton *button = GTK_BUTTON( widget );
  int *selected;
  int timeSlot, day;
  int i, j;

  for(i = 0; i < 2; i++)
    for(j = 0; j < 5; j++){
      background_color( GTK_WIDGET(calendar->buttonsBooking[i][j]), "#ffffff" );
      if( button == calendar->buttonsBooking[i][j]){
        timeSlot = i;
        day = j;
      }
    }

  background_color( widget, "#444444" );

  selected = moveInCalendar(calendar->planning.year, calendar->planning.month, calendar->planning.day, day);
  calendar->daySelected.year = selected[0];
  calendar->daySelected.month = selected[1] + 1;
  calendar->daySelected.day = selected[2];
  calendar->timeSlotSelected = timeSlot;
  calendar->wDaySelected = day;

  updateTimeSlotComboPlanning(calendar);
  gtk_widget_set_opacity( GTK_WIDGET(calendar->next), 1 );
  gtk_widget_set_sensitive( GTK_WIDGET(calendar->next), TRUE );

  updateTimeSlotLabels(calendar);

  free(selected);
  selected = NULL;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : updateTimeSlotComboPlanning
-------------------------
Called when a time slot is selected, check if the rest of the day is available, if yes
the user can choose in the booking ComboBox between the morning, the afternoon or the all day. Else only the time time slot
selected is proposed in the ComboBox
-------------------------
 Calendar *c : address of the struct Calendar
*/
void updateTimeSlotComboPlanning(Calendar *calendar){
  char timeSlots[3][16] = {"Matin", "Après-midi", "Journée"};
  char id[3][4] = {"0", "1", "2"};
  char idRoom[4];
  int i;
  int timeSlot= calendar->timeSlotSelected;

  gtk_combo_box_text_remove_all (calendar->timeSlotCombo);

  sprintf(idRoom, "%d", calendar->id_room);
  if( isRestDayAvailable( calendar->daySelected, timeSlot, idRoom) == 1 ){
    for(i = 0; i < 3; i++)
      gtk_combo_box_text_append(calendar->timeSlotCombo,id[i], timeSlots[i]);
    gtk_combo_box_set_active_id (GTK_COMBO_BOX(calendar->timeSlotCombo), id[timeSlot]);
  }else{
    gtk_combo_box_text_append(calendar->timeSlotCombo,id[timeSlot], timeSlots[timeSlot]);
    gtk_combo_box_set_active_id (GTK_COMBO_BOX(calendar->timeSlotCombo), id[timeSlot]);
  }

  gtk_widget_set_opacity( GTK_WIDGET(calendar->timeSlotCombo), 1 );
  gtk_widget_set_sensitive( GTK_WIDGET(calendar->timeSlotCombo), TRUE );

}


/*
-----------------------------------------------------------------------------------------------------------
Function : onTimeSlotPlanningChanged
-------------------------
Called when the user select a time slot in the ComboBox. Update the button color of the time slot selected at this day.
If the time selected is all the day, the color of two buttons of the day is changed;
update the field timeSlotSelected in the Calendar struct.
-------------------------
GtkWidget *widget: widget activated to open the window, here the calendar button
Calendar *c : address of the struct Calendar
*/
void onTimeSlotPlanningChanged(GtkWidget *widget, gpointer data){
  Calendar *c = data;
  GtkComboBox *combo = GTK_COMBO_BOX(widget);
  int id;

  if( gtk_combo_box_get_active_id(combo) != NULL){
    id = atoi(gtk_combo_box_get_active_id(combo));
    c->timeSlotSelected = id;
    if( id < 2){
      background_color_if_sensitive( GTK_WIDGET(c->buttonsBooking[id][c->wDaySelected]), "#444444" );
      background_color_if_sensitive( GTK_WIDGET(c->buttonsBooking[1^id][c->wDaySelected]), "#ffffff" );
    }
    else
      for(int i = 0; i < 2; i++)
        background_color_if_sensitive( GTK_WIDGET(c->buttonsBooking[i][c->wDaySelected]), "#444444" );

  }

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
Update the time slot which confirm which time slot is selected in planning window
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
******************************
*       DRINK 2 WINDOW       *
******************************
*/

/*
-----------------------------------------------------------------------------------------------------------
Function : open_drink_window_2
-------------------------
Open the window of the choice of having a drink or not, after planning window.
-------------------------
GtkWidget *widget: widget activated to open the window, here the button_planning_next button in planning window
Session *session : address of the struct Session
*/
void open_drink_window_2(GtkWidget *Widget, gpointer data){
  Session *session = data;
  Calendar *c = session->calendar;
  GtkButton *backButton;

  session->backFunction = open_planning_window;
  close_and_open_window(session, "window_drink");
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_drink") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  click_button(session, "button_drink_next", getPlanningDrinksCheckbox);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getPlanningDrinksCheckbox
-------------------------
Get the selected drinks after the planning window, and put them in the struct Calendar. Then open the reservations window.
-------------------------
GtkWidget *widget: widget activated to open the window, here the next button in drinks window 2
Session *s : address of the session
*/
void getPlanningDrinksCheckbox(GtkWidget *widget, gpointer data){
  Session *s = data;
  Calendar *calendar = s->calendar;
  Booking *booking;
  int drinks[2] = {0};

  GtkCheckButton *checkCoffee = GTK_CHECK_BUTTON(gtk_builder_get_object(s->builder, "check_drink_coffee"));
  GtkCheckButton *checkTea = GTK_CHECK_BUTTON(gtk_builder_get_object(s->builder, "check_drink_tea"));

  drinks[0] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkCoffee) );
  drinks[1] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkTea ) );

  for(int i = 0; i < 2; i++)
    s->calendar->drinks[i] = drinks[i];

  booking = prepareBookingPlanning(*calendar);
  reserveRoomByPlanning(booking);


  open_reservations_window(NULL, s);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : prepareBookingPlanning
-------------------------
Create a Booking with all the informations to insert in DB
-------------------------
Calendar c: calendar
-------------------------
Return value
  Booking *booking : address of the Booking created
*/
Booking *prepareBookingPlanning(Calendar c){
  Booking *booking;
  int priceHalfDay;
  double price = 0;
  int drinks[2];

  booking = malloc( sizeof(Booking) );
  if( booking == NULL ) exit(1);

  booking->idRoom = c.id_room;
  booking->nb_persons = 1;
  booking->time_slot = c.timeSlotSelected;
  booking->date = c.daySelected;
  for(int i = 0; i < 2; i++) booking->drinks[i] = c.drinks[i];

  priceHalfDay = getPriceRoom(c.id_room);
  price += c.timeSlotSelected < 2 ? priceHalfDay : 2 * priceHalfDay;

  for(int i = 0; i < 2; i++) drinks[i] = c.drinks[i];
  price += getPriceDrinks(drinks, c.id_place);

  booking->price = price;

  return booking;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getPriceRoom
-------------------------
The function retrieve the price half day of a room from database.
-------------------------
int idRoom: the id of the room selected.
*/
unsigned int getPriceRoom(int idRoom){
  MYSQL *conn = connect_db();
  MYSQL_RES *result;
  MYSQL_ROW row;
  char request[512];
  unsigned int price = 0;

  sprintf(request, "select price_half_day from ROOM where id = %d", idRoom);
  result = query(conn, request);
  if((row = mysql_fetch_row(result)) != NULL)
    price = atoi(*row);

  mysql_free_result(result);
  mysql_close(conn);

  return price;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : reserveRoomByPlanning
-------------------------
When the user click on "reserver" , the function reserveRoomPlanning insert the new reservation of
the user in the table BOOKING of database.
-------------------------
Booking *b : address of the struct Booking
*/
void reserveRoomByPlanning(Booking *b){
  Session *session = b->session;
  MYSQL *conn = connect_db();
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char request[512];

  sprintf(request, "INSERT INTO BOOKING(nb_persons,price,date_booking,time_slot,state,room) \
  VALUES(%d,%d,'%d-%d-%d','%s',1,%d) ;",\
  b->nb_persons, (int)b->price, b->date.year, b->date.month, b->date.day, time_slots[b->time_slot], b->idRoom );

  query(conn, request);
  insertDrinks(b->drinks, conn );
  mysql_close(conn);
  free(b);
  b = NULL;
}
