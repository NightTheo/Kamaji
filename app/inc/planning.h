/*
planning.h
-------------------------
headers of the src/planning.c file
*/

// Place Room
void open_place_room_window(GtkWidget *widget, gpointer data);
void getIdRoom(GtkWidget *widget, Session *session);
void checkDataPlaceRoom(GtkWidget *widget, gpointer data);

// Planning
void open_planning_window(GtkWidget *widget,gpointer data);
void getCalendarWidgets(Calendar *c, GtkBuilder *builder);
void chooseTimeSlot(GtkWidget *widget, Calendar *calendar);
void updateTimeSlotComboPlanning(Calendar *calendar);
void onTimeSlotPlanningChanged(GtkWidget *widget, gpointer data);


// Drinks
void open_drink_window_2(GtkWidget *Widget,gpointer data);
void getPlanningDrinksCheckbox(GtkWidget *widget, gpointer data);
Booking *prepareBookingPlanning(Calendar c);
unsigned int getPriceRoom(int idRoom);
void reserveRoomByPlanning(Booking *b);
