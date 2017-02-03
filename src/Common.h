//
// Created by angel on 30/01/2017.
//


#ifndef SQLTESTHARNESS_COMMON_H
#define SQLTESTHARNESS_COMMON_H

#include <sqltypes.h>
#include <sql.h>

typedef enum  {First, Previous, Next, Last} NavigationDirection;

typedef enum {ContactId = 0, FirstName, LastName, PhoneNumber, Email, ColumnCount} ColumnInformation;

typedef enum {ReadValues, SetValues} UiFieldOperation;

typedef enum {Create, Retrieve, Update, Delete} DatabaseOperation;

template <typename T>
class Result {
public:
    T Data;
    void* Tag = nullptr;
    int ReturnCode = -1;
    bool Success = false;
    std::string Exception;
};

typedef struct  {
    SQLHDBC dbc;
    SQLHENV env;
    SQLHSTMT stmt;
    SQLRETURN retval;
    std::string Exception;
    SQLCHAR outputBuffer[1024];
    SQLSMALLINT outputBufferLength;

    bool Success() {
        return SQL_SUCCEEDED(retval);
    }
} ConnectionResult;

template <typename T>
class ParentStub {
public:
    std::function<void(void)> FetchData;
    std::function<void(bool)> NotifyParent;
    std::function<void(bool)> EnableFieldsinParent;
    std::function<void(UiFieldOperation op, T& contact)> ParentUiOperation;

    ParentStub() {    }

    ParentStub(std::function<void(bool)> notifyParent,
               std::function<void(bool)> enableFieldsinParent,
               std::function<void(UiFieldOperation op, T& contact)> parentUiOperation,
               std::function<void(void)> fetchData) {
        FetchData = fetchData;
        NotifyParent = notifyParent;
        ParentUiOperation = parentUiOperation;
        EnableFieldsinParent = enableFieldsinParent;
    }
};

#endif //SQLTESTHARNESS_COMMON_H
