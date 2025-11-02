#include <iostream>
#include <string>
#include "bank1.h"
#include <sqlite3.h>

int* b1_credit_1_svc(char* account, int amount, struct svc_req *rqstp){
    std::cout << "\also here";
    static int result;
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;
    int balance;
    //open bank1.db
     rc = sqlite3_open("bank1.db", &db);

    //find balance based on account in bank1.db
    rc = sqlite3_prepare_v2(db, "SELECT balance FROM accounts WHERE account_id = ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, account, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        balance = sqlite3_column_int(stmt, 0);
    } else {
        //account not found
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        result = 1;
        return &result;
    }
    sqlite3_finalize(stmt);


    //if not, add amount from balance and update the table with correct balance
    // Update balance
    rc = sqlite3_prepare_v2(db, "UPDATE accounts SET balance = ? WHERE account_id = ?", -1, &stmt, NULL);

    sqlite3_bind_int(stmt, 1, balance + amount);
    sqlite3_bind_text(stmt, 2, account, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);

    //cleanup
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    result = 0;
    return &result;

}

int* b1_debit_1_svc(char* account, int amount, struct svc_req *rqstp){
    static int result;
    sqlite3* db;
    sqlite3_stmt* stmt;
    sqlite3_stmt* stmt2;
    int rc;
    int balance;
    //open bank1.db
     rc = sqlite3_open("bank1.db", &db);

    //find balance based on account in bank1.db
    rc = sqlite3_prepare_v2(db, "SELECT balance FROM accounts WHERE account_id = ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, account, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        balance = sqlite3_column_int(stmt, 0);
    } else {
        //account not found
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        result = 1;
        return &result;
    }

    //check whether amount exceeds balance
     if (amount > balance) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        result = 2;
        return &result;
    }

    // Update balance
    rc = sqlite3_prepare_v2(db, "UPDATE accounts SET balance = ? WHERE account_id = ?", -1, &stmt2, NULL);

    sqlite3_bind_int(stmt2, 1, balance - amount);
    sqlite3_bind_text(stmt2, 2, account, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt2);

    //cleanup
    sqlite3_finalize(stmt);
    sqlite3_finalize(stmt2);
    sqlite3_close(db);
    result = 0;
    return &result;
}
