/*
import_glade.c

create the windows from the file glade in ui/glade/home.glade
connet the signals for the navigation between the windows

*/

#include <time.h>
#include <gtk/gtk.h>
#include "../inc/navigation.h"
#include "../inc/insertDataGtk.h"


//defines
#define GLADE_FILE "ui/glade/home.glade"


// ----------------------
/*
function: homekamaji
Open Window_home
*/
void open_home_window(GtkWidget *widget, gpointer data){
  Session *session = data;

  newWindow(GLADE_FILE, "window_home", session);

  session->backFunction = open_home_window;

  click_button(session, "button_home_reservations", open_reservations_window);
  click_button(session, "button_home_search", open_new_res_window);
  click_button(session, "button_home_calendars", open_place_room_window);

  freeDelReservations(&session->nextReservation);
}

// ----------------------
/*
click_button
*/
void click_button(Session *session, char *idButton, void (*function)){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(session->builder, idButton));
  g_signal_connect (button,"clicked",G_CALLBACK(function),session);
}

// ----------------------
/*
close_and_open_window
*/
void close_and_open_window(Session *session, char *idNewWindow){
  gtk_widget_destroy( GTK_WIDGET( session->window ) );
  newWindow(GLADE_FILE, idNewWindow, session);
}

// ----------------------
/*
newWindow
*/
void newWindow(char* file, char* idWindow, Session *session){
  GtkBuilder      *builder;
  GtkWidget       *window;

  builder = gtk_builder_new_from_file(file);
  window = GTK_WIDGET(gtk_builder_get_object(builder, idWindow));
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(kamajiQuit), session);

  session->builder = builder;
  session->window = GTK_WINDOW(window);

  //Background color
  background_color(window, "#444444" );

  gtk_widget_show_all(window);
}

// ----------------------
void background_color( GtkWidget *widget, char *color ){
  GtkCssProvider * cssProvider = gtk_css_provider_new();    //store the css

  char css[64] = "* { background-image:none; background-color:";
  strcat( strcat( css , color ), ";}" );

  gtk_css_provider_load_from_data(cssProvider, css,-1,NULL);
  GtkStyleContext * context = gtk_widget_get_style_context(widget);   //manage CSS provider
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}

//##############################################################################
// ----------------------
// RESERVATIONS

void open_reservations_window(GtkWidget *widget,gpointer data){
  Session *session = data;
  GtkGrid *gridContainer;
  GtkButton *backButton;
  ReservationBox reservation;
  delReservation *delReservation;
  MYSQL_ROW row;
  MysqlSelect selectReservations;
  int i = 0;

  session->backFunction = open_home_window;
  close_and_open_window(session,"window_reservations");
  gridContainer = GTK_GRID( gtk_builder_get_object(session->builder, "grid_reservations") );
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_reservations") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back),session);
  selectReservations = findReservationsInDB();

  while ((row = mysql_fetch_row(selectReservations.result)) != NULL){
    reservation = newReservation();
    displayReservationData(reservation, row);
      delReservation = addDelReservation(session, (uint32_t)atoi(row[0]));
      session->nextReservation = delReservation;

    g_signal_connect (reservation.delete,"clicked",G_CALLBACK(deleteReservation), delReservation);

    gtk_grid_attach (gridContainer, GTK_WIDGET(reservation.box),i%2, i/2, 1, 1);
    i++;
  }

  mysql_free_result(selectReservations.result);
  mysql_close(selectReservations.conn);
}

delReservation *addDelReservation(Session *session, uint32_t id){
  delReservation *inter = malloc( sizeof(delReservation) );
  if( inter == NULL ) exit(1);
  inter->idBooking = id;
  inter->session = session;
  inter->next = session->nextReservation;
  return inter;
}

void deleteReservation(GtkWidget *widget,gpointer data){
  delReservation *delReservation = data;
  GtkWidget *window;
  GtkBuilder *builder;
  GtkButton *no;
  GtkButton *yes;

  builder = gtk_builder_new_from_file(GLADE_FILE);
  window = GTK_WIDGET(gtk_builder_get_object(builder, "dialog_delete_reservation"));
  gtk_builder_connect_signals(builder, NULL);

  no = GTK_BUTTON(gtk_builder_get_object(builder, "button_delete_reservation_right"));
  yes = GTK_BUTTON(gtk_builder_get_object(builder, "button_delete_reservation_left"));

  delReservation->dialogWindow = window;
  g_signal_connect(no, "clicked", G_CALLBACK(abordDeleteReservation), window);
  g_signal_connect(yes, "clicked", G_CALLBACK(confirmDeleteReservation), delReservation);

  gtk_widget_show_all(window);
}

