/*
insertDataGtk.h
*/
Session *initSession();
void kamajiQuit(GtkWidget *w, gpointer data);

// BACK
void back(GtkWidget *widget, gpointer data);


// free
void freeDelReservations(delReservation **start);


void comboBoxTextFill( GtkComboBoxText *comboBoxText, char *firstRow, char *request );
void fillComboBoxRooms(GtkComboBoxText *place,gpointer room);
RoomGtkBox newRoomAvailable( MYSQL_ROW row);
ReservationBox newReservation();
void displayReservationData(ReservationBox reservation,MYSQL_ROW row);
void displayReservationDrinks(GtkImage *drinks[2], char *idBooking);
int *getReservationDrinks(char *idBooking);
void confirmDeleteReservation(GtkWidget *widget, gpointer data);
void displayRoomEquipments(GtkImage *equipments[4], char *idRoom);
void displayTimeSlotComboBox(RoomGtkBox room, char *idRoom, Search *search);
void displayTimeSlotLabel(GtkLabel *label, char *idRoom, Date date, int time_slot);
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
void setDateReservation(char *dateSQL, GtkLabel *label);
void focusDateCalendar(GtkCalendar *calendar);
uint8_t checkDataInputPlace(GtkComboBox *place);
uint8_t checkDataCalendar(GtkCalendar *calendar);
void checkDataNewRes(GtkWidget *widget, gpointer data);
void checkDataPlaceRoom(GtkWidget *widget, gpointer data);





//
