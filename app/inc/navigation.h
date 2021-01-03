/*
navigation.h
*/


// free


void click_button(Session *session, char *idButton,void (*function));
void close_and_open_window(Session *session, char *idNewWindow);
void newWindow(char* file, char* idWindow, Session *session);
void background_color( GtkWidget *widget, char *color );

//NAVIGATION
void open_home_window(GtkWidget *widget, gpointer data);
void open_reservations_window(GtkWidget *widget, gpointer data);
void open_place_room_window(GtkWidget *widget, gpointer data);
void open_planning_window(GtkWidget *widget,gpointer data);
void open_drink_window_2(GtkWidget *Widget,gpointer data);
void open_reservations_window2(GtkWidget *widget,gpointer data);
void editReservation(GtkWidget *widget,gpointer data);


// GET DATA
int *getRoomsEquipment(char *idRoom);
Booking *prepareBookingPlanning(Calendar c);
void getIdRoom(GtkWidget *widget, Session *session);
void getCalendarWidgets(Calendar *c, GtkBuilder *builder);
void getPlanningDrinksCheckbox(GtkWidget *widget, gpointer data);
unsigned int getPriceRoom(int idRoom);
MysqlSelect findReservationsInDB();


// SET DATA
delReservation *addDelReservation(Session *session, uint32_t id);
void chooseTimeSlot(GtkWidget *widget, Calendar *calendar);
void reserveRoomByPlanning(Booking *b);
void updateTimeSlotComboPlanning(Calendar *calendar);
void onTimeSlotPlanningChanged(GtkWidget *widget, gpointer data);
void deleteReservation(GtkWidget *widget, delReservation *delReservation);
void abordDeleteReservation(GtkWidget *widget, GtkWidget *window);


// PRINT
void printSearchParameter(Search *seach);

//STYLE
void stylePlanningRoom(Session *session);
void background_color_if_sensitive(GtkWidget *widget, char* color);



//
