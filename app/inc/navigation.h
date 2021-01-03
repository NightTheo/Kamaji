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

void open_planning_window(GtkWidget *widget,gpointer data);
void open_drink_window_2(GtkWidget *Widget,gpointer data);
void open_reservations_window2(GtkWidget *widget,gpointer data);


// GET DATA

Booking *prepareBookingPlanning(Calendar c);
void getCalendarWidgets(Calendar *c, GtkBuilder *builder);
void getPlanningDrinksCheckbox(GtkWidget *widget, gpointer data);
unsigned int getPriceRoom(int idRoom);



// SET DATA

void chooseTimeSlot(GtkWidget *widget, Calendar *calendar);
void reserveRoomByPlanning(Booking *b);
void updateTimeSlotComboPlanning(Calendar *calendar);
void onTimeSlotPlanningChanged(GtkWidget *widget, gpointer data);




// PRINT
void printSearchParameter(Search *seach);

//STYLE
void stylePlanningRoom(Session *session);
void background_color_if_sensitive(GtkWidget *widget, char* color);



//
