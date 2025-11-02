#include <iostream>
#include <string>
#include "transactions.h"

#define server_name "localhost"
int main()
{
    CLIENT *clnt;
    int *result;
    char* account1, *account2, *operation;
    char buffer[1024];
    int amount;

    while(true){
        //read input, seperate into proper variables
        if (fgets(buffer, sizeof(buffer), stdin)) {
            operation = strtok(buffer, " \n"); 
            if(strcmp(operation, "exit") == 0){
                exit(0);
            }
            account1 = strtok(NULL, " \n");  
            if(strcmp(operation, "transfer") == 0){
                account2 = strtok(NULL, " \n"); 
                amount = atoi(strtok(NULL, " \n")); 
            }
            else{
                amount = atoi(strtok(NULL, " \n")); 
            }
        }
        else{
            std::cout << "\nInput Error.";
        }

        //create client
        clnt = clnt_create(server_name, VIRTUAL_BANK, VB_VERS, "netpath");
        if (clnt == NULL) {
            clnt_pcreateerror(server_name);
            return 1;
        }
        
        // Call the remote procedure.
        if(strcmp(operation, "credit") == 0){
            result = vb_credit_1(account1, amount, clnt);
        }
        else if(strcmp(operation,"debit") == 0){
            result = vb_debit_1(account1, amount, clnt);
        }
        else if(strcmp(operation, "transfer") == 0){
            result = vb_transfer_1(account1, account2, amount, clnt);
        }
        else{
            std::cout << "Error: invalid operation";
        }

        //check result for errors
        if (result == NULL) {
            clnt_perror(clnt, "call failed\n");
        } else {
            if (*result == 0){
                //call success
            }
            else if(*result == 1){
                std::cout << "\nError making call to Virtual Bank. Unknown account\n";
            }
            else if(*result == 2){
                std::cout << "\nError during transaction: insufficient funds.\n";
            }
        }
        clnt_destroy(clnt);
    }
   
    return 0;
}