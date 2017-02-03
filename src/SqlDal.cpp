//
// Created by angel on 30/01/2017.
//


#include "SqlDal.h"
#include "Common.h"
#include "ContactModel.h"
#include "SqlCommands.h"

/*
 *
 */
SqlDal::SqlDal() {

}

/*
 *
 */
SqlDal::~SqlDal() {

}

/*
 *
 */
Result<int> SqlDal::CreateRecord(const Contact &contact) {
    Result<int> retval;

    if (!contact.IsEmpty()) {
        auto dataAccessCode = [&] (ConnectionResult &conn){
            retval.ReturnCode = SQLExecDirect(conn.stmt, (SQLCHAR*) SqlCommands::InsertContact_get(contact).c_str(), SQL_NTS);

            if (!SQL_SUCCEEDED(retval.ReturnCode)) {
                retval.Success = false;
                retval.Exception = ExtractErrorFromDriver(conn.stmt, SQL_HANDLE_STMT);
            } else {
                SQLRETURN result;
                SQLINTEGER identity;
                SQLLEN lenContactId;
                retval.ReturnCode = SQLExecDirect(conn.stmt, (SQLCHAR*) SqlCommands::GetIdentityValue().c_str(), SQL_NTS);

                if (!SQL_SUCCEEDED(retval.ReturnCode)) {
                    retval.Success = false;
                    retval.Exception = ExtractErrorFromDriver(conn.stmt, SQL_HANDLE_STMT);
                } else {
                    SQLBindCol(conn.stmt, 1, SQL_INTEGER, &identity, ContactId_Length, &lenContactId);


                    while((result = SQLFetch(conn.stmt)) != SQL_NO_DATA) {
                        if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO) {
                            retval.Data = identity;
                            retval.Success = true;
                        }
                    }
                }
            }
        };

        Using(dataAccessCode);
    }


    return retval;
 }

/*
 *
 */
Result<int> SqlDal::DeleteRecord(const Contact &contact) {
    Result<int> retval;

    if (!contact.IsEmpty()) {
        auto dataAccessCode = [&] (ConnectionResult &conn){
            retval.ReturnCode = SQLExecDirect(conn.stmt, (SQLCHAR*) SqlCommands::DeleteContact_get(contact.ContactId).c_str(), SQL_NTS);

            if (!SQL_SUCCEEDED(retval.ReturnCode)) {
                retval.Success = false;
                retval.Exception = ExtractErrorFromDriver(conn.stmt, SQL_HANDLE_STMT);
            } else {
               retval.Success = true;
            }
        };

        Using(dataAccessCode);
    }


    return retval;
}

/*
 *
 */
Result<int> SqlDal::UpdateRecord(const Contact &contact) {
    Result<int> retval;

    if (!contact.IsEmpty()) {
        auto dataAccessCode = [&] (ConnectionResult &conn){
            retval.ReturnCode = SQLExecDirect(conn.stmt, (SQLCHAR*) SqlCommands::UpdateContact_get(contact).c_str(), SQL_NTS);

            if (!SQL_SUCCEEDED(retval.ReturnCode)) {
                retval.Success = false;
                retval.Exception = ExtractErrorFromDriver(conn.stmt, SQL_HANDLE_STMT);
            } else {
                retval.Success = true;
            }
        };

        Using(dataAccessCode);
    }

    return retval;
}

/*
 *
 */
Result<std::vector<Contact>>  SqlDal::RetrieveRecords() {
    Result<std::vector<Contact>> retval;

    auto dataAccessCode = [&] (ConnectionResult &conn){
        SQLRETURN result;
        Contact newRecord;
        SQLLEN lenContactId, lenFirstName, lenLastName, lenPhoneNumber, lenEmail;
        retval.ReturnCode = SQLExecDirect(conn.stmt, (SQLCHAR*) SqlCommands::RetrieveContacts_get().c_str(), SQL_NTS);

        if (!SQL_SUCCEEDED(retval.ReturnCode)) {
            retval.Success = false;
            retval.Exception = ExtractErrorFromDriver(conn.stmt, SQL_HANDLE_STMT);
        } else {
            SQLBindCol(conn.stmt, 1, SQL_INTEGER, &newRecord.ODBCFields.ContactId, ContactId_Length, &lenContactId);
            SQLBindCol(conn.stmt, 2, SQL_C_CHAR, &newRecord.ODBCFields.FirstName, FirstName_Length, &lenFirstName);
            SQLBindCol(conn.stmt, 3, SQL_C_CHAR, &newRecord.ODBCFields.LastName, LastName_Length, &lenLastName);
            SQLBindCol(conn.stmt, 4, SQL_C_CHAR, &newRecord.ODBCFields.PhoneNumber, PhoneNumber_Length, &lenPhoneNumber);
            SQLBindCol(conn.stmt, 5, SQL_C_CHAR, &newRecord.ODBCFields.Email, Email_Length, &lenEmail);

            while((result = SQLFetch(conn.stmt)) != SQL_NO_DATA) {
                if (result == SQL_SUCCESS || result == SQL_SUCCESS_WITH_INFO)
                    retval.Data.push_back(Contact(newRecord.ODBCFields));
            }
            retval.Success = true;
        }
    };

    Using(dataAccessCode);

    return retval;
}

/*
 *
 */
void SqlDal::ReleaseConnectionResources(ConnectionResult &conn) {
    if (conn.Success())
        SQLDisconnect(conn.dbc);

    SQLFreeHandle(SQL_HANDLE_STMT, conn.stmt);
    SQLFreeHandle(SQL_HANDLE_DBC, conn.dbc);
    SQLFreeHandle(SQL_HANDLE_ENV, conn.env);
}

/*
 *
 */
ConnectionResult SqlDal::GetConnection(std::string connString) {
    ConnectionResult ret;

    if (connString.size() > 0) {
        try {
             SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &ret.env);
             SQLSetEnvAttr(ret.env, SQL_ATTR_ODBC_VERSION, (void *) SQL_OV_ODBC3, 0);
             SQLAllocHandle(SQL_HANDLE_DBC, ret.env, &ret.dbc);

             ret.retval = SQLDriverConnect(ret.dbc, NULL,  (SQLCHAR*) connString.c_str(),
                                          SQL_NTS, ret.outputBuffer, sizeof(ret.outputBuffer),
                                          &ret.outputBufferLength, SQL_DRIVER_COMPLETE);

             SQLAllocHandle(SQL_HANDLE_STMT, ret.dbc,  &ret.stmt);

            if (!SQL_SUCCEEDED(ret.retval)) {
                ret.Exception = ExtractErrorFromDriver(ret.dbc, SQL_HANDLE_DBC);
                ReleaseConnectionResources(ret);
            }
        } catch(std::exception &ex) {
            ret.Exception = ex.what();
        }
    }
    return ret;
}


/*
 *
 */
std::string SqlDal::ConnectionString_get() {
    return ConnectionString;
}

/*
 *
 */
void SqlDal::ConnectionString_set(std::string connStr) {
    ConnectionString = connStr;
}

/*
 *
 */
std::string SqlDal::ExtractErrorFromDriver(SQLHANDLE handle, SQLSMALLINT type) {
    SQLRETURN ret;
    SQLSMALLINT length;
    std::string retval;
    SQLINTEGER i = 0, native;
    SQLCHAR state[7], description[256];

    do {
        ret = SQLGetDiagRec(type, handle, ++i, state, &native, description, sizeof(description), &length);

        if (SQL_SUCCEEDED(ret))
            retval.append((char*)description);

    } while(ret == SQL_SUCCESS);

    return retval;
}