/*
retreiveDataFromInput.c

retreive the date from windows inputs from glade file in ui/glade/home.glade
and return the values in a array

last modif: 
*/

#include <gtk/gtk.h>
#include "../inc/RetrieveDataFromInput.h"

char *retrieveDataCBox(GtkWidget *widget,GtkComboBox *input)
{
  const char *entry;
  entry = gtk_combo_box_get_active_id(input);
  printf ("ID NEW RESERVATION WINDOWS: %s\n", entry);

  //return entry;
}

int retrieveDataSpin(GtkWidget *widget,GtkSpinButton *input)
{
  int entry;
  entry = gtk_spin_button_get_value_as_int(input);
  printf ("ID NEW RESERVATION WINDOWS NB PERSONNE: %d\n", entry);

  return entry;
}

void retrieveDataCalendar(GtkWidget *widget,GtkCalendar *input)
{
  printf("OK");
  guint year,month,day;

  gtk_calendar_get_date(input,&year,&month,&day);
  printf ("CALENDAR: %d-%d-%d\n", (int)year,(int)month,(int)day);
}
