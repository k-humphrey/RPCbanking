// Kayla Humphrey, docs used: https://www.golinuxcloud.com/golang-tcp-server-client/
package main

import (
	"net"
	"os"
	"fmt"
	"strings"
	"strconv"
	"assignment1/tuplespace"
)


func main(){
	//variables for tcp
	const (
    	HOST = "localhost"
    	PORT = "8080"
    	TYPE = "tcp"
	)	

	//create tuplespace
	ts := tuplespace.CreateTS()
	
	//start server
	listen, err := net.Listen(TYPE, HOST+":"+PORT)
	if err != nil { 
    	println("Error:", err.Error())
		os.Exit(1)
	}
	defer listen.Close()
	fmt.Println("Server running, listening to port ", PORT)
	//infinity loop to listen for connections, spawn a new goroutine for each connection
	for {
		conn, err := listen.Accept()
		if err != nil {
			println("Error:", err.Error())
			os.Exit(1)
		}

		go handleRequest(conn, ts) 
	}
}

//HANDLEREQUEST
//handles client connections by parsing their input and calling the correct commands
func handleRequest(conn net.Conn, ts *tuplespace.TupleSpace) {
	buffer := make([]byte, 1024)
	n, err := conn.Read(buffer)
	if err != nil {
		println("Error:", err.Error())
		os.Exit(1)
	}
	//this is intended to verify if a string matches the servers criteria for commands sent by the client
	//message structure is "-cmd (string, ?, int64, float64)" 
	//Notice, strings are not in quotes, as a consequence of this design all unsupported types will default to strings
	msg := string(buffer[:n])
	var cmd string
	fmt.Println(msg)

	if msg == "-kill"{ //if killing server, no tuple argument needed
		msg = "killed server"
		_, err = conn.Write([]byte(msg))
		if err != nil {
			println("Error:", err.Error())
			os.Exit(1)
		}
		os.Exit(1)
	}

	i := strings.Index(msg, " ")
	if(i != -1){
		cmd = msg[:i]
		msg = strings.TrimSpace(msg[i+1:])

	}

	var tuple tuplespace.Tuple
	msg = strings.Trim(msg, "()")
	values := strings.Split(msg, ",")
	for _, value := range values {
		value = strings.TrimSpace(value)

		if (value == "?"){ //wildcard
			tuple = append(tuple, nil)
			continue
		}
        // Try int, if it works, run the code block to append it
        if i, err := strconv.Atoi(value); err == nil {
            tuple = append(tuple, i)
            continue
        }
        if f, err := strconv.ParseFloat(value, 64); err == nil {
            tuple = append(tuple, f)
            continue
        }
        // default: string
        tuple = append(tuple, value)
	}


	//this is a switch to call the commands and then build a response message to the client from those command returns
	switch cmd{
	case "-out":
		err = ts.Out(tuple)
		if err != nil{
			msg = err.Error()
		} else{
			msg = "Tuple with: "
			for i, value := range tuple {
				msg += fmt.Sprintf("%T: %v", value, value)
				if i < len(tuple)-1 {
					msg += ", and "
				}
			}
			msg += " stored in tuple space."
		}
	case "-rd":
		tuple, err = ts.Rd(tuple)
		if err != nil{
			msg = err.Error()
		} else{
			msg = "Matched tuple: "
			for i, v := range tuple {
				msg += fmt.Sprintf("%T: %v", v, v)
				if i < len(tuple)-1 {
					msg += ", "
				}
			}
		}
	case "-in":
		tuple, err = ts.In(tuple)
		if err != nil{
			msg = err.Error()
		} else{
			msg = "deleted tuple: "
			for i, v := range tuple {
				msg += fmt.Sprintf("%T: %v", v, v)
				if i < len(tuple)-1 {
					msg += ", "
				}
			}
		}
	default:
		msg = "Error: message format invalid, command not found"
	}
	
	//write response to client
	_, err = conn.Write([]byte(msg))
	if err != nil {
		println("Error:", err.Error())
		os.Exit(1)
	}
	// close conn
	conn.Close()
}

