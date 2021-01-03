/*
search.h
-------------------------
headers of the src/search.c file
*/

// New reservation
void open_new_res_window(GtkWidget *widget, gpointer data);
void freeCheckDataSearch(GtkWidget *widget,gpointer data);
void getSearchArguments(GtkWidget *widget, Session *session);
void focusDateCalendar(GtkCalendar *calendar);
uint8_t checkDataInputPlace(GtkComboBox *place);
uint8_t checkDataCalendar(GtkCalendar *calendar);
void checkDataNewRes(GtkWidget *widget, gpointer data);

// Equipments
void open_equipment_window(GtkWidget *widget, gpointer data);
void getEquipmentsCheckbox(GtkWidget *widget, Session *session);

// Drinks
void open_drink_window(GtkWidget *widget, gpointer data);
void getSearchDrinksCheckbox(GtkWidget *widget, Session *session);

// Available rooms
void open_rooms_available_window(GtkWidget *widget, gpointer data);
void displayMapPlace(GtkBuilder *builder, uint32_t id_place);
RoomGtkBox newRoomAvailable( MYSQL_ROW row);
char *getPathMapPlace(uint32_t id_place);
uint8_t fileExists(char *path);
void displayDataRoom(RoomGtkBox room, MYSQL_ROW row, Session *session);
void displayRoomEquipments(GtkImage *equipments[4], char *idRoom);
int *getRoomsEquipment(char *idRoom);
void displayTimeSlotComboBox(RoomGtkBox room, char *idRoom, Search *search);
void displayTimeSlotLabel(GtkLabel *label, char *idRoom, Date date, int time_slot);
uint8_t hasRequiredEquipments(int requiredEquipments[4], char *idRoom);
Booking *prepareBooking( Search *search, RoomGtkBox room, char *idRoom );
int getPriceDrinks(int drinks[2], int idPlace);
void insertDrinks(int drinks[2], MYSQL *conn);
void freeBookings(Booking **start);
MysqlSelect findAvailableRooms(Search *search);
int isRestDayAvailable( Date date, int time_slot_int, char *idRoom );
int isTimeSlotAvailable(char *time_slot, char *date, char *idRoom);
void reserveRoomBySearch(GtkWidget *widget, Booking *b);