void abordDeleteReservation(GtkWidget *widget, gpointer data){
  GtkWidget *window = data;
  gtk_widget_destroy(window);
}

// ----------------------

MysqlSelect findReservationsInDB(){
  MysqlSelect select;

  strcpy(select.request, "SELECT B.id, R.id, R.name, P.name, B.nb_persons, B.price, B.date_booking, B.time_slot \
  FROM BOOKING as B\
  INNER JOIN ROOM as R ON B.room = R.id\
  INNER JOIN PLACE as P ON R.place = P.id\
  WHERE B.state = 1 AND R.state = 1 AND P.state = 1");

  select.conn = connect_db();
  select.result = query(select.conn, select.request);

  return select;
}

// ----------------------
// NEW RESERVATIONS
/*
function: open_new_res_window
Open window_new_reservation
*/
void open_new_res_window(GtkWidget *widget, gpointer data){
  Session *session = data;
  GtkComboBox *inputplace;
  GtkButton *backButton, *nextButton;
  GtkCalendar *calendar;
  GtkWidget **check;

  close_and_open_window(session, "window_new_reservation");

  session->backFunction = open_home_window;
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_new_res") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  nextButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_new_res") );
  gtk_widget_set_sensitive(GTK_WIDGET(nextButton), FALSE);

  inputplace = GTK_COMBO_BOX(gtk_builder_get_object(session->builder, "combo_new_reservation_where"));
  comboBoxTextFill( GTK_COMBO_BOX_TEXT(inputplace) ,"Choisir un lieu", "SELECT id, name FROM PLACE WHERE state = 1" );

  calendar = GTK_CALENDAR(gtk_builder_get_object(session->builder, "calendar_new_res") );
  focusDateCalendar(calendar);

  //check data
  if( (check = malloc( sizeof(GtkWidget*) * 3 )) == NULL) exit(1);
  check[0] = GTK_WIDGET(nextButton);
  check[1] = GTK_WIDGET(inputplace);
  check[2] = GTK_WIDGET(calendar);
  g_signal_connect(inputplace, "changed", G_CALLBACK(checkDataNewRes), check);
  g_signal_connect(calendar, "day-selected", G_CALLBACK(checkDataNewRes), check);

  click_button(session, "button_new_res", getSearchArguments);
}

// ----------------------

void getSearchArguments(GtkWidget *widget,gpointer data){
  Session *session = data;
  Search *search = session->search;

  GtkComboBox *inputplace;
  GtkComboBox *inputTime;
  GtkSpinButton *inputNbPeoples;
  GtkCalendar *inputDate;

  inputplace = GTK_COMBO_BOX(gtk_builder_get_object(session->builder, "combo_new_reservation_where"));
  inputNbPeoples = GTK_SPIN_BUTTON(gtk_builder_get_object(session->builder, "spin_new_reservation_group"));
  inputTime= GTK_COMBO_BOX(gtk_builder_get_object(session->builder, "combo_new_reservation_when"));
  inputDate = GTK_CALENDAR(gtk_builder_get_object(session->builder, "calendar_new_res"));

  search->id_place = atoi( gtk_combo_box_get_active_id( GTK_COMBO_BOX(inputplace) ) );
  search->nb_persons = gtk_spin_button_get_value_as_int(inputNbPeoples) ;
  search->time_slot = atoi( gtk_combo_box_get_active_id( GTK_COMBO_BOX(inputTime) ) );
  gtk_calendar_get_date(inputDate, (guint*)&search->date.year, (guint*)&search->date.month, (guint*)&search->date.day);
  search->date.month++;

  open_equipment_window(NULL, session);
}


//##############################################################################
// ----------------------
// EQUIPMENTS
void open_equipment_window(GtkWidget *widget,gpointer data){
  Session* session = data;
  GtkButton *backButton;

  session->backFunction = open_new_res_window;
  close_and_open_window(session, "window_equipment");
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_equiments") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  click_button(session, "button_equipment_search", getEquipmentsCheckbox);
}

