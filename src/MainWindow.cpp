//
// Created by angel on 30/01/2017.
//


#include <iostream>
#include <gtk/gtk.h>
#include "MainWindowController.h"

using namespace std;

GtkApplication *app;
MainWindowController mainWindow;

/**
 *
 */

int main(int argc, char **argv) {
   app = gtk_application_new("Gtk.SqlTestHarness.Demo", G_APPLICATION_FLAGS_NONE);
   g_signal_connect (app, "activate", G_CALLBACK (&MainWindowController::StartUp), NULL);
   g_application_run (G_APPLICATION (app), argc, argv);
   g_object_unref (app);

   return 0;
}