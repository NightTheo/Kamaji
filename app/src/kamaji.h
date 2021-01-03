/*
kamaji.h
-------------------------

*/

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <mysql/mysql.h>
#include <gtk/gtk.h>

// DEFINE
#define GLADE_FILE "ui/glade/home.glade"


// STRUCTS

/*
Struct : RoomGtkBox
-------------------------
With a builder, get all the widgets needed to make a member of the available rooms list
from the xml file and store them in the struct to update them according to the room in DB.
*/
typedef struct RoomGtkBox{
  GtkBox *box;
  GtkBuilder *builder;
  GtkLabel *locationLabel;
  GtkLabel *timeSlotLabel;
  GtkLabel *priceHalfDay;
  GtkLabel *nbPersons;
  GtkImage *equipments[4];
  GtkComboBoxText *bookingTimeSlotComboBox;
  GtkButton *bookingButton;
} RoomGtkBox;


/*
Struct : Date
-------------------------
Store the year, the month and the day
*/
typedef struct Date{
  int year;
  int month;
  int day;
} Date;


/*
Struct : Booking
-------------------------
Store all the data useful to insert in the BOOKING table in DB.
Allocated each time a result in rooms available is displayed, and passed in callback
when the user want to book one.
Used too when the user make a reservation by the calendar.
Node of a linked list.
*/
typedef struct Booking{
  char test[8];
  int idRoom;
  int nb_persons;
  double price;
  Date date;
  int drinks[2];
  int time_slot;
  void *session;
  struct Booking *next;
} Booking;


/*
Struct : Search
-------------------------
Store the data useful to search the available rooms, pass from window to window until the list of the rooms.
*/
typedef struct Search{
  int id_place;
  int nb_persons;
  int time_slot;
  Date date;
  int equipments[4];
  int drinks[2];
  Booking *startBooking;
} Search;


/*
Struct : Calendar
-------------------------
Store all the widgets useful in the planning window like labels, buttons, etc and
data for the navigation in the calendar, like the date of the monday of the week currently displayed,
or wich time slot is selected, etc.
*/
typedef struct Calendar{
  int id_room;
  int id_place;
  Date planning;
  Date daySelected;
  int timeSlotSelected;
  int wDaySelected;
  int drinks[2];

  GtkLabel *week;
  GtkLabel *days[5];
  GtkButton *buttonsBooking[2][5];
  GtkLabel *room;
  GtkLabel *place;
  GtkLabel *dateLabel;
  GtkLabel *timeSlotLabel;
  GtkLabel *price;
  GtkImage *equipments[4];
  GtkComboBoxText *timeSlotCombo;
  GtkButton *next; // button to book the room
} Calendar;


/*
Struct : MysqlSelect
-------------------------
To make a request in the DB in a function and use the result in another one, the connexion
is close later and the reslut free later, so they have to be returned.
*/
typedef struct MysqlSelect{
  MYSQL *conn;
  char request[1024];
  MYSQL_RES *result;
} MysqlSelect;


/*
Struct : ReservationBox
-------------------------
With a builder, get all the widgets needed to make a member of the reservations list
from the xml file and store them in the struct to update them according
to the reservation data in DB.
*/
typedef struct ReservationBox{
  GtkBox *box;
  GtkLabel *locationLabel;
  GtkLabel *dateLabel;
  GtkLabel *timeSlotLabel;
  GtkLabel *priceLabel;
  GtkImage *equipments[4];
  GtkImage *drinks[2];
  GtkButton *delete;
} ReservationBox;


/*
Struct : delReservation
-------------------------
Node of the linked list used for delete a reservation because all reservations have
a delete button which callback a function. The data passed to this function have to
be allocated because it is async.
*/
typedef struct delReservation{
  uint32_t idBooking;
  GtkWidget *dialogWindow;
  void *session;
  struct delReservation *next;
}delReservation;


/*
Struct : Session
-------------------------
Store the main variables used in the app : search and calendar.
To access the widgets the user see, we have to use the builder
wich built the current window (also stored).
backFunction is the pointer of the function wich open the previous window.
*/
typedef struct Session{
  GtkBuilder *builder;
  GtkWindow *window;
  Search *search;
  Calendar *calendar;
  struct tm *today;
  struct delReservation *nextReservation;
  void (*backFunction)(GtkWidget *, gpointer);
} Session;



// INCLUDE
#include "../inc/config_mysql.h"
#include "../inc/navigation.h"
#include "../inc/planning.h"
#include "../inc/reservations.h"
#include "../inc/search.h"
#include "../inc/style.h"













//
