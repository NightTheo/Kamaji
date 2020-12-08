/*
retreiveDataFromInput.c

retreive the date from windows inputs from glade file in ui/glade/home.glade
and return the values in a array

last modif: 
*/

#include <gtk/gtk.h>
#include "../inc/RetrieveDataFromInput.h"

void retrieveDataCBox(GtkWidget *widget,GtkComboBox *input)
{
  const char *entry;
  entry = gtk_combo_box_get_active_id(input);
  printf ("ID PLACE NEW RESERVATION: %s\n", entry);
}

