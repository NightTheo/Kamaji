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
void open_home_window(char *idWindow){
  time_t now;
  Session *session;

  session = malloc(sizeof(Session));
  if( session == NULL ) exit(1);

  newWindow(GLADE_FILE, idWindow, session);
  time( &now );
  session->today = localtime( &now ); // get the current date as tm struct

  click_button(session, "button_home_reservations", open_reservations_window);
  click_button(session, "button_home_search", open_new_res_window);
  click_button(session, "button_home_calendars", open_place_room_window);

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
  gtk_builder_connect_signals(builder, NULL);
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(gtk_main_quit), NULL);

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
  close_and_open_window(session,"window_reservations");
}

// ----------------------
/*
function: open_new_res_window
Open window_new_reservation
*/
void open_new_res_window(GtkWidget *widget, gpointer data){
  Session *session = data;
  GtkComboBox *inputplace;

  close_and_open_window(session, "window_new_reservation");

  //liste déroulante pour le lieu séléctionné de la réservation
  inputplace = GTK_COMBO_BOX(gtk_builder_get_object(session->builder, "combo_new_reservation_where"));
  comboBoxTextFill( GTK_COMBO_BOX_TEXT(inputplace) ,"Choisir un lieu", "SELECT id, name FROM PLACE WHERE state = 1" );

  click_button(session, "button_new_res", getSearchArguments);
}

// ----------------------
void getSearchArguments(GtkWidget *widget,gpointer data){
  Session *session = data;
  Search *search = malloc(sizeof(Search));
  if( search == NULL ) exit(1);

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

  session->search = search;

  open_equipment_window(session);
}


//##############################################################################
// ----------------------
// EQUIPMENTS
void open_equipment_window(Session *session){

  close_and_open_window(session, "window_equipment");
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

  open_drink_window(session);
}

// ----------------------
// DRINK
void open_drink_window(Session *session){
  close_and_open_window(session, "window_drink");
  click_button(session, "button_drink_next", getDrinksCheckbox);
}
// ----------------------

void getDrinksCheckbox(GtkWidget *widget,gpointer data){
  Session *session = data;
  Search *search = session->search;
  int drinks[2] = {0};

  GtkCheckButton *checkCoffee = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_drink_coffee"));
  GtkCheckButton *checkTea = GTK_CHECK_BUTTON(gtk_builder_get_object(session->builder, "check_drink_tea"));

  drinks[0] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkCoffee) );
  drinks[1] = (int)gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON( checkTea) );

  for(int i = 0; i < 2; i++)
    session->search->drinks[i] = drinks[i];

  open_rooms_available_window(session);
}


//##############################################################################
// ----------------------
// ROOMS AVAILABLES

void open_rooms_available_window(Session *session){
  Search *search = session->search;
  RoomGtkBox *room;
  GtkContainer *listContainer;
  MYSQL_ROW row;
  MysqlSelect select;
  Booking *booking;

  close_and_open_window(session,"window_rooms_available");
  //printSearchParameter(s);
  listContainer = GTK_CONTAINER( gtk_builder_get_object(session->builder, "box_available_list") );

  //background_color of the map container
  GtkWidget *viewport_available_right = GTK_WIDGET(gtk_builder_get_object(session->builder,"viewport_available_right"));
  background_color(viewport_available_right , "#FFFFFF");

  select = findAvailableRooms(search);
  while ((row = mysql_fetch_row(select.result)) != NULL){
    room = newRoomAvailable(row);
    displayRoomEquipments(room->equipments, row[0]);
    displayTimeSlotComboBox(room, row[0], search);
    displayTimeSlotLabel(room, row[0], search );

    booking = prepareBooking(search, room, row[0]);
    booking->session = session;
    g_signal_connect (room->bookingButton,"clicked",G_CALLBACK(reserveRoom),booking);
    gtk_container_add ( listContainer, GTK_WIDGET(room->box) );
  }
  mysql_free_result(select.result);
  mysql_close(select.conn);
}

// ----------------------

Booking *prepareBooking( Search *search, RoomGtkBox *room, char *idRoom ){
  Booking *booking;
  GtkComboBox *timeSlotComboBox;
  int priceHalfDay;
  double price = 0;


  booking = malloc( sizeof(Booking) );
  if( booking == NULL ) exit(1);

  booking->idRoom = atoi( idRoom );
  booking->nb_persons = search->nb_persons;
  booking->date = search->date;
  for( int i = 0; i < 2; i++ )
    booking->drinks[i] = search->drinks[i];

  // get the time slot from the comboBoxText
  timeSlotComboBox = GTK_COMBO_BOX( gtk_builder_get_object(room->builder, "combo_available_list_element_when") );
  booking->time_slot = atoi( gtk_combo_box_get_active_id(timeSlotComboBox) );

  price += getPriceDrinks(search);

  priceHalfDay = atoi( gtk_label_get_text(room->priceHalfDay) );
  price += search->time_slot == 2 ? priceHalfDay * 2 : priceHalfDay ;
  booking->price = price;

  return booking;
}