// ----------------------
void getEquipmentsCheckbox(GtkWidget *widget,gpointer data){
  Session *session = data;
  int equipments[4] = {0};

  GtkCheckButton *checkMonitor = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_equipment_monitor"));
  GtkCheckButton *checkWhiteboard = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_equipment_whiteboard"));
  GtkCheckButton *checkCamera = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_equipment_camera"));
  GtkCheckButton *checkProjector = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_equipment_projector"));

  equipments[0] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkMonitor) );
  equipments[1] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkWhiteboard) );
  equipments[2] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkCamera) );
  equipments[3] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkProjector) );

  for(int i = 0; i < 4; i++)
    session->search->equipments[i] = equipments[i];

  open_drink_window(NULL, session);
}

// ----------------------
// DRINK
void open_drink_window(GtkWidget *widget,gpointer data){
  Session *session = data;
  GtkButton *backButton;

  session->backFunction = open_equipment_window;
  close_and_open_window(session, "window_drink");
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_drink") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  click_button(session, "button_drink_next", getSearchDrinksCheckbox);
}
// ----------------------

void getSearchDrinksCheckbox(GtkWidget *widget,gpointer data){
  Session *session = data;
  Search *search = session->search;
  int drinks[2] = {0};

  GtkCheckButton *checkCoffee = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_drink_coffee"));
  GtkCheckButton *checkTea = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_drink_tea"));

  drinks[0] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkCoffee) );
  drinks[1] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkTea) );

  for(int i = 0; i < 2; i++)
    session->search->drinks[i] = drinks[i];

  open_rooms_available_window(NULL ,session);
}


//##############################################################################
// ----------------------
// ROOMS AVAILABLES

void open_rooms_available_window(GtkWidget *widget,gpointer data){
  Session *session = data;
  Search *search = session->search;
  RoomGtkBox room;
  GtkContainer *listContainer;
  MYSQL_ROW row;
  MysqlSelect select;
  GtkButton *backButton;

  session->backFunction = open_drink_window;
  close_and_open_window(session,"window_rooms_available");
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_rooms_available") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);
  listContainer = GTK_CONTAINER( gtk_builder_get_object(session->builder, "box_available_list") );

  GtkWidget *viewport_available_right = GTK_WIDGET(gtk_builder_get_object(session->builder,"viewport_available_right"));
  background_color(viewport_available_right , "#FFFFFF");

  select = findAvailableRooms(search);
  while ((row = mysql_fetch_row(select.result)) != NULL){
    if( !hasRequiredEquipments(search->equipments, row[0]) ) continue;

    room = newRoomAvailable(row);
    displayDataRoom(room, row, session);
    gtk_container_add ( listContainer, GTK_WIDGET(room.box) );
  }

  mysql_free_result(select.result);
  mysql_close(select.conn);
}

void displayDataRoom(RoomGtkBox room, MYSQL_ROW row, Session *session){
  Search *search = session->search;
  Booking *booking;

  displayRoomEquipments(room.equipments, row[0]);
  displayTimeSlotComboBox(room, row[0], search);
  displayTimeSlotLabel(room.timeSlotLabel, row[0], search->date, search->time_slot );

  booking = prepareBooking(search, room, row[0]);
  booking->session = session;
  search->startBooking = booking;

  g_signal_connect (room.bookingButton,"clicked",G_CALLBACK(reserveRoomBySearch),booking);
}

uint8_t hasRequiredEquipments(int requiredEquipments[4], char *idRoom){
  int *roomEquipments = getRoomsEquipment(idRoom);

  for(int i = 0; i < 4; i++)
    if( requiredEquipments[i] == 1 && roomEquipments[i] == 0)
      return 0;

  free(roomEquipments);
  roomEquipments = NULL;
  return 1;
}
// ----------------------

Booking *prepareBooking( Search *search, RoomGtkBox room, char *idRoom ){
  Booking *booking;
  GtkComboBox *timeSlotComboBox;
  int priceHalfDay;
  double price = 0;
  int drinks[2];

  booking = malloc( sizeof(Booking) );
  if( booking == NULL ) exit(1);

  strcpy(booking->test, "test");
  booking->idRoom = atoi( idRoom );
  booking->nb_persons = search->nb_persons;
  booking->date = search->date;
  for( int i = 0; i < 2; i++ )
    booking->drinks[i] = search->drinks[i];

  // get the time slot from the comboBoxText
  timeSlotComboBox = GTK_COMBO_BOX( gtk_builder_get_object(room.builder, "combo_available_list_element_when") );
  booking->time_slot = atoi( gtk_combo_box_get_active_id(timeSlotComboBox) );

  for(int i = 0; i < 2; i++) drinks[i] = search->drinks[i];
  price += getPriceDrinks(drinks, search->id_place);

  priceHalfDay = atoi( gtk_label_get_text(room.priceHalfDay) );
  price += search->time_slot == 2 ? priceHalfDay * 2 : priceHalfDay ;
  booking->price = price;

  booking->next = search->startBooking;

  return booking;
}

