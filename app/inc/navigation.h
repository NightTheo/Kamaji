/*
import_glade.h

*/

//Struct

typedef struct Date{
  int year;
  int month;
  int day;
} Date;

typedef struct Search{
  int id_place;
  int nb_persons;
  int time_slot;
  Date date;
  int equipments[4];
  int drinks[2];
} Search;

typedef struct Calendar{
  int id_room;
  Date date;
  int time_slot;
} Calendar;

typedef struct Booking{
  int nb_persons;
  double price;
  Date date;
  int time_slot;
} Booking;

typedef struct Session{
  GtkBuilder *builder;
  GtkWindow *window;
  Search *search;
  Calendar *calendar;
  Booking *booking;
} Session;

typedef struct MysqlSelect{
  MYSQL *conn;
  char request[1024];
  MYSQL_RES *result;
} MysqlSelect;

typedef struct RoomGtkBox{
  GtkBuilder *builder;
  GtkBox *box;
  GtkLabel *locationLabel;
  GtkLabel *timeSlotLabel;
  GtkLabel *priceHalfDay;
  GtkLabel *nbPersons;
  GtkImage *equipments[4];
  GtkComboBoxText *bookingTimeSlotComboBox;
  GtkButton *bookingButton;
  /*
  box : box_available_list_element_0
  room - place : lbl_available_list_element_0_where
  time slot : lbl_available_list_element_0_time_slot
  price 1/2 day: lbl_available_list_element_0_price
  nb persons: lbl_available_list_element_0_left_nb
  equipments : img_rooms_available_{ whiteboard / monitor / projector/ camera }0
  booking time slot : combo_available_list_element_0_when
  booking button : button_available_list_element_0_booking
  */
} RoomGtkBox;


void open_home_window(char *idWindow);

void click_button(Session *session, char *idButton,void (*function));
void close_and_open_window(Session *session, char *idNewWindow);
void newWindow(char* file, char* idWindow, Session *session);
void background_color( GtkWidget *widget, char *color );

//NAVIGATION
void open_reservations_window(GtkWidget *widget,gpointer data);
void open_new_res_window(GtkWidget *widget, gpointer data);
void open_place_room_window(GtkWidget *widget,gpointer data);
void open_equipment_window(Session *session);
void open_drink_window(Session *session);
void open_planning_window(GtkWidget *widget,gpointer data);
void open_rooms_available_window(Session *session);
void open_drink_window_2(GtkWidget *Widget,gpointer data);
void open_reservations_window2(GtkWidget *widget,gpointer data);

// GET DATA
void getSearchArguments(GtkWidget *widget,gpointer data);
void getEquipmentsCheckbox(GtkWidget *widget,gpointer data);
void getDrinksCheckbox(GtkWidget *widget,gpointer data);
MysqlSelect findAvailableRooms(Search *search);


// SET DATA
void fillComboBoxRooms(GtkComboBoxText *place,gpointer room);
void addRoomAvailable(MYSQL_ROW row, Session *session, GtkContainer *listContainer);

// PRINT
void printSearchParameter(Search *seach);






//
