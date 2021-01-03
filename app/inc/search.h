/*
search.h
-------------------------
headers of the search.c file
*/

// New reservation
void open_new_res_window(GtkWidget *widget, gpointer data);
void freeCheckDataSearch(GtkWidget *widget,gpointer data);
void getSearchArguments(GtkWidget *widget, Session *session);

// Equipments
void open_equipment_window(GtkWidget *widget, gpointer data);
void getEquipmentsCheckbox(GtkWidget *widget, Session *session);

// Drinks
void open_drink_window(GtkWidget *widget, gpointer data);
void getSearchDrinksCheckbox(GtkWidget *widget, Session *session);

// Available rooms
void open_rooms_available_window(GtkWidget *widget, gpointer data);
void displayMapPlace(GtkBuilder *builder, uint32_t id_place);
char *getPathMapPlace(uint32_t id_place);
uint8_t fileExists(char *path);
void displayDataRoom(RoomGtkBox room, MYSQL_ROW row, Session *session);
uint8_t hasRequiredEquipments(int requiredEquipments[4], char *idRoom);
Booking *prepareBooking( Search *search, RoomGtkBox room, char *idRoom );
int getPriceDrinks(int drinks[2], int idPlace);
void reserveRoomBySearch(GtkWidget *widget, Booking *b);
void insertDrinks(int drinks[2], MYSQL *conn);
void freeBookings(Booking **start);
MysqlSelect findAvailableRooms(Search *search);
int isRestDayAvailable( Date date, int time_slot_int, char *idRoom );
int isTimeSlotAvailable(char *time_slot, char *date, char *idRoom);