//

int getPriceDrinks(int drinks[2], int idPlace){
  int price = 0;
  char request[512];
  MYSQL *conn = connect_db();
  MYSQL_ROW row;

  for(int i = 0; i < 2; i++){
    if( drinks[i] == 1 ){
      MYSQL_RES *result;
      sprintf(request, "SELECT ppd.price FROM ROOM as R\
      INNER JOIN PLACE as P on R.place = P.id\
      INNER JOIN _place_propose_drink as ppd on ppd.place = P.id\
      WHERE ppd.place = %d AND ppd.drink = %d\
      GROUP BY P.id", idPlace, drinks[i]);
      result = query(conn, request);
      if((row = mysql_fetch_row(result)) != NULL)
        price += atoi(*row);

      mysql_free_result(result);
    }
  }
  mysql_close(conn);

  return price;
}

// ----------------------

void reserveRoomBySearch(GtkWidget *widget, gpointer data){
  Booking *b = data;
  Session *session = b->session;
  MYSQL *conn = connect_db();
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char request[512];

  sprintf(request, "INSERT INTO BOOKING(nb_persons,price,date_booking,time_slot,state,room) \
  VALUES(%d,%d,'%d-%d-%d','%s',1,%d) ;",\
  b->nb_persons, (int)b->price, b->date.year, b->date.month, b->date.day, time_slots[b->time_slot], b->idRoom );

  //add in _booking_include_drink

  query(conn, request);
  mysql_close(conn);

  freeBookings(&session->search->startBooking);

  open_reservations_window(NULL, session);
}


void freeBookings(Booking **start){
  while(*start != NULL){
    Booking *remove = *start;
    *start = (*start)->next;
    free(remove);
  }
}

// ----------------------

MysqlSelect findAvailableRooms(Search *s){
  Date d = s->date;
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char request[1024];
  MysqlSelect select;

  /*
  select ROOM.id , ROOM.name, PLACE.name, ROOM.max_capacity and ROOM.price_half_day
  for the rooms which are available at a date and at a time slot.
  */
  sprintf(request, "SELECT A.* FROM (\n\
  	SELECT R.id, R.name as room_name, P.name as place_name, R.max_capacity, R.price_half_day\n\
      FROM ROOM as R\n\
      INNER JOIN PLACE as P ON R.place = P.id\n\
      WHERE R.max_capacity >= %d\n\
      AND R.place = %d\n\
      AND R.state = 1\n\
  ) as A\n\
  LEFT JOIN (\n\
      SELECT B.room FROM BOOKING AS B\n\
      INNER JOIN ROOM as R ON B.room = R.id\n\
      WHERE R.place = %d\n\
      AND B.date_booking = '%d-%d-%d'\n\
      AND ( B.time_slot = '8h - 20h' OR B.time_slot = '%s')\n\
      AND B.state = 1\n\
  ) as B\n\
  ON A.id = B.room\n\
  WHERE B.room IS NULL;\n"\
  , s->nb_persons, s->id_place, s->id_place, d.year, d.month, d.day, time_slots[s->time_slot] );

  //query the reservations
  MYSQL *conn = connect_db();
  MYSQL_RES *result = query(conn, request);

  select.conn = conn;
  select.result = result;
  strcpy(select.request, request);

  return select;
}

// ----------------------

int isRestDayAvailable( Date date, int time_slot_int, char *idRoom ){
  char dateString[16];
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char time_slot[16] = "";

  sprintf( dateString, "%d-%d-%d", date.year, date.month, date.day );
  strcpy(time_slot, time_slots[ 1^time_slot_int ] ); // 1 XOR time_slot -> 1^0 = 1 and 1^1 = 0

  return isTimeSlotAvailable(time_slot, dateString, idRoom);
}

