/*
insertDataGtk.c

Call the data in the database and inject them in the windows.

*/


void comboBoxTextFill( GtkComboBoxText *comboBoxText, char *firstRow, char *request ){

  MYSQL_ROW row;
  MYSQL *conn = connect_db();
  MYSQL_RES *result;

  //empty the comboBoxText
  gtk_combo_box_text_remove_all (comboBoxText);

  //Add the first : choose a room
  gtk_combo_box_text_append(comboBoxText,"0", firstRow);
  gtk_combo_box_set_active_id (GTK_COMBO_BOX(comboBoxText), "0");

  result = query(conn, request);

  while ((row = mysql_fetch_row(result)) != NULL)
    gtk_combo_box_text_append ( comboBoxText, row[0], row[1]);

  mysql_free_result(result);
  mysql_close(conn);

}
