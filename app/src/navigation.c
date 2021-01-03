/*
import_glade.c

create the windows from the file glade in ui/glade/home.glade
connet the signals for the navigation between the windows

*/
#include "kamaji.h"

//defines


// NAVIGATION
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





// STYLE
/*
-----------------------------------------------------------------------------------------------------------
Function : background_color
-------------------------
Change the back ground color of a widget
-------------------------
GtkWidget *widget : the widget to change
char *color : color in hexadecimal like #RRGGBB
*/
void background_color( GtkWidget *widget, char *color ){
  GtkCssProvider * cssProvider = gtk_css_provider_new();    //store the css

  char css[64] = "* { background-image:none; background-color:";
  strcat( strcat( css , color ), ";}" );

  gtk_css_provider_load_from_data(cssProvider, css,-1,NULL);
  GtkStyleContext * context = gtk_widget_get_style_context(widget);   //manage CSS provider
  gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(cssProvider),GTK_STYLE_PROVIDER_PRIORITY_USER);
}


/*
-----------------------------------------------------------------------------------------------------------
Function : stylePlanningRoom
-------------------------
Change the style of the planning window.
-------------------------
Session *session : address of the struct Session
*/
void stylePlanningRoom(Session *session){
  GtkWidget *planningContainer;
  GtkWidget *locationContainer;

  planningContainer = GTK_WIDGET( gtk_builder_get_object(session->builder, "box_planning_planning") );
  locationContainer = GTK_WIDGET( gtk_builder_get_object(session->builder, "box_planning_where") );

  background_color(planningContainer, "#ffffff" );
  background_color(locationContainer, "#ffffff" );
}


/*
-----------------------------------------------------------------------------------------------------------
Function : background_color_if_sensitive
-------------------------
if the widget is clickable then call the function background_color to the color of the widget.
-------------------------
GtkWidget *widget: widget to change.
char *color: string of the code color in hexadecimal like "#RRGGBB"
*/
void background_color_if_sensitive(GtkWidget *widget, char* color){
  if( (int)gtk_widget_is_sensitive (widget) )
    background_color( widget, color );
}

// ----------------------


// ----------------------





















//#############
