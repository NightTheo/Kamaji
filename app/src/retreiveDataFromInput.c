/*
retreiveDataFromInput.c

retreive the data from windows inputs from glade file in ui/glade/home.glade
and return the values in a array

*/


char *retrieveDataCBoxText(GtkComboBoxText *input)
{
  char *entry;
  entry = (char*)gtk_combo_box_get_active_id( GTK_COMBO_BOX(input) );
  //printf ("ID NEW RESERVATION WINDOWS: %s\n", entry);

  return entry;
}

int retrieveDataSpin(GtkSpinButton *input)
{
  int entry;
  entry = gtk_spin_button_get_value_as_int(input);
  printf ("ID NEW RESERVATION WINDOWS NB PERSONNE: %d\n", entry);

  return entry;
}

void retrieveDataCalendar(GtkCalendar *input)
{
  guint year,month,day;

  gtk_calendar_get_date(input,&year,&month,&day);
  printf ("CALENDAR: %d-%d-%d\n", (int)year,(int)month,(int)day);
}
