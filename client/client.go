//Kayla Humphrey, same docs used as server
package main

import(
	"net"
	"os"
)

func main(){
	//defining constants for tcp
	const (
		HOST = "localhost"
		PORT = "8080"
		TYPE = "tcp"
	)
	

	//resolve the address, meaning get the IP
	tcpServer, err := net.ResolveTCPAddr(TYPE, HOST+":"+PORT) 
	if err != nil {
		println("Error:", err.Error())
		os.Exit(1)
	}

	//dial, or open a socket connection to the server
	conn, err := net.DialTCP(TYPE, nil, tcpServer) 
	if err != nil {
		println("Error:", err.Error())
		os.Exit(1)
	}

	
	//write command to server
	msg := os.Args[1]
	_, err = conn.Write([]byte(msg))
	if err != nil {
		println("Error:", err.Error())
		os.Exit(1)
	}
	
	//read server response
	buffer := make([]byte, 1024)
	n, err := conn.Read(buffer)
	if err != nil {
		println("Error:", err.Error())
		os.Exit(1)
	}

	println(string(buffer[:n]))

	conn.Close()
	
}


