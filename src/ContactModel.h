//
// Created by angel on 30/01/2017.
//



#ifndef SQLTESTHARNESS_CONTACTMODEL_H
#define SQLTESTHARNESS_CONTACTMODEL_H

#include <string.h>
#include <iostream>
#include "Common.h"


#define Email_Length 30
#define ContactId_Length 4
#define LastName_Length 30
#define FirstName_Length 30
#define PhoneNumber_Length 30

typedef struct {
    SQLINTEGER ContactId;
    SQLCHAR FirstName[FirstName_Length];
    SQLCHAR LastName[LastName_Length];
    SQLCHAR PhoneNumber[PhoneNumber_Length];
    SQLCHAR Email[Email_Length];
} ODBCMapping;

class Contact {
public:
    int ContactId;
    std::string FirstName;
    std::string LastName;
    std::string PhoneNumber;
    std::string Email;
    ODBCMapping ODBCFields;

    Contact() {
        ContactId = 0;
    }

    /*
     *
     */
    Contact(ODBCMapping &mapping) {
        ODBCFields = mapping;
        ContactId = mapping.ContactId;
        FirstName = std::string((char*) mapping.FirstName);
        LastName = std::string((char*) mapping.LastName);
        PhoneNumber = std::string((char*) mapping.PhoneNumber);
        Email = std::string((char*) mapping.Email);
    }

    /*
     *
     */
    bool IsEmpty() const {
       return (FirstName.size() == 0 || LastName.size() == 0 || Email.size() == 0 || PhoneNumber.size() == 0);
    }

    /*
     *
     */
    void SetFieldByIndex(void* value, int index) {
        if (value != nullptr) {
            switch(index) {
                case ColumnInformation::ContactId:
                    ContactId = atoi((char*) value);
                    break;
                case ColumnInformation::FirstName:
                    FirstName = std::string((char*) value);
                    break;
                case ColumnInformation::LastName:
                    LastName = std::string((char*) value);
                    break;
                case ColumnInformation::Email:
                    Email = std::string((char*) value);
                    break;
                case ColumnInformation::PhoneNumber:
                    PhoneNumber = std::string((char*) value);
                    break;
            }
        }
    }

    /*
    *
    */
    std::string Indexer(ColumnInformation column) {
        std::string retval;

        switch(column) {
            case ColumnInformation::ContactId:
                char buffer[10];
                sprintf(buffer, "%d", ContactId);
                retval.append(buffer);
                break;
            case ColumnInformation::FirstName:
                retval = FirstName;
                break;
            case ColumnInformation::LastName:
                retval = LastName;
                break;
            case ColumnInformation::Email:
                retval = Email;
                break;
            case ColumnInformation::PhoneNumber:
                retval = PhoneNumber;
                break;
        }

        return retval;
    }
};

#endif //SQLTESTHARNESS_CONTACTMODEL_H
