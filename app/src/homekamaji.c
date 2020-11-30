#include <gtk/gtk.h>

// called when window is closed
void on_window_main_destroy()
{
    gtk_main_quit();
}

void searchkamaji();

void homekamaji(int argcounter, char **argvalue){
    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *button;

    gtk_init(&argcounter, &argvalue);

    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder,"glade/home.glade", NULL);

    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_home"));
    gtk_builder_connect_signals(builder, NULL);
    
    button = GTK_WIDGET(gtk_builder_get_object(builder, "button_home_search"));
    g_signal_connect (button,"clicked",G_CALLBACK(searchkamaji),window);


    g_object_unref(builder);

    gtk_widget_show(window);                
    gtk_main();

}

void searchkamaji(GtkWidget *Widget,GtkWidget *window){
    GtkBuilder      *builder;
    
    gtk_widget_destroy(window);
    builder = gtk_builder_new();
    gtk_builder_add_from_file (builder,"glade/home.glade", NULL);
    window = GTK_WIDGET(gtk_builder_get_object(builder, "window_new_reservation"));
    gtk_builder_connect_signals(builder, NULL);
    gtk_widget_show(window); 
}

int main(int argc, char **argv)
{
    homekamaji(argc,argv);
    return 0;
}