int isTimeSlotAvailable(char *time_slot, char *date, char *idRoom){
  int isAvailable;
  MYSQL *conn = connect_db();
  MYSQL_RES *result;
  MYSQL_ROW row;
  char request[512];

  // return "1" if a room is available at a date and a time slot, "0" if not
  sprintf(request, "SELECT IF(\
  	(SELECT COUNT(time_slot) FROM BOOKING\
  	WHERE room = %s\
  	AND date_booking = '%s'\
  	AND time_slot = '%s'\
  	AND state = 1) > 0, -1, 1\
  ) AS Q", idRoom, date, time_slot );

  result = query(conn, request);

  if( (row = mysql_fetch_row(result)) != NULL )
    isAvailable = atoi(*row);
  else{
    printf("Error mysql: fetch == NULL in isTimeSlotAvailable\n");
    exit(0);
  }

  mysql_free_result(result);
  mysql_close(conn);
  return isAvailable;
}

//##############################################################################
// ----------------------
//PLACE ROOM

void open_place_room_window(GtkWidget *widget,gpointer data){
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

// ----------------------

void getIdRoom(GtkWidget *widget, gpointer data){
  Session *session = data;
  GtkComboBox *place = GTK_COMBO_BOX( gtk_builder_get_object( session->builder, "combo_place_room_place" ) );
  GtkComboBox *room = GTK_COMBO_BOX( gtk_builder_get_object( session->builder, "combo_place_room_room" ) );
  Calendar *calendar = session->calendar;

  calendar->id_room = atoi( gtk_combo_box_get_active_id(room) );
  calendar->id_place = atoi( gtk_combo_box_get_active_id(place) );

  open_planning_window(NULL, session);
}

// ----------------------
//PLANNING

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

// ----------------------

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

// ----------------------

void chooseTimeSlot(GtkWidget *widget, gpointer data){
  Calendar *calendar = data;
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

// ----------------------

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

// ----------------------

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

// ----------------------

void background_color_if_sensitive(GtkWidget *widget, char* color){
  if( (int)gtk_widget_is_sensitive (widget) )
    background_color( widget, color );
}

// ----------------------
// DRINKS 2

void open_drink_window_2(GtkWidget *Widget,gpointer data){
  Session *session = data;
  Calendar *c = session->calendar;
  GtkButton *backButton;

  session->backFunction = open_planning_window;
  close_and_open_window(session, "window_drink");
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_drink") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  click_button(session, "button_drink_next", getPlanningDrinksCheckbox);
}

// ----------------------

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

  //
  booking = prepareBookingPlanning(*calendar);
  reserveRoomByPlanning(booking);


  open_reservations_window(NULL, s);
}

// ----------------------

Booking *prepareBookingPlanning(Calendar c){
  Booking *booking;
  int priceHalfDay;
  double price = 0;
  int drinks[2];

  booking = malloc( sizeof(Booking) );
  if( booking == NULL ) exit(1);

  booking->idRoom = c.id_room;
  booking->nb_persons = 1;        // get the nb of persons ???
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

void reserveRoomByPlanning(Booking *b){
  Session *session = b->session;
  MYSQL *conn = connect_db();
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char request[512];

  sprintf(request, "INSERT INTO BOOKING(nb_persons,price,date_booking,time_slot,state,room) \
  VALUES(%d,%d,'%d-%d-%d','%s',1,%d) ;",\
  b->nb_persons, (int)b->price, b->date.year, b->date.month, b->date.day, time_slots[b->time_slot], b->idRoom );

  query(conn, request);
  mysql_close(conn);
  free(b);
  b = NULL;
}


// ----------------------

void printSearchParameter(Search *search){
  int eq[4];
  int dr[2];
  int i;

  for(i = 0; i < 4; i++) eq[i] = search->equipments[i];
  for(i = 0; i < 2; i++) dr[i] = search->drinks[i];

  printf("\nLieu: %d\nNb personnes: %d\nCreneau: %d\nDate: %d-%d-%d\n",search->id_place,search->nb_persons, search->time_slot, search->date.year, search->date.month, search->date.day  );
  printf("Ecran: %d\nWhiteBoard: %d\nCamera: %d\nProjecteur: %d\n",eq[0],eq[1],eq[2],eq[3] );
  printf("Caffe: %d\nThe: %d\n",dr[0],dr[1]);
}

// ----------------------

// STYLE
void stylePlanningRoom(Session *session){
  GtkWidget *planningContainer;
  GtkWidget *locationContainer;

  planningContainer = GTK_WIDGET( gtk_builder_get_object(session->builder, "box_planning_planning") );
  locationContainer = GTK_WIDGET( gtk_builder_get_object(session->builder, "box_planning_where") );

  background_color(planningContainer, "#ffffff" );
  background_color(locationContainer, "#ffffff" );
}

// ----------------------


// ----------------------





















//#############
