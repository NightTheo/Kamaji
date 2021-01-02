/*
import_glade.c

create the windows from the file glade in ui/glade/home.glade
connet the signals for the navigation between the windows

*/

#include <stdint.h>
#include <time.h>
#include <gtk/gtk.h>
#include "../inc/navigation.h"
#include "../inc/insertDataGtk.h"


//defines
#define GLADE_FILE "ui/glade/home.glade"


/*
-----------------------------------------------------------------------------------------------------------
Function : open_home_window
-------------------------
Open the home window, the first one. User can choose between
see all the reservations, search the rooms availables or see the planning
of a room
-------------------------
GtkWidget *widget : widget activated to open the window
Session *session : address of the struct Session
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


/*
-----------------------------------------------------------------------------------------------------------
Function : click_button
-------------------------
Get a button and give it the function to call when is clicked
-------------------------
Session *session : adress of the struct Session
char *idButton : the id of the button in the xml file
void (*function) : the callback function called when the button is clicked
*/
void click_button(Session *session, char *idButton, void (*function)){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(session->builder, idButton));
  g_signal_connect (button,"clicked",G_CALLBACK(function),session);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : close_and_open_window
-------------------------
Close the current window present in the struct Session and open a new one
-------------------------
Session *session : address of the struct Session
char *idNewWindow : id of the new window in the xml file
*/
void close_and_open_window(Session *session, char *idNewWindow){
  gtk_widget_destroy( GTK_WIDGET( session->window ) );
  newWindow(GLADE_FILE, idNewWindow, session);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : newWindow
-------------------------
Close the current window present in the struct Session and open a new one
-------------------------
char* file : path of the xml file
char* idWindow : id of the widow in the xml file
Session *session : address of the struct Session
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


/*
-----------------------------------------------------------------------------------------------------------
Function : background_color
-------------------------
Change the back ground color of a widget
-------------------------
GtkWidget *widget : the widget to change
char *color : color in hexadecimal like #RRGGBB
*/
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

/*
-----------------------------------------------------------------------------------------------------------
Function : open_reservations_window
-------------------------
Open the window wich contains the list of all the reservations
-------------------------
GtkWidget *widget : widget activated to open the window
Session *session : address of the struct Session
*/
void open_reservations_window(GtkWidget *widget, gpointer data){
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


/*
-----------------------------------------------------------------------------------------------------------
Function : addDelReservation
-------------------------
Add a node in the linked list of delReservation structs.
The struct is passed in the callback function when the delete button is clicked
-------------------------
Session *session : address of the struct Session
uint32_t id : the id of the reservation (BOOKING.id in the DB)
-------------------------
Return values
    delReservation *inter: the new struct in the linked list
*/
delReservation *addDelReservation(Session *session, uint32_t id){
  delReservation *inter = malloc( sizeof(delReservation) );
  if( inter == NULL ) exit(1);
  inter->idBooking = id;
  inter->session = session;
  inter->next = session->nextReservation;
  return inter;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : deleteReservation
-------------------------
Open a dialog window, to be sure if the user wants to delete the booking.
If yes, the state column in BOOKING is passed to 0.
-------------------------
GtkWidget *widget : widget activated to open the window, here the delete button
delReservation *delReservation: pointer of struct which contains the id of the reservation to delete & the Session
*/
void deleteReservation(GtkWidget *widget, delReservation *delReservation){
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

void abordDeleteReservation(GtkWidget *widget, GtkWidget *window){
  gtk_widget_destroy(window);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : findReservationsInDB
-------------------------
Select the data of all the active reservations in DB
The connexion will be closed later so it has to be return, here in the struct MysqlSelect
-------------------------
Return values
    MysqlSelect select : struct with the connexion of DB and the result of the select
*/
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


//##############################################################################
// ----------------------
// EQUIPMENTS
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



// ----------------------
// DRINK
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


//##############################################################################
// ----------------------
// ROOMS AVAILABLES
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
Function : background_color_if_sensitive
-------------------------
if the widget is clickable then call the function background_color to the color of the widget.
-------------------------
GtkWidget *widget: widget to change.
char *color: string of the code color in hexadecimal like "#RRGGBB"
*/
void background_color_if_sensitive(GtkWidget *widget, char* color){
  if( (int)gtk_widget_is_sensitive (widget) )
    background_color( widget, color );
}

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

// STYLE
/*
-----------------------------------------------------------------------------------------------------------
Function : stylePlanningRoom
-------------------------
Change the style of the planning window.
-------------------------
Session *session : address of the struct Session
*/
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
