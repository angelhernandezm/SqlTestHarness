//
// Created by angel on 30/01/2017.
//


#ifndef SQLTESTHARNESS_DATACONTEXT_H
#define SQLTESTHARNESS_DATACONTEXT_H

#include "CommonHeaders.h"
#include "ContactModel.h"
#include "SqlDal.h"


class DataContext {
private:
    bool canEdit;
    int RowPosition = 0;
    std::vector<Contact> Rows;
    ParentStub<Contact> Parent;
    static GtkApplication *application;
    Contact CurrentContact, PreviousContact;

public:
    DataContext();
    SqlDal DataAccess;
    int RowCount_get();
    bool CanEdit_get();
    void FetchDataSet();
    int RowPosition_get();
    void CanEdit_set(bool value);
    void RowPosition_set(int index);
    std::vector<Contact>& Rows_get();
    Contact CurrentContact_get() const;
    Contact PreviousContact_get() const;
    void PreviousContact_set(Contact contact);
    void Rows_set(std::vector<Contact> rows);
    DataContext(GtkApplication& app, ParentStub<Contact> parent);
    void GetOrSetValuesInUiThroughCallback(UiFieldOperation op, Contact& contact);
    void UpdateContextPostDbOperation(const Contact& contact, DatabaseOperation operation);
    void CurrentContact_set(Contact contact, std::function<void(Contact&)> updateUiCallback = nullptr);
    void Navigate(NavigationDirection direction, bool calledFromGrid = false, std::function<void(void)> removeItemOnDelete = nullptr);
};


#endif //SQLTESTHARNESS_DATACONTEXT_H
