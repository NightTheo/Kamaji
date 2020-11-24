/*
08/11/2020

compile:
gcc `pkg-config --cflags gtk+-3.0` -o sql_gtk sql_gtk.c `pkg-config --libs gtk+-3.0` -l mysqlclient
*/

#include <mysql/mysql.h>
#include <gtk/gtk.h>


static void print_hello (GtkWidget *widget, gpointer   data);
static void activate (GtkApplication *app, gpointer user_data);


int main(int argc, char **argv){

  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;

  char *server = "localhost";
  char *user = "root";
  char *password = "";
  char *database = "test_libc";

  conn = mysql_init(NULL);

  /* Connect to database */
  if (!mysql_real_connect(conn, server,
      user, password, database, 8889, NULL, 0)) {
     fprintf(stderr, "%s\n", mysql_error(conn));
     exit(1);
  }

  /* send SQL query */
  if (mysql_query(conn, "show tables")) {
     fprintf(stderr, "%s\n", mysql_error(conn));
     exit(1);
  }

  res = mysql_use_result(conn);

  /* output table name */
  printf("MySQL Tables in mysql database:\n");
  while ((row = mysql_fetch_row(res)) != NULL)
     printf("%s \n", row[0]);

  /* close connection */
  mysql_free_result(res);
  mysql_close(conn);

  /* GTK app */
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}

/*END MAIN___________________________________________*/

static void print_hello (GtkWidget *widget, gpointer   data){
  g_print ("Hello World\n");
}

static void activate (GtkApplication *app, gpointer user_data){
  GtkWidget *window;
  GtkWidget *button;
  GtkWidget *button_box;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  button_box = gtk_button_box_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_container_add (GTK_CONTAINER (window), button_box);

  button = gtk_button_new_with_label ("Hello World");
  g_signal_connect (button, "clicked", G_CALLBACK (print_hello), NULL);
  g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_window_close), window);
  gtk_container_add (GTK_CONTAINER (button_box), button);

  gtk_widget_show_all (window);
}
