#include <iostream>
#include <string>
#include "transactions.h"
#include "bank1.h"
#include "bank2.h"
#include <sqlite3.h>

#define server_name "localhost"


int* vb_credit_1_svc(char* account, int amount, struct svc_req *rqstp) {
    static int result;
    int* resultFromBank;
    CLIENT* clnt;
    sqlite3* db;
    int rc;
    sqlite3_stmt* stmt;
    const char* bank_name;

    // Open database
    rc = sqlite3_open("virtualbank.db", &db);
    //query the database to get the bankname from our account map table.
    rc = sqlite3_prepare_v2(db, "SELECT bank_name FROM account_map WHERE account_id = ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, account, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        bank_name = (const char*)sqlite3_column_text(stmt, 0);
    } else {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        result = 1;
        return &result;
    }

    // Choose bank and create client
    if (strcmp(bank_name, "BANK1") == 0) {
        clnt = clnt_create(server_name, BANK1, BANK1_VERS, "netpath");
        if (clnt == NULL) {
            clnt_pcreateerror(server_name);
            result = 1;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return &result;
        }
        //call credit with bank1
        resultFromBank = b1_credit_1(account, amount, clnt);
    } else if (strcmp(bank_name, "BANK2") == 0) {
        clnt = clnt_create(server_name, BANK2, BANK2_VERS, "netpath");
        if (clnt == NULL) {
            clnt_pcreateerror(server_name);
            result = 1;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return &result;
        }
        //call credit with bank2
        resultFromBank = b2_credit_1(account, amount, clnt);
    } else {
        //bank name corruption possibly
        result = 1;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return &result;
    }

    //return whatever we got from bank calls
    clnt_destroy(clnt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    result = *resultFromBank;
    return &result;
}

int* vb_debit_1_svc(char* account, int amount, struct svc_req *rqstp){
    static int result;
    int *resultFromBank;
    CLIENT *clnt;
    sqlite3* db;
    int rc;
    sqlite3_stmt *stmt;
    const char *bank_name;

    //open database
    rc = sqlite3_open("virtualbank.db", &db);
    //find bank name from account map
    rc = sqlite3_prepare_v2(db, "SELECT bank_name FROM account_map WHERE account_id = ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, account, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        bank_name = (const char*)sqlite3_column_text(stmt, 0);
    } else {
        //account not found
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        result = 1;
        return &result;
    }

    // Choose bank and create client
    if (strcmp(bank_name, "BANK1") == 0) {
        clnt = clnt_create(server_name, BANK1, BANK1_VERS, "netpath");
        if (clnt == NULL) {
            clnt_pcreateerror(server_name);
            result = 1;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return &result;
        }
        resultFromBank = b1_debit_1(account, amount, clnt);
    } else if (strcmp(bank_name, "BANK2") == 0) {
        clnt = clnt_create(server_name, BANK2, BANK2_VERS, "netpath");
        if (clnt == NULL) {
            clnt_pcreateerror(server_name);
            result = 1;
            sqlite3_finalize(stmt);
            sqlite3_close(db);
            return &result;
        }
        resultFromBank = b2_debit_1(account, amount, clnt);
    } else {
        //cant find bank name
        result = 1;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return &result;
    }
    // Cleanup
    clnt_destroy(clnt);
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    // Return result
    result = *resultFromBank;
    return &result;
}


int* vb_transfer_1_svc(char* account1, char* account2, int amount, struct svc_req *rqstp) {
    static int result;
    int* resultFromBank;
    CLIENT* clnt;
    sqlite3* db;
    int rc;
    sqlite3_stmt* stmt;
    sqlite3_stmt* stmt2;
    const char* bank1_name;
    const char* bank2_name;

    //open database
    rc = sqlite3_open("virtualbank.db", &db);
    //find bankname1 for account1
    rc = sqlite3_prepare_v2(db, "SELECT bank_name FROM account_map WHERE account_id = ?", -1, &stmt, NULL);
    sqlite3_bind_text(stmt, 1, account1, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        bank1_name = (const char*)sqlite3_column_text(stmt, 0);
    } else {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        result = 1;
        return &result;
    }

    //find bankname2 for account2
    rc = sqlite3_prepare_v2(db, "SELECT bank_name FROM account_map WHERE account_id = ?", -1, &stmt2, NULL);
    sqlite3_bind_text(stmt2, 1, account2, -1, SQLITE_STATIC);
    rc = sqlite3_step(stmt2);
    if (rc == SQLITE_ROW) {
        bank2_name = (const char*)sqlite3_column_text(stmt2, 0);
    } else {
        sqlite3_finalize(stmt2);
        sqlite3_close(db);
        result = 1;
        return &result;
    }

    //attempt a debit with the source bank
    if (strcmp(bank1_name, "BANK1") == 0) {
        clnt = clnt_create(server_name, BANK1, BANK1_VERS, "netpath");
        if (clnt == NULL) {
            clnt_pcreateerror(server_name);
            sqlite3_close(db);
            result = 1;
            return &result;
        }
        resultFromBank = b1_debit_1(account1, amount, clnt);
        clnt_destroy(clnt);
    } else if (strcmp(bank1_name, "BANK2") == 0) {
        clnt = clnt_create(server_name, BANK2, BANK2_VERS, "netpath");
        if (clnt == NULL) {
            clnt_pcreateerror(server_name);
            sqlite3_close(db);
            result = 1;
            return &result;
        }
        resultFromBank = b2_debit_1(account1, amount, clnt);
        clnt_destroy(clnt);
    } else {
        sqlite3_close(db);
        result = 1;
        return &result;
    }
    
    //if that debit call failed, you have insufficient funds and the transfer does not happen.
    if (*resultFromBank == 2 || *resultFromBank == 1) {
        sqlite3_close(db);
        result = *resultFromBank;
        return &result;
    } else {
        //assuming the debit was successful, we credit the other bank.
        if (strcmp(bank2_name, "BANK1") == 0) {
            clnt = clnt_create(server_name, BANK1, BANK1_VERS, "netpath");
            if (clnt == NULL) {
                clnt_pcreateerror(server_name);
                sqlite3_close(db);
                result = 1;
                return &result;
            }
            resultFromBank = b1_credit_1(account2, amount, clnt);
            clnt_destroy(clnt);
        } else if (strcmp(bank2_name, "BANK2") == 0) {
            clnt = clnt_create(server_name, BANK2, BANK2_VERS, "netpath");
            if (clnt == NULL) {
                clnt_pcreateerror(server_name);
                sqlite3_close(db);
                result = 1;
                return &result;
            }
            resultFromBank = b2_credit_1(account2, amount, clnt);
            clnt_destroy(clnt);
        } else {
            //rollback time! but i dont want to use transactions so this is what ya get for now.
            if(strcmp(bank1_name, "BANK1") == 0){
                resultFromBank = b1_credit_1(account1, amount, clnt);
                result = 2;
                sqlite3_close(db);
                clnt_destroy(clnt);
                return &result;
            }
            else{
                resultFromBank = b2_credit_1(account1, amount, clnt);
                result = 2;
                sqlite3_close(db);
                clnt_destroy(clnt);
                return &result;
            }
        }

        //heres where we'd exit if everything was fine
        sqlite3_close(db);
        sqlite3_finalize(stmt);
        sqlite3_finalize(stmt2);
        result = *resultFromBank;
        return &result;
    }
}