//

int getPriceDrinks(Search *search){
  int price = 0;
  char request[512];
  MYSQL *conn = connect_db();
  MYSQL_ROW row;

  for(int i = 0; i < 2; i++){
    if( search->drinks[i] == 1 ){
      MYSQL_RES *result;
      sprintf(request, "SELECT ppd.price FROM ROOM as R\
      INNER JOIN PLACE as P on R.place = P.id\
      INNER JOIN _place_propose_drink as ppd on ppd.place = P.id\
      WHERE ppd.place = %d AND ppd.drink = %d\
      GROUP BY P.id", search->id_place, search->drinks[i]);
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

void reserveRoom(GtkWidget *widget, gpointer data){
  Booking *b = data;
  Session *session = b->session;
  MYSQL *conn = connect_db();
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char request[512];

  sprintf(request, "INSERT INTO BOOKING(nb_persons,price,date_booking,time_slot,state,room) \
  VALUES(%d,%d,'%d-%d-%d','%s',1,%d) ;",\
  b->nb_persons, (int)b->price, b->date.year, b->date.month, b->date.day, time_slots[b->time_slot], b->idRoom );

  query(conn, request);
  mysql_close(conn);

  open_reservations_window(NULL, session);
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

int isRestDayAvailable( Search *search, char *idRoom ){
  char date[16];
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char time_slot[16] = "";

  sprintf( date, "%d-%d-%d", search->date.year, search->date.month, search->date.day );
  strcpy(time_slot, time_slots[ 1^search->time_slot ] ); // 1 XOR time_slot -> 1^0 = 1 and 1^1 = 0

  return isTimeSlotAvailable(time_slot, date, idRoom);
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
//PLANNING
void open_place_room_window(GtkWidget *widget,gpointer data){
  Session *session = data;
  GtkComboBoxText *place, *room;

  close_and_open_window(session,"window_place_room");
  place = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(session->builder, "combo_place_room_place"));
  room = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(session->builder, "combo_place_room_room"));
  gtk_builder_connect_signals(session->builder, NULL);

  // fill the comboBoxText with places from db
  comboBoxTextFill( place,"Choisir un lieu", "SELECT id, name FROM PLACE WHERE state = 1" );
  g_signal_connect( place,"changed",G_CALLBACK(fillComboBoxRooms),room);


  click_button(session, "button_place_room", getIdRoom);
}

// ----------------------

void getIdRoom(GtkWidget *widget, gpointer data){
  Session *session = data;
  GtkComboBox *room = GTK_COMBO_BOX( gtk_builder_get_object( session->builder, "combo_place_room_room" ) );
  Calendar *calendar = malloc( sizeof(Calendar) );
  if( calendar == NULL ) exit(1);

  calendar->id_room = atoi( gtk_combo_box_get_active_id(room) );
  session->calendar = calendar;
  open_planning_window(session);
}

// ----------------------

void open_planning_window(Session *session){

  close_and_open_window(session,"window_planning");

  getCalendarWidgets(session->calendar, session->builder);
  planningNumbers(session->calendar, session->today);
  setRoomInfo(session->calendar);

  updateButtonsPlanning(session);


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
  c->nav[0] = GTK_BUTTON( gtk_builder_get_object(builder, "button_planning_weeks_back") );
  c->nav[1] = GTK_BUTTON( gtk_builder_get_object(builder, "button_planning_weeks_next") );

  for(i = 0; i < 5; i++){ //days
    sprintf(id, "lbl_planning_nb_%d", i);
    c->days[i] = GTK_LABEL( gtk_builder_get_object(builder, id) );
  }
  for(i = 0; i < 2; i++) //buttonsBooking
    for(j = 0; j < 5; j++){
      sprintf(id, "button_planning_%s_%d",time[i], j);
      c->buttonsBooking[i][j] = GTK_BUTTON( gtk_builder_get_object(builder, id) );
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

}

// ----------------------

void chooseTimeSlot(GtkWidget *widget, gpointer data){
  Session *session = data;
  Calendar *calendar = session->calendar;
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

  updateTimeSlotLabels(session);

  free(selected);
}

// ----------------------

void open_drink_window_2(GtkWidget *Widget,gpointer data){
  Session *session = data;
  Calendar *c = session->calendar;


  close_and_open_window(session, "window_drink");

  printf("%d/%d/%d : %d\n", c->daySelected.year, c->daySelected.month, c->daySelected.day, c->timeSlotSelected );

  click_button(session, "button_drink_next", open_reservations_window2);
}

// ----------------------

void open_reservations_window2(GtkWidget *widget,gpointer data){
  Session *session = data;
  close_and_open_window(session, "window_reservations");
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
