/*
navigation.h
*/




Session *initSession();
void kamajiQuit(GtkWidget *w, gpointer data);
void back(GtkWidget *widget, gpointer data);
void open_home_window(GtkWidget *widget, gpointer data);
void click_button(Session *session, char *idButton,void (*function));
void close_and_open_window(Session *session, char *idNewWindow);
void newWindow(char* file, char* idWindow, Session *session);


//STYLE
void background_color( GtkWidget *widget, char *color );
void stylePlanningRoom(Session *session);
void background_color_if_sensitive(GtkWidget *widget, char* color);



//
