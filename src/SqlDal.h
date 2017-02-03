//
// Created by angel on 30/01/2017.
//


#ifndef SQLTESTHARNESS_SQLDAL_H
#define SQLTESTHARNESS_SQLDAL_H

#include "CommonHeaders.h"
#include "ContactModel.h"
#include <sql.h>
#include <sqlext.h>


class SqlDal {
private:
    std::string ConnectionString;
    void ReleaseConnectionResources(ConnectionResult &conn);
    std::string ExtractErrorFromDriver(SQLHANDLE handle, SQLSMALLINT type);

protected:
    ConnectionResult GetConnection(std::string connString);

    template <class Func>
     void Using(Func&& dataAccessCode) {
        auto conn = GetConnection(ConnectionString);
        dataAccessCode(conn);
        ReleaseConnectionResources(conn);
    }

public:
    SqlDal();
    ~SqlDal();
    std::string ConnectionString_get();
    void ConnectionString_set(std::string connStr);
    Result<std::vector<Contact>> RetrieveRecords();
    Result<int> CreateRecord(const Contact &contact);
    Result<int> DeleteRecord(const Contact &contact);
    Result<int> UpdateRecord(const Contact &contact);
};


#endif //SQLTESTHARNESS_SQLDAL_H
