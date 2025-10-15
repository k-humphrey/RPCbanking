.PHONY: build run wait test clean

test: wait
	./test_cases.sh
	

build: 
	go build -o serverEx ./server
	go build -o clientEx ./client

run: build
	nohup ./serverEx > /dev/null 2>&1 & 


# i needed to listen to make sure the server is actually up before we do test cases,
# this line is only needed for "make" not by calling these labels individually
wait: run
	@until lsof -iTCP:8080 -sTCP:LISTEN -n > /dev/null; do \
		echo "Waiting for server..."; \
		sleep 1; \
	done


clean: 
	rm -f ./serverEx
	rm -f ./clientEx
