/*
reservations.c
-------------------------
The user can see all the reservations in the DB with its
data like the room booked, the time slot, the materialin the room, etc.
The user can delete a reservation, it update the BOOKING.state from 1 to 0.
*/
#include "kamaji.h"



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
