//
// Created by angel on 30/01/2017.
//

#include "ActionToolbar.h"

DataContext*::ActionToolbar::Context;
GtkApplication*::ActionToolbar::application;


ActionToolbar::ActionToolbar() {

}

/*
 *
 */
ActionToolbar::ActionToolbar(GtkApplication &app, const GtkBuilder *builder, DataContext &context) {
    application = &app;
    Context = &context;

   Initialize(const_cast<GtkBuilder*>(builder));
}

/*
 *
 */
GtkWindow* ActionToolbar::AppWindow_get() {
    return gtk_application_get_active_window (application);
}

/*
 *
 */
void ActionToolbar::Initialize(GtkBuilder *builder) {
    std::map<std::string, GCallback> eventHandlers;

    auto buttonNames = ButtonNames_get();

    eventHandlers.insert(std::pair<std::string, GCallback>("btnNew", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnNewRecord))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnRefresh", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnRefresh))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnSave", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnSaveRecord))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnEdit", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnEditRecord))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnCancel", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnCancelEdit))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnNext", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnNavigateNext))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnLast", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnNavigateLast))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnFirst", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnNavigateFirst))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnDelete", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnDeleteRecord))));
    eventHandlers.insert(std::pair<std::string, GCallback>("btnPrevious", G_CALLBACK(reinterpret_cast<void(*)>(&ActionToolbar::OnNavigatePrevious))));


    std::for_each(buttonNames.begin(), buttonNames.end(), [&] (std::string name) {
        Buttons.insert(std::pair<std::string, GtkWidget*>(name, GTK_WIDGET (gtk_builder_get_object (builder, name.c_str()))));
        g_signal_connect(Buttons.at(name), "clicked",  (GCallback) eventHandlers.at(name), NULL);
    });
}

void ActionToolbar::OnEditRecord() {
   if (!Context->CurrentContact_get().IsEmpty() && !Context->CanEdit_get()) {
       Context->PreviousContact_set(Context->CurrentContact_get());
       Context->CanEdit_set(true);
   }
}

/*
 *
 */
void ActionToolbar::OnNewRecord() {
    if (!Context->CanEdit_get()) {
        Contact newRecord;
        Context->CanEdit_set(true);
        Context->PreviousContact_set(Context->CurrentContact_get());
        Context->CurrentContact_set(newRecord, [&](Contact& c) {
            Context->GetOrSetValuesInUiThroughCallback(UiFieldOperation::SetValues, c);});
    }
}

/*
 *
 */
void ActionToolbar::OnSaveRecord() {
    Contact contact;
    Result<int> ret;

    if (Context->CanEdit_get()) {
        Context->GetOrSetValuesInUiThroughCallback(UiFieldOperation::ReadValues, contact);
        // Insert or delete record depending on the case
        if (Context->CurrentContact_get().IsEmpty()) {
            if ((ret = Context->DataAccess.CreateRecord(contact)).Success) {
                contact.ContactId = ret.Data;
                Context->Rows_get().push_back(contact);
                Context->CurrentContact_set(contact);
                Context->RowPosition_set(Context->Rows_get().size() - 1);
                Context->UpdateContextPostDbOperation(contact, DatabaseOperation::Create);
            }
        } else {
            if ((ret = Context->DataAccess.UpdateRecord(contact)).Success) {
                Context->CurrentContact_set(contact);
                Context->UpdateContextPostDbOperation(contact, DatabaseOperation::Update);
            }
        }
    }
    Context->CanEdit_set(false);
}

/*
 *
 */
void ActionToolbar::OnCancelEdit() {
    if (Context->CanEdit_get()) {
        auto dialog = gtk_message_dialog_new(AppWindow_get(), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO, "Cancel edit?");

        if (gtk_dialog_run((GtkDialog*) dialog) == GTK_RESPONSE_YES) {
            Context->CurrentContact_set(Context->PreviousContact_get(), [&](Contact& c) {
                Context->GetOrSetValuesInUiThroughCallback(UiFieldOperation::SetValues, c);});
            Context->CanEdit_set(false);
        }

        gtk_widget_destroy(dialog);
    }
}

/*
 *
 */
void ActionToolbar::OnNavigateNext() {
    if (!Context->CanEdit_get())
        Context->Navigate(NavigationDirection::Next);
}

/*
 *
 */
void ActionToolbar::OnNavigateLast() {
    if (!Context->CanEdit_get())
        Context->Navigate(NavigationDirection::Last);
}

/*
 *
 */
void ActionToolbar::OnDeleteRecord() {
    Contact selected;

    if (!Context->CanEdit_get() && ((selected = Context->CurrentContact_get()).ContactId > 0)
            && Context->RowCount_get() > 0) {
        auto dialog = gtk_message_dialog_new(AppWindow_get(), GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
                                             GTK_BUTTONS_YES_NO, "Delete record?");

        if (gtk_dialog_run((GtkDialog*) dialog) == GTK_RESPONSE_YES) {
            if (Context->DataAccess.DeleteRecord(selected).Success) {
               Context->UpdateContextPostDbOperation(selected, DatabaseOperation::Delete);
            }
        }
        gtk_widget_destroy(dialog);
    }
}

/*
 *
 */
void ActionToolbar::OnNavigateFirst() {
    if (!Context->CanEdit_get())
        Context->Navigate(NavigationDirection::First);

}

/*
 *
 */
void ActionToolbar::OnNavigatePrevious() {
    if (!Context->CanEdit_get())
        Context->Navigate(NavigationDirection::Previous);
}


void ActionToolbar::OnRefresh() {
    if (!Context->CanEdit_get()) {
        Context->FetchDataSet();
    }
}

/*
 *
 */
void ActionToolbar::EnableOrDisableButtonsBasedOnMode(bool enable) {
    gtk_widget_set_sensitive(Buttons.at("btnSave"), enable);
    gtk_widget_set_sensitive(Buttons.at("btnNew"), !enable);
    gtk_widget_set_sensitive(Buttons.at("btnEdit"), !enable);
    gtk_widget_set_sensitive(Buttons.at("btnNext"), !enable);
    gtk_widget_set_sensitive(Buttons.at("btnLast"), !enable);
    gtk_widget_set_sensitive(Buttons.at("btnCancel"), enable);
    gtk_widget_set_sensitive(Buttons.at("btnFirst"), !enable);
    gtk_widget_set_sensitive(Buttons.at("btnDelete"), !enable);
    gtk_widget_set_sensitive(Buttons.at("btnRefresh"), !enable);
    gtk_widget_set_sensitive(Buttons.at("btnPrevious"), !enable);
}

std::vector<std::string> ActionToolbar::ButtonNames_get() const {
    return  {"btnNew", "btnNext", "btnLast", "btnSave",
                          "btnFirst", "btnDelete", "btnCancel",
                          "btnPrevious", "btnEdit", "btnRefresh"};
}