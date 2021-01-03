/*
insertDataGtk.c

Call the data in the database and inject them in the windows.

*/
#include "kamaji.h"


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

//RESERVATIONS



//PLANNING
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





//PLACE ROOM
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
