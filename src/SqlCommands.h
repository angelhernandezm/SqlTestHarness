//
// Created by angel on 30/01/2017.
//


#ifndef SQLTESTHARNESS_SQLCOMMANDS_H
#define SQLTESTHARNESS_SQLCOMMANDS_H

#include "ContactModel.h"

class SqlCommands{
public:
    /*
     *
     */
    static const std::string RetrieveContacts_get() {
      return "Select * from Contact";
    };

    /*
     *
     */
    static const std::string InsertContact_get(const Contact &contact) {
        char buffer[512];
        sprintf(buffer, "Insert into Contact Values ('%s', '%s', '%s', '%s')", contact.FirstName.c_str(),
                contact.LastName.c_str(), contact.PhoneNumber.c_str(), contact.Email.c_str());

        return std::string(buffer);
    }

    /*
     *
     */
    static const std::string UpdateContact_get(const Contact &contact) {
        char buffer[512];
        std::string updateStmt = "Update Contact Set FirstName = '%s', LastName = '%s', PhoneNumber = '%s', Email = '%s' "
                                 "Where Contact_Id = %d";

        sprintf(buffer, updateStmt.c_str(), contact.FirstName.c_str(),
                contact.LastName.c_str(), contact.PhoneNumber.c_str(),
                contact.Email.c_str(), contact.ContactId);

        return std::string(buffer);
    }

    /*
     *
     */
    static const std::string DeleteContact_get(int contactId) {
       char buffer[50];
       sprintf(buffer, "Delete from Contact where Contact_Id = %d", contactId);

       return std::string(buffer);
    }

    /*
     *
     */
    static const std::string SelectContactById_get(int contactId) {
        char buffer[50];
        sprintf(buffer, "Select * from Contact where Contact_Id = %d", contactId);

        return std::string(buffer);
    }

    /*
     *
     */
    static const std::string GetIdentityValue() {
        return "Select LastId=IDENT_CURRENT('Contact')";
    }
};

#endif //SQLTESTHARNESS_SQLCOMMANDS_H
