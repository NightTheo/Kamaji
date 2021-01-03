/*
search.c
-------------------------
Search the available rooms in db according to the user wishes
like the date, time slot, nb of persons and place he wants.
Display the list of the rooms with its details, and the possiblity of
booking the room to this date.
*/
#include "kamaji.h"


/*
******************************
*   NEW RESERVATION WINDOW   *
******************************
*/

/*
-----------------------------------------------------------------------------------------------------------
Function : open_new_res_window
-------------------------
Open the servation window wich get the arguments of the search
-------------------------
GtkWidget *widget : widget activated to open the window
delReservation *delReservation: pointer of struct which contains the id of the reservation to delete & the Session
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
  g_signal_connect( nextButton, "clicked", G_CALLBACK(freeCheckDataSearch), check );

  click_button(session, "button_new_res", getSearchArguments);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : freeCheckDataSearch
-------------------------
Free the array check.
check is an array of GtkWidget*. It contains the validation button, the ComboBox of places and the Calendar.
-------------------------
GtkWidget *widget : widget activated to open the window,
gpointer data: (void*) the check array of widgets *
*/
void freeCheckDataSearch(GtkWidget *widget, gpointer data){
  GtkWidget **check = data;
  free(check);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getSearchArguments
-------------------------
Get the widgets used to specify the search, then get the data inside.
And open the the equipments window.
-------------------------
GtkWidget *widget : widget activated to open the window,
Session *session : address of the struct Session
*/
void getSearchArguments(GtkWidget *widget, Session *session){
  Search *search = session->search;
  GtkComboBox *inputplace;
  GtkComboBox *inputTime;
  GtkSpinButton *inputNbPeoples;
  GtkCalendar *inputDate;

  inputplace = GTK_COMBO_BOX(gtk_builder_get_object(session->builder, "combo_new_reservation_where"));
  inputNbPeoples = GTK_SPIN_BUTTON(gtk_builder_get_object(session->builder, "spin_new_reservation_group"));
  inputTime = GTK_COMBO_BOX(gtk_builder_get_object(session->builder, "combo_new_reservation_when"));
  inputDate = GTK_CALENDAR(gtk_builder_get_object(session->builder, "calendar_new_res"));

  search->id_place = atoi( gtk_combo_box_get_active_id( GTK_COMBO_BOX(inputplace) ) );
  search->nb_persons = gtk_spin_button_get_value_as_int(inputNbPeoples) ;
  search->time_slot = atoi( gtk_combo_box_get_active_id( GTK_COMBO_BOX(inputTime) ) );
  gtk_calendar_get_date(inputDate, (guint*)&search->date.year, (guint*)&search->date.month, (guint*)&search->date.day);
  search->date.month++;

  open_equipment_window(NULL, session);
}




/*
******************************
*     EQUIPMENTS WINDOW      *
******************************
*/

/*
-----------------------------------------------------------------------------------------------------------
Function : open_equipment_window
-------------------------
Open the window where the user chooses the desired equipments.
-------------------------
GtkWidget *widget : widget activated to open the window
Session *session : address of the struct Session
*/
void open_equipment_window(GtkWidget *widget, gpointer data){
  Session *session = data;
  GtkButton *backButton;

  session->backFunction = open_new_res_window;
  close_and_open_window(session, "window_equipment");
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_equiments") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  click_button(session, "button_equipment_search", getEquipmentsCheckbox);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getEquipmentsCheckbox
-------------------------
Get the value of the equipment's checkboxes, put them in an array of 0 and 1 and open the drinks window
-------------------------
GtkWidget *widget : widget activated to open the window
Session *session : address of the struct Session
*/
void getEquipmentsCheckbox(GtkWidget *widget, Session *session){
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




/*
******************************
*        DRINKS WINDOW       *
******************************
*/

/*
-----------------------------------------------------------------------------------------------------------
Function : open_drink_window
-------------------------
Open the window where the user chooses the desired drinks.
-------------------------
GtkWidget *widget : widget activated to open the window
Session *session : address of the struct Session
*/
void open_drink_window(GtkWidget *widget, gpointer data){
  Session *session = data;
  GtkButton *backButton;

  session->backFunction = open_equipment_window;
  close_and_open_window(session, "window_drink");
  backButton = GTK_BUTTON( gtk_builder_get_object(session->builder, "button_back_from_drink") );
  g_signal_connect(backButton, "clicked", G_CALLBACK(back), session);

  click_button(session, "button_drink_next", getSearchDrinksCheckbox);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : getSearchDrinksCheckbox
-------------------------
Get the value of the drinks's checkboxes, put them in the array of 0 and 1 Search.drinks and open the rooms available window
-------------------------
GtkWidget *widget : widget activated to open the window
Session *session : address of the struct Session
*/
void getSearchDrinksCheckbox(GtkWidget *widget, Session *session){
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




/*
******************************
*      AVAILABLE ROOMS       *
******************************
*/

/*
-----------------------------------------------------------------------------------------------------------
Function : open_rooms_available_window
-------------------------
Open the window with the list of all the rooms available with the desired conditions
-------------------------
GtkWidget *widget : widget activated to open the window
Session *session : address of the struct Session
*/
void open_rooms_available_window(GtkWidget *widget, gpointer data){
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
  displayMapPlace(session->builder, search->id_place);

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


/*
-----------------------------------------------------------------------------------------------------------
Function : displayMapPlace
-------------------------
Change the path of the image of the map
-------------------------
GtkBuilder *builder : address of the builder which built the image
*/
void displayMapPlace(GtkBuilder *builder, uint32_t id_place){
  char *path;
  GtkImage *image;

  path = getPathMapPlace(id_place);
  image = GTK_IMAGE( gtk_builder_get_object(builder,"img_rooms_available_") );
  gtk_image_set_from_file (image, path);

  free(path);
}

/*
-----------------------------------------------------------------------------------------------------------
Function : getPathMapPlace
-------------------------
Get the path of the map image in the DB
-------------------------
uint32_t id_place : id of the place wanted
-------------------------
Return values
    char *path : the path found, or the default image's path
*/
char *getPathMapPlace(uint32_t id_place){
  MYSQL_ROW row;
  MYSQL *conn = connect_db();
  MYSQL_RES *result;
  char request[64];
  char *path;
  uint8_t error = 0;

  path = malloc( sizeof(char) * 64 );
  if( path == NULL ) exit(1);

  sprintf(request, "SELECT map FROM PLACE WHERE id = %d", id_place);
  result = query(conn, request);
  if((row = mysql_fetch_row(result)) != NULL){
    if( strlen(*row) < 64)
      strcpy( path, *row );
    else error = 1;
  }
  else error = 1;

  if( !fileExists(path) ) error = 1;

  if(error) strcpy(path, "ui/img/maps/default.jpg");

  mysql_free_result(result);
  mysql_close(conn);

  return path;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : fileExists
-------------------------
says if the given path is a file, at least an readable file
-------------------------
char *path : path of the file
-------------------------
Return values
    uint8_t exists : boolean, 1 if the file exists, else 0
*/
uint8_t fileExists(char *path){
  uint8_t exists;
  FILE *fp = fopen( path, "r" );
  exists = fp != NULL ? 1 : 0;
  fclose(fp);
  return exists;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : displayDataRoom
-------------------------
Set all the data selected in DB in the widgets.
Add node in the linked list of booking
-------------------------
RoomGtkBox room : struct of the room with all the widgets
MYSQL_ROW row : array of strings of the current found room
Session *session : address of the struct Session
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function : hasRequiredEquipments
-------------------------
Check if a room has the equipments required in the search
-------------------------
int requiredEquipments[4] : array of the required equipement : 1 is required, 0 not
char *idRoom : string of the id of the room in DB
-------------------------
Return value
    uint8_t state : boolean, 1 if the room possed all the desired equipment
*/
uint8_t hasRequiredEquipments(int requiredEquipments[4], char *idRoom){
  uint8_t state = 1;
  int *roomEquipments = getRoomsEquipment(idRoom);

  for(int i = 0; i < 4; i++)
    if( requiredEquipments[i] == 1 && roomEquipments[i] == 0)
      state = 0;

  free(roomEquipments);
  roomEquipments = NULL;
  return state;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : prepareBooking
-------------------------
Check if a room has the equipments required in the search
-------------------------
Search *search : address of struct Search
RoomGtkBox room :
char *idRoom : id of the room
-------------------------
Return value
    Booking *booking : struct with all the data ready to insert in BOOKING table
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function : getPriceDrinks
-------------------------
Get the sum of the price of the desired drinks.
If a drink is desired, get the price proposed by the place and sum it to the int price
-------------------------
int drinks[2] : array of the desired drinks
int idPlace : id of the place in DB
-------------------------
Return value
    int price : sum of the price of the desired drinks.
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function : reserveRoomBySearch
-------------------------
insert the reservation in DB. Then open the reservations window.
-------------------------
GtkWidget *widget: widget activated to open the window, here the booking button
Booking *b : address of the struct Booking, which contains all the data of the room the user wants to book
*/
void reserveRoomBySearch(GtkWidget *widget, Booking *b){
  Session *session = b->session;
  MYSQL *conn = connect_db();
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char request[512];

  sprintf(request, "INSERT INTO BOOKING(nb_persons,price,date_booking,time_slot,state,room) \
  VALUES(%d,%d,'%d-%d-%d','%s',1,%d) ;",\
  b->nb_persons, (int)b->price, b->date.year, b->date.month, b->date.day, time_slots[b->time_slot], b->idRoom );


  MYSQL_RES *result = query(conn, request);
  insertDrinks(b->drinks, conn );

  freeBookings(&session->search->startBooking);

  mysql_close(conn);
  mysql_free_result(result);

  open_reservations_window(NULL, session);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : insertDrinks
-------------------------
Get the last id inserted in BOOKING then insert the drinks in DB (table _booking_include_drink).
-------------------------
int drinks[2] : array of the desired drinks
MYSQL *conn : connexion of the DB which inserted the booking
*/
void insertDrinks(int drinks[2], MYSQL *conn){
  uint32_t idBooking;
  char request[512];
  MYSQL *connInsert = connect_db();
  MYSQL_RES *resultId;
  MYSQL_RES *resultInsert;
  MYSQL_ROW row;
  uint8_t i;

  resultId = query(conn, "SELECT LAST_INSERT_ID()");
  row = mysql_fetch_row(resultId);
  if( row != NULL && atoi(*row) > 0){
    idBooking = (uint32_t)atoi(*row);

    for(i = 0; i < 2; i++){
      if( drinks[i] ){
        sprintf(request, "INSERT INTO _booking_include_drink VALUES(%d,%d)", idBooking, i+1);
        resultInsert = query(connInsert, request);
        mysql_free_result(resultInsert);
      }

    }

  }else{
    printf("Error insert drinks\n");
    exit(1);
  }

  mysql_free_result(resultId);
  mysql_close(connInsert);

}


/*
-----------------------------------------------------------------------------------------------------------
Function : freeBookings
-------------------------
Free all the linked list of struct Booking.
-------------------------
Booking **start : address of the pointer of the last node added.
*/
void freeBookings(Booking **start){
  while(*start != NULL){
    Booking *remove = *start;
    *start = (*start)->next;
    free(remove);
  }
}


/*
-----------------------------------------------------------------------------------------------------------
Function : findAvailableRooms
-------------------------
Get the available rooms in DB with the specifics conditions of the user.
-------------------------
Search *s : address of the struct Search wich contains all the data the user entered for the search
-------------------------
Return value
    MysqlSelect select : struct storing the connexionand the result.
                         The conn will be closed later, so the connexion has to be returned
*/
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


/*
-----------------------------------------------------------------------------------------------------------
Function : isRestDayAvailable
-------------------------
Invert the time slot: "8h - 14h" becomes "14h - 20h", by a xor of the time slot searched.
The call the function isTimeSlotAvailable with the inverted time slot.
-------------------------
 Date date : desired date in search
 int time_slot_int : desired time slot in search
 char *idRoom : string of the id of the room in DB
-------------------------
Return value
    int : boolean, 1 if the rest of the day is available, 0 if not
*/
int isRestDayAvailable( Date date, int time_slot_int, char *idRoom ){
  char dateString[16];
  char time_slots[3][16]= {"8h - 14h", "14h - 20h", "8h - 20h"};
  char time_slot[16] = "";

  sprintf( dateString, "%d-%d-%d", date.year, date.month, date.day );
  strcpy(time_slot, time_slots[ 1^time_slot_int ] ); // 1 XOR time_slot -> 1^0 = 1 and 1^1 = 0

  return isTimeSlotAvailable(time_slot, dateString, idRoom);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : isTimeSlotAvailable
-------------------------
select in DB
-------------------------
 char *time_slot : string of the time slot
 char *date : date with the sql format yyyy-[1-12]-[1-31]
 char *idRoom : string of the id of the room in DB
-------------------------
Return value
    int : 1 or -1. 1 if the rest of the time slot of this room at this date is available,  AND -1 if not !!!
*/
int isTimeSlotAvailable(char *time_slot, char *date, char *idRoom){
  int isAvailable;
  MYSQL *conn = connect_db();
  MYSQL_RES *result;
  MYSQL_ROW row;
  char request[512];

  // return "1" if a room is available at a date and a time slot, "-1" if not
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




//
