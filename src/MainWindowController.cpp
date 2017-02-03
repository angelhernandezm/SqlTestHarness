//
// Created by angel on 30/01/2017.
//


#include "MainWindowController.h"

MainWindowController* MainWindowController::self;
GtkApplication* MainWindowController::application;

/**
 *
 */

void MainWindowController::OnApplicationQuit(GtkWidget* widget) {
    auto dialog = gtk_message_dialog_new(gtk_application_get_active_window (application),
                                         GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
                                         GTK_BUTTONS_YES_NO, "Are you sure you want to quit?");

    if (gtk_dialog_run((GtkDialog*) dialog) == GTK_RESPONSE_YES) {
        gtk_widget_destroy(dialog);
        g_application_quit(G_APPLICATION(application));
    } else {
        gtk_widget_destroy(dialog);
    }
}

/*
 *
 */
void MainWindowController::Initialize() {
    // This doesn't apply for this class
}

/*
 *
 */

void MainWindowController::StartUp(GtkApplication &app, gpointer user_data) {
    application = &app;
    GError *err = nullptr;
    self->builder = gtk_builder_new();
    gtk_builder_add_from_file(self->builder, "./../../ui/SqlTestHarness.glade", &err);

    if (err) {
        g_error(err->message);
        g_error_free(err);
        throw std::runtime_error("Unable to load UI. Please ensure glade file exists.");
    }

    self->SetUpUI();
    self->ConnectSignals();
    g_object_unref (G_OBJECT (self->builder));
    gtk_application_add_window(&app, (GtkWindow*) self->Controls.at("frmMain"));
    gtk_widget_show ((GtkWidget*) (GtkWindow*) self->Controls.at("frmMain"));

    self->Context = DataContext(app, ParentStub<Contact>([&](bool ignoreFields){self->Refresh(ignoreFields);},
                                                [&](bool controlState) {self->EnableOrDisableFieldsBasedOnMode(controlState);},
                                                [&](UiFieldOperation op, Contact& c){self->ReadOrWriteToFieldsOnUi(op, c);},
                                                [&] {self->FetchDataset();}));

    self->Context.CanEdit_set(false);

    self->FetchDataset();

    g_timeout_add_seconds(1, [&](gpointer data)->gboolean {self->UpdateStatusBar(data);}, self->Controls.at("sbrMain") );
}


void MainWindowController::FetchDataset() {
    g_thread_new("RetrieveRecordsThread",
                 [&](gpointer data) -> gpointer   {
                     self->Context.Rows_set(self->Context.DataAccess.RetrieveRecords().Data);
                     self->CreateView();
                     self->Refresh();
                     return nullptr;
                 }, nullptr);
}

/*
 *
 */
gboolean MainWindowController::UpdateStatusBar(gpointer data) {
    std::ostringstream oss;
    auto current = std::time(nullptr);
    auto statusBar = (_GtkStatusbar*) data;
    auto localTime = *std::localtime(&current);
    oss << "Date and Time: " <<  std::put_time(&localTime, "%d/%m/%Y  %H:%M:%S") << " - Record Count: " << Context.RowCount_get();
    auto contextId = gtk_statusbar_get_context_id(statusBar, "CurrentInformation");
    gtk_statusbar_remove_all(statusBar, contextId);
    gtk_statusbar_push(statusBar, contextId, oss.str().c_str());

    return TRUE;
}


/*
 *
 */
void MainWindowController::Refresh(bool ignoreFields) {
    auto rowcount = Context.RowCount_get();

    if (rowcount > 0) {
        // Update fields on the GtkWindow
       if (!ignoreFields) {
           if (Context.CurrentContact_get().IsEmpty())
               Context.CurrentContact_set(Context.Rows_get().at(0), [&](Contact& contact){this->DatabindFields(contact);});
           else Context.CurrentContact_set(Context.Rows_get().at(Context.RowPosition_get()),
                                           [&](Contact& contact){this->DatabindFields(contact);});
       }

        // Update GtkTreeView
        auto model = FillView();
        gtk_tree_view_set_model(GTK_TREE_VIEW(Controls.at("grdRows")), FillView());
        g_object_unref(model);
    } else {
        Contact blankContact;
        this->DatabindFields(blankContact);
        gtk_tree_view_set_model(GTK_TREE_VIEW(Controls.at("grdRows")), nullptr);
    }
}

