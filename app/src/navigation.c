/*
navigation.c
-------------------------
Functions to navigate betweens the windows, the initiation of the app
and the end of the app.
*/
#include "kamaji.h"

//defines


// NAVIGATION

/*
-----------------------------------------------------------------------------------------------------------
Function : initSession
-------------------------
Allocates memory for the main variables used : session, search and calendar.
Init the linked list of the reservations.
-------------------------
Return value
  Session *session: the session created
*/
Session *initSession(){
  Session *session;
  Search *search;
  Calendar *calendar;

  session = malloc(sizeof(Session));
  search = malloc(sizeof(Search));
  calendar = malloc(sizeof(Calendar));
  if( session == NULL || search == NULL || calendar == NULL)
    exit(1);

  search->startBooking = NULL;

  session->search = search;
  session->calendar = calendar;
  session->nextReservation = NULL;

  return session;
}


/*
-----------------------------------------------------------------------------------------------------------
Function : kamajiQuit
-------------------------
Free memory allocated for main variables search, calendar and session.
Then quit the gtk loop.
-------------------------
GtkWidget *w : widget activated.
Session *session : address of the struct Session
*/
void kamajiQuit(GtkWidget *w, gpointer data){
  Session *session = data;
  /*printf("Delete-event\n");
  printf("Search : %p\n", session->search);
  printf("Calendar : %p\n", session->calendar);*/
  if( session->search != NULL ) free(session->search);
  if( session->calendar != NULL ) free(session->calendar);
  freeDelReservations(&session->nextReservation);
  gtk_main_quit();
}


/*
-----------------------------------------------------------------------------------------------------------
Function : back
-------------------------
Back to the previous window.
-------------------------
GtkWidget *widget : widget activated.
Session *session : address of the struct Session
*/
void back(GtkWidget *widget, gpointer data){
  Session *session = data;
  gtk_widget_destroy( GTK_WIDGET( session->window ) );
  (*session->backFunction)(NULL, session);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : open_home_window
-------------------------
Open the home window, the first one. User can choose between
see all the reservations, search the rooms availables or see the planning
of a room
-------------------------
GtkWidget *widget : widget activated to open the window
Session *session : address of the struct Session
*/
void open_home_window(GtkWidget *widget, gpointer data){
  Session *session = data;
  newWindow(GLADE_FILE, "window_home", session);

  session->backFunction = open_home_window;

  click_button(session, "button_home_reservations", open_reservations_window);
  click_button(session, "button_home_search", open_new_res_window);
  click_button(session, "button_home_calendars", open_place_room_window);

  freeDelReservations(&session->nextReservation);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : click_button
-------------------------
Get a button and give it the function to call when is clicked
-------------------------
Session *session : adress of the struct Session
char *idButton : the id of the button in the xml file
void (*function) : the callback function called when the button is clicked
*/
void click_button(Session *session, char *idButton, void (*function)){
  GtkWidget *button;
  button = GTK_WIDGET(gtk_builder_get_object(session->builder, idButton));
  g_signal_connect (button,"clicked",G_CALLBACK(function),session);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : close_and_open_window
-------------------------
Close the current window present in the struct Session and open a new one
-------------------------
Session *session : address of the struct Session
char *idNewWindow : id of the new window in the xml file
*/
void close_and_open_window(Session *session, char *idNewWindow){
  gtk_widget_destroy( GTK_WIDGET( session->window ) );
  newWindow(GLADE_FILE, idNewWindow, session);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : newWindow
-------------------------
Close the current window present in the struct Session and open a new one
-------------------------
char* file : path of the xml file
char* idWindow : id of the widow in the xml file
Session *session : address of the struct Session
*/
void newWindow(char* file, char* idWindow, Session *session){
  GtkBuilder      *builder;
  GtkWidget       *window;

  builder = gtk_builder_new_from_file(file);
  window = GTK_WIDGET(gtk_builder_get_object(builder, idWindow));
  g_signal_connect(G_OBJECT(window), "delete-event", G_CALLBACK(kamajiQuit), session);

  session->builder = builder;
  session->window = GTK_WINDOW(window);

  //Background color
  background_color(window, "#444444" );

  gtk_widget_show_all(window);
}
