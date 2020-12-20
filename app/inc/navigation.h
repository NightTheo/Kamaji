/*
import_glade.h

*/

//Struct

typedef struct Search{
  int place;
  int nb_persons;
  char *time_slot;
  char *date;
  int equipment[4];
  int drinks[2];
} Search;

typedef struct Session{
  GtkBuilder *builder;
  GtkWindow *window;
  Search Search;
} Session;


void open_home_window(char *idWindow);

void click_button(Session *session, char *idButton,void (*pf));
void *close_and_open_window(Session *session, char *idNewWindow);
void newWindow(char* file, char* idWindow, Session *session);
void background_color( GtkWidget **widget, char *color );

//NAVIGATION
void open_reservations_window(GtkWidget *widget,gpointer data);
void open_new_res_window(GtkWidget *widget,gpointer data);
void open_place_room_window(GtkWidget *widget,gpointer data);
void open_equipment_window(GtkWidget *widget,gpointer data);
void open_drink_window(GtkWidget *Widget,gpointer data);
void open_planning_window(GtkWidget *widget,gpointer data);
void open_rooms_available_window(GtkWidget *Widget,gpointer data);
void open_drink_window_2(GtkWidget *Widget,gpointer data);
void open_reservations_window2(GtkWidget *widget,gpointer data);

void fillRooms(GtkComboBoxText *place,gpointer room);

void on_window_main_destroy();
void on_window_new_booking_destroy();




//
