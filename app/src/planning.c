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
