/*
reservations.h
-------------------------
headers of the src/reservations.c file
*/


void open_reservations_window(GtkWidget *widget, gpointer data);
void freeDelReservations(delReservation **start);
delReservation *addDelReservation(Session *session, uint32_t id);
MysqlSelect findReservationsInDB();
ReservationBox newReservation();
void displayReservationData(ReservationBox reservation,MYSQL_ROW row);
void displayReservationDrinks(GtkImage *drinks[2], char *idBooking);
int *getReservationDrinks(char *idBooking);
void deleteReservation(GtkWidget *widget, delReservation *delReservation);
void abordDeleteReservation(GtkWidget *widget, GtkWidget *window);
void confirmDeleteReservation(GtkWidget *widget, gpointer data);
void setDateReservation(char *dateSQL, GtkLabel *label);
