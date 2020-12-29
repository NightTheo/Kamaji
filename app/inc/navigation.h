/*
navigation.h
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
  GtkButton *next;

} Calendar;


typedef struct Session{
  GtkBuilder *builder;
  GtkWindow *window;
  Search *search;
  Calendar *calendar;
  struct tm *today;
} Session;

typedef struct Booking{
  int idRoom;
  int nb_persons;
  double price;
  Date date;
  int drinks[2];
  int time_slot;
  Session *session;
} Booking;

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
void open_planning_window(Session *session);
void open_rooms_available_window(Session *session);
void open_drink_window_2(GtkWidget *Widget,gpointer data);
void open_reservations_window2(GtkWidget *widget,gpointer data);

// GET DATA
void getSearchArguments(GtkWidget *widget,gpointer data);
void getEquipmentsCheckbox(GtkWidget *widget,gpointer data);
void getSearchDrinksCheckbox(GtkWidget *widget,gpointer data);
MysqlSelect findAvailableRooms(Search *search);
int *getRoomsEquipment(char *idRoom);
int isTimeSlotAvailable(char *time_slot, char *date, char *idRoom);
int isRestDayAvailable( Date date, int time_slot_int, char *idRoom );
Booking *prepareBooking( Search *search, RoomGtkBox *room, char *idRoom );
Booking *prepareBookingPlanning(Calendar c);
int getPriceDrinks(int drinks[2], int idPlace);
void getIdRoom(GtkWidget *widget, gpointer data);
void getCalendarWidgets(Calendar *c, GtkBuilder *builder);
int *moveInCalendar(int year, int month, int day, int move);
void getPlanningDrinksCheckbox(GtkWidget *widget, gpointer data);
unsigned int getPriceRoom(int idRoom);

// SET DATA
void fillComboBoxRooms(GtkComboBoxText *place,gpointer room);
RoomGtkBox *newRoomAvailable(MYSQL_ROW row);
void displayRoomEquipments(GtkImage *equipments[4], char *idRoom);
void displayTimeSlotComboBox(RoomGtkBox *room, char *idRoom, Search *search);
void displayTimeSlotLabel(RoomGtkBox *room, char *idRoom, Search *search);
void reserveRoomBySearch(GtkWidget *widget, gpointer data);
void planningNumbers(Calendar *calendar, struct tm *date);
void click_button_planning(Session *session, char *idButton);
void updatePlanningNumbers(int *startDate, GtkLabel *days[5]);
void planningChangeWeek(GtkWidget *widget, gpointer data);
void updateWeekLabel( int *startDate, GtkLabel *week);
void setRoomInfo(Calendar *calendar);
void updateButtonsPlanning( Calendar *calendar, struct tm *td);
void showOrHidePlanningButton(int isAvailable, GtkWidget *button);
void chooseTimeSlot(GtkWidget *widget, gpointer data);
void updateTimeSlotLabels(Calendar *c);
void reserveRoomByPlanning(Booking *b);
void updateTimeSlotComboPlanning(Calendar *calendar);
void onTimeSlotPlanningChanged(GtkWidget *widget, gpointer data);

// PRINT
void printSearchParameter(Search *seach);

//STYLE
void stylePlanningRoom(Session *session);
void background_color_if_sensitive(GtkWidget *widget, char* color);




//
