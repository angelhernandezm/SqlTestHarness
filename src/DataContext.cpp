//
// Created by angel on 30/01/2017.
//

#include "DataContext.h"

GtkApplication*::DataContext::application;

/*
 *
 */
int DataContext::RowCount_get() {
    return (int) Rows.size();
}

/*
 *
 */
void DataContext::Navigate(NavigationDirection direction, bool calledFromGrid, std::function<void(void)> removeItemOnDelete ) {
    auto rowCount = Rows.size();

    if (rowCount > 0) {
        auto currentPos = RowPosition_get();

        // This code only runs when deleting a record. Otherwise, grid databinding
        // won't reflect the changes in the UI when calling Parent.NotifyParent() below
        if (removeItemOnDelete != nullptr)
            removeItemOnDelete();

        switch(direction) {
            case NavigationDirection::First:
                RowPosition_set(0);
                break;
            case NavigationDirection::Previous:
                if (currentPos > 0)
                    RowPosition_set(currentPos - 1);
                else RowPosition_set(0);
                break;
            case NavigationDirection::Next:
                if (currentPos < RowCount_get() - 1)
                    RowPosition_set(currentPos + 1);
                break;
            case NavigationDirection::Last:
                RowPosition_set(rowCount - 1);
                break;
        }
    } else if (!calledFromGrid) {
        auto dialog = gtk_message_dialog_new(gtk_application_get_active_window (application),
                                             GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
                                             GTK_BUTTONS_OK, "No records found");

      if (gtk_dialog_run((GtkDialog*) dialog) == GTK_RESPONSE_OK)
          gtk_widget_destroy(dialog);
    }
    Parent.NotifyParent(false);
}

/*
 *
 */
void DataContext::Rows_set(std::vector<Contact> rows) {
    Rows = rows;
}

/*
 *
 */
void DataContext::FetchDataSet() {
    Parent.FetchData();
}


/*
 *
 */
std::vector<Contact>& DataContext::Rows_get()  {
    return Rows;
}

void DataContext::GetOrSetValuesInUiThroughCallback(UiFieldOperation op, Contact &contact) {
    Parent.ParentUiOperation(op, contact);
}

/*
 *
 */
DataContext::DataContext() {

}

/*
 *
 */
void DataContext::CanEdit_set(bool value) {
    if (canEdit != value) {
        canEdit = value;
        Parent.EnableFieldsinParent(value);
    }
}

/*
 * 
 */
bool DataContext::CanEdit_get() {
    return canEdit;
}

/*
 *
 */
Contact DataContext::CurrentContact_get() const {
    return CurrentContact;
}

/*
 *
 */
void DataContext::CurrentContact_set(Contact contact, std::function<void(Contact&)> updateUiCallback) {
        CurrentContact = contact;

        // is it required to do anything to the UI?
        if (updateUiCallback != nullptr)
            updateUiCallback(CurrentContact);
}

/*
 *
 */
int DataContext::RowPosition_get() {
    return RowPosition;
}

/*
 *
 */
void DataContext::RowPosition_set(int index) {
    RowPosition = index;
}

/*
 *
 */
Contact DataContext::PreviousContact_get() const {
    return PreviousContact;
}

/*
 *
 */
void DataContext::PreviousContact_set(Contact contact) {
    PreviousContact = contact;
}

/*
 *
 */

void DataContext::UpdateContextPostDbOperation(const Contact& contact, DatabaseOperation operation) {
    if (!contact.IsEmpty()) {
        auto index = 0;
        auto found = std::find_if(Rows_get().begin(), Rows_get().end(), [&](Contact &c) {
            index++;
            return (c.ContactId == contact.ContactId);
        });

        if (operation == DatabaseOperation::Create) {
            Navigate(NavigationDirection::Last);
        } else if (operation == DatabaseOperation::Delete) {
            if (found != Rows_get().end())
                Navigate(NavigationDirection::Previous, true, [&]{Rows_get().erase(found);});
        } else if (operation == DatabaseOperation::Update) {
            if (found != Rows_get().end()) {
                index--;
                Rows_get().at(index).Email = contact.Email;
                Rows_get().at(index).LastName = contact.LastName;
                Rows_get().at(index).FirstName = contact.FirstName;
                Rows_get().at(index).PhoneNumber = contact.PhoneNumber;
            }
            Parent.NotifyParent(true);
        }
    }
}

/*
 *
 */
DataContext::DataContext(GtkApplication& app, ParentStub<Contact> parent) {
    application = &app;
    Parent = parent;
    DataAccess.ConnectionString_set("DSN=Contacts;UID=sa;PWD=p@ssw0rd;DATABASE=GtkDemo;");
}