all: clean generate_stubs remove_base_stubs build databases
clean: 
	@find . -type f -name "*.o" -exec rm -f {} +
	@find . -type f -name "*.c" -exec rm -f {} +
	@find . -type f -name "*.h" -exec rm -f {} +
	@find . -type f -name "*.db" -exec rm -f {} +
	@find . -type f -perm -111 -exec rm -f {} +


generate_stubs: 
	@rpcgen -N -C transactions.x 
	@rpcgen -N -C bank1.x
	@rpcgen -N -C bank2.x

	@cp transactions.h VirtualBank/ & cp transactions.h Client/
	@cp transactions_xdr.c VirtualBank/ & cp transactions_xdr.c Client/
	@cp transactions_svc.c VirtualBank/ 
	@cp transactions_clnt.c Client/

	@cp bank1.h VirtualBank/ & cp bank1.h Bank1/
	@cp bank1_xdr.c VirtualBank/ & cp bank1_xdr.c Bank1/
	@cp bank1_clnt.c VirtualBank/
	@cp bank1_svc.c Bank1/

	@cp bank2.h VirtualBank/ & cp bank2.h Bank2/
	@cp bank2_xdr.c VirtualBank/ & cp bank2_xdr.c Bank2/
	@cp bank2_clnt.c VirtualBank/
	@cp bank2_svc.c Bank2/

	
remove_base_stubs:
	@find . -maxdepth 1 -type f -name "*.c" -exec rm -f {} +
	@rm -f "transactions.h"
	@rm -f "bank1.h"
	@rm -f "bank2.h"

databases:
	@sqlite3 VirtualBank/virtualbank.db < VirtualBank/schema.sql
	@sqlite3 Bank1/bank1.db < Bank1/schema.sql
	@sqlite3 Bank2/bank2.db < Bank2/schema.sql


build:
	@cc -g -I /usr/include/tirpc -I.. -c -o VirtualBank/transactions_svc.o VirtualBank/transactions_svc.c
	@cc -g -I /usr/include/tirpc -c -o VirtualBank/transactions_xdr.o VirtualBank/transactions_xdr.c
	@cc -g -I /usr/include/tirpc -I.. -c -o VirtualBank/bank1_clnt.o VirtualBank/bank1_clnt.c
	@cc -g -I /usr/include/tirpc -c -o VirtualBank/bank1_xdr.o VirtualBank/bank1_xdr.c
	@cc -g -I /usr/include/tirpc -I.. -c -o VirtualBank/bank2_clnt.o VirtualBank/bank2_clnt.c
	@cc -g -I /usr/include/tirpc -c -o VirtualBank/bank2_xdr.o VirtualBank/bank2_xdr.c
	@g++ -g -I /usr/include/tirpc -c -o VirtualBank/virtualbank.o VirtualBank/virtualbank.cpp
	@g++ -g -I /usr/include/tirpc -o VirtualBank/virtualbank VirtualBank/virtualbank.o VirtualBank/transactions_svc.o VirtualBank/transactions_xdr.o VirtualBank/bank1_clnt.o VirtualBank/bank1_xdr.o VirtualBank/bank2_clnt.o VirtualBank/bank2_xdr.o  -lnsl -ltirpc -lsqlite3

	@cc -g -I /usr/include/tirpc -I.. -c -o Client/transactions_clnt.o Client/transactions_clnt.c
	@cc -g -I /usr/include/tirpc -c -o Client/transactions_xdr.o Client/transactions_xdr.c
	@g++ -g -I /usr/include/tirpc -c -o Client/client.o Client/client.cpp
	@g++ -g -I /usr/include/tirpc -o Client/client Client/client.o Client/transactions_clnt.o Client/transactions_xdr.o -lnsl -ltirpc

	@cc -g -I /usr/include/tirpc -I.. -c -o Bank1/bank1_svc.o Bank1/bank1_svc.c
	@cc -g -I /usr/include/tirpc -c -o Bank1/bank1_xdr.o Bank1/bank1_xdr.c
	@g++ -g -I /usr/include/tirpc -c -o Bank1/bank1.o Bank1/bank1.cpp
	@g++ -g -I /usr/include/tirpc -o Bank1/bank1 Bank1/bank1.o Bank1/bank1_svc.o Bank1/bank1_xdr.o -lnsl -ltirpc -lsqlite3

	@cc -g -I /usr/include/tirpc -I.. -c -o Bank2/bank2_svc.o Bank2/bank2_svc.c
	@cc -g -I /usr/include/tirpc -c -o Bank2/bank2_xdr.o Bank2/bank2_xdr.c
	@g++ -g -I /usr/include/tirpc -c -o Bank2/bank2.o Bank2/bank2.cpp
	@g++ -g -I /usr/include/tirpc -o Bank2/bank2 Bank2/bank2.o Bank2/bank2_svc.o Bank2/bank2_xdr.o -lnsl -ltirpc -lsqlite3


 get_amounts:
	@echo "Contents of BANK1:"
	@sqlite3 Bank1/bank1.db "SELECT * FROM accounts;"

	@echo "Contents of BANK2:"
	@sqlite3 Bank2/bank2.db "SELECT * FROM accounts;"

start:
	@cd VirtualBank && ./virtualbank &
	@wait
	@cd Bank1 &&  ./bank1 &
	@wait
	@cd Bank2 && ./bank2 &
	@wait
	
stop_rpc:
	@echo "Stopping all RPC servers (virtualbank, bank1, bank2)..."
	@for name in virtualbank bank1 bank2; do \
		pids=$$(pgrep $$name); \
		for pid in $$pids; do \
			kill -9 $$pid && echo "Killed $$name PID $$pid"; \
		done; \
	done


test: start 
	@cd Client && ./client < test_input.txt

	@echo "Contents of BANK1:"
	@sqlite3 Bank1/bank1.db "SELECT * FROM accounts;"
	@echo "Contents of BANK2:"
	@sqlite3 Bank2/bank2.db "SELECT * FROM accounts;"

	@echo "Stopping all RPC servers (virtualbank, bank1, bank2)..."
	@for name in virtualbank bank1 bank2; do \
		pids=$$(pgrep $$name); \
		for pid in $$pids; do \
			kill -9 $$pid && echo "Killed $$name PID $$pid"; \
		done; \
	done


