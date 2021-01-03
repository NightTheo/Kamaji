/*
config_mysql.h
-------------------------
headers of the src/config_mysql.c file
*/

Session *initSession();
void kamajiQuit(GtkWidget *w, gpointer data);

// BACK
void back(GtkWidget *widget, gpointer data);


// free
void freeDelReservations(delReservation **start);


void comboBoxTextFill( GtkComboBoxText *comboBoxText, char *firstRow, char *request );
void fillComboBoxRooms(GtkComboBoxText *place,gpointer room);




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


void checkDataPlaceRoom(GtkWidget *widget, gpointer data);





//
