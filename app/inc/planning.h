/*
planning.h
-------------------------
headers of the src/planning.c file
*/

// Place Room
void open_place_room_window(GtkWidget *widget, gpointer data);
void getIdRoom(GtkWidget *widget, Session *session);
void checkDataPlaceRoom(GtkWidget *widget, gpointer data);
void comboBoxTextFill( GtkComboBoxText *comboBoxText, char *firstRow, char *request );
void fillComboBoxRooms(GtkComboBoxText *place,gpointer room);

// Planning
void open_planning_window(GtkWidget *widget,gpointer data);
void getCalendarWidgets(Calendar *c, GtkBuilder *builder);
void chooseTimeSlot(GtkWidget *widget, Calendar *calendar);
void updateTimeSlotComboPlanning(Calendar *calendar);
void onTimeSlotPlanningChanged(GtkWidget *widget, gpointer data);
void planningNumbers(Calendar *calendar, struct tm *date);
void updatePlanningNumbers(int *startDate, GtkLabel *days[5]);
int *moveInCalendar(int year, int month, int day, int move);
void click_button_planning(Session *session, char *idButton);
void planningChangeWeek(GtkWidget *widget, gpointer data);
void updateWeekLabel( int *startDate, GtkLabel *week);
void setRoomInfo(Calendar *calendar);
void updateButtonsPlanning( Calendar *calendar, struct tm *td);
void showOrHidePlanningButton(int isAvailable, GtkWidget *button);
void updateTimeSlotLabels(Calendar *c);


// Drinks
void open_drink_window_2(GtkWidget *Widget,gpointer data);
void getPlanningDrinksCheckbox(GtkWidget *widget, gpointer data);
Booking *prepareBookingPlanning(Calendar c);
unsigned int getPriceRoom(int idRoom);
void reserveRoomByPlanning(Booking *b);