/**
 *
 */

void MainWindowController::SetUpUI() {
    auto controlNames = {"frmMain", "pagContainer", "imagemenuitem5", "sbrMain", "grdRows",
                         "txtContactId", "txtFirstName", "txtLastName", "txtPhoneNumber",
                         "txtEmailAddress"};

    std::for_each(controlNames.begin(), controlNames.end(), [&](std::string controlName) {
          Controls.insert(std::pair<std::string, GtkWidget*>(controlName,
                                                             GTK_WIDGET (gtk_builder_get_object (builder, controlName.c_str()))));
    });

    auto dataGridPage = gtk_notebook_get_nth_page((GtkNotebook*) Controls.at("pagContainer"), 1);
    auto dataEntryPage = gtk_notebook_get_nth_page((GtkNotebook*) Controls.at("pagContainer"), 0);
    gtk_notebook_set_tab_label((GtkNotebook*) Controls.at("pagContainer"), dataEntryPage, gtk_label_new("Data Entry"));
    gtk_notebook_set_tab_label((GtkNotebook*) Controls.at("pagContainer"), dataGridPage, gtk_label_new("Rows Available"));
    toolbar = ActionToolbar(*application, builder, Context);
}

/**
 *
 */

void MainWindowController::ConnectSignals() {
    auto gridSelection = gtk_tree_view_get_selection (GTK_TREE_VIEW (Controls.at("grdRows")));
    gtk_tree_selection_set_mode (gridSelection, GTK_SELECTION_SINGLE);
    g_signal_connect (Controls.at("frmMain"), "destroy", G_CALLBACK ([=]{g_application_quit(G_APPLICATION(application));}), NULL);
    g_signal_connect(Controls.at("imagemenuitem5"), "activate", G_CALLBACK(reinterpret_cast<void(*)>(&MainWindowController::OnApplicationQuit)), NULL);
    g_signal_connect(G_OBJECT(gridSelection), "changed", G_CALLBACK(reinterpret_cast<void(*)>(&MainWindowController::OnGridSelectionChanged)), NULL);
}

MainWindowController::~MainWindowController() {
    // Nothing to dispose here
}

MainWindowController::MainWindowController() {
    self = this;
}

/*
 *
 */
void MainWindowController::OnGridSelectionChanged(GtkTreeSelection *selection, gpointer data) {
    GtkTreeIter iter;
    GtkTreeModel *model;

    auto selected = gtk_tree_view_get_selection((GtkTreeView*) self->Controls.at("grdRows"));

    if (gtk_tree_selection_get_selected(selected, &model, &iter)) {
        // Set row index in DataContext (This is for record navigation)
        auto path = gtk_tree_model_get_path(model, &iter);
        auto index = gtk_tree_path_get_indices(path)[0];
        self->Context.RowPosition_set(index);
        gtk_tree_path_free(path);

        // Set current contact based on its index
        self->Context.CurrentContact_set(self->Context.Rows_get().at(index),
                                         [&](Contact& contact){this->DatabindFields(contact);});
    }
}

/*
 *
 */
