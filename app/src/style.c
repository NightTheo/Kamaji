/*
style.c
-------------------------
Set the style of the windows like the background color,
the fonts, the bordeers.
*/

#include "kamaji.h"


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