GtkTreeModel* MainWindowController::FillView() {
    GtkTreeIter iter;
    GtkTreeStore *store = nullptr;
    auto hasData = Context.RowCount_get() > 0;

    if (hasData) {
        auto colCount = (int)ColumnInformation::ColumnCount;
        store = gtk_tree_store_new (colCount, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
        for (auto&& contact : Context.Rows_get()) {
            gtk_tree_store_append (store, &iter, NULL);

             gtk_tree_store_set(store, &iter,
                                ColumnInformation::ContactId, contact.Indexer(ColumnInformation::ContactId).c_str(),
                                ColumnInformation::FirstName, contact.Indexer(ColumnInformation::FirstName).c_str(),
                                ColumnInformation::LastName, contact.Indexer(ColumnInformation::LastName).c_str(),
                                ColumnInformation::PhoneNumber, contact.Indexer(ColumnInformation::PhoneNumber).c_str(),
                                ColumnInformation::Email, contact.Indexer(ColumnInformation::Email).c_str(), -1);
        }

    }

    return  (hasData ? GTK_TREE_MODEL(store) : nullptr);
}

/*
 *
 */
void MainWindowController::DatabindFields(Contact& contact) {
    char buffer[10] = {'\0'};

    if (contact.ContactId > 0)
        sprintf(buffer, "%d", contact.ContactId);

    gtk_entry_set_text(GTK_ENTRY(self->Controls.at("txtContactId")), buffer);
    gtk_entry_set_text(GTK_ENTRY(self->Controls.at("txtEmailAddress")), contact.Email.c_str());
    gtk_entry_set_text(GTK_ENTRY(self->Controls.at("txtLastName")), contact.LastName.c_str());
    gtk_entry_set_text(GTK_ENTRY(self->Controls.at("txtFirstName")), contact.FirstName.c_str());
    gtk_entry_set_text(GTK_ENTRY(self->Controls.at("txtPhoneNumber")), contact.PhoneNumber.c_str());
}

/*
 *
 */
void MainWindowController::CreateView() {
    auto index = 0;
    std::vector<std::string> columns {"Contact Id", "First Name", "Last Name", "Phone Number", "Email" };

    auto gridCols = gtk_tree_view_get_columns(GTK_TREE_VIEW(Controls.at("grdRows")));

    if (gridCols == nullptr) {
        for (auto&& col : columns) {
            auto renderer = gtk_cell_renderer_text_new();
            auto newColumn = gtk_tree_view_column_new_with_attributes(col.c_str(), renderer, "text", index, NULL);
            gtk_tree_view_append_column(GTK_TREE_VIEW(Controls.at("grdRows")), newColumn);
            index++;
        }
    }

    g_list_free(gridCols);
}

/*
 *
 */
void MainWindowController::EnableOrDisableFieldsBasedOnMode(bool enable) {
    auto controlNames = ControlNames_get();

    // Let's handle fields first
    std::for_each(controlNames.begin(), controlNames.end(), [&](std::string name) {
        if (name != "txtContactId") {
            gtk_editable_set_editable(GTK_EDITABLE(self->Controls.at(name)), enable);
            gtk_widget_set_can_focus(self->Controls.at(name), enable);
        }
    });

    this->toolbar.EnableOrDisableButtonsBasedOnMode(enable);
}

/*
 *
 */
void MainWindowController::ReadOrWriteToFieldsOnUi(UiFieldOperation op, Contact& contact) {
    switch(op) {
        case UiFieldOperation::ReadValues:
            contact.ContactId = atoi(gtk_entry_get_text(GTK_ENTRY(self->Controls.at("txtContactId"))));
            contact.LastName =  std::string(gtk_entry_get_text(GTK_ENTRY(self->Controls.at("txtLastName"))));
            contact.Email =  std::string(gtk_entry_get_text(GTK_ENTRY(self->Controls.at("txtEmailAddress"))));
            contact.FirstName =  std::string(gtk_entry_get_text(GTK_ENTRY(self->Controls.at("txtFirstName"))));
            contact.PhoneNumber =  std::string(gtk_entry_get_text(GTK_ENTRY(self->Controls.at("txtPhoneNumber"))));
            break;

        case UiFieldOperation::SetValues:
            auto controlNames = ControlNames_get();

            // if contact is not empty is bound to UI, otherwise we'll clear fields on UI
            if (!contact.IsEmpty()) {
                DatabindFields(contact);
            } else {
                std::for_each(controlNames.begin(), controlNames.end(), [&](std::string name) {
                    gtk_entry_set_text(GTK_ENTRY(self->Controls.at(name)), "");
                });
            }
            break;
    }
}

/*
 *
 */
std::vector<std::string> MainWindowController::ControlNames_get() const {
   return {"txtContactId", "txtEmailAddress",
           "txtLastName", "txtFirstName",
           "txtPhoneNumber"};
}