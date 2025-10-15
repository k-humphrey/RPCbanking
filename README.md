# LISTEN LINDA 
# ------------------
# A TCP server and client model that uses a tuplespace to store tuples.
# Clients connect to the server and may store tuples (-out), read tuples (-rd), or delete tuples (-in)


# HOW TO USE
# ------------------
# Make sure you have go version 1.19 installed
# Assuming you have downloaded, and unzipped (if needed) the Assignment1 folder
# Make sure your working directory is in assignment 1, but not any of the subfolders

# Option 1, run the test cases
# Step 1: Type "make" in the terminal 
# Step 2: Type "make clean" when done

# Option 2, manually running
# Step 1: Type "make run" in terminal, this simply builds and starts the server in the background
# Step 2: Manually send commands from the client by typing:  ./clientEX "-cmd (1, 2, 3)"
# Step 3: When you want the server to shutoff, send a -kill command


# DESIGN EXPLANATION
# ------------------
# Valid commands are structured as follows
# -cmd (tuple contents)
# Tuple contents can be int64, float64, a wildcard ? symbol, or strings without quotes
# Tuples are just comma seperated lists, all tuples can hold as much or as little of any type.
# If a type isnt explicitly supported, it will be assumed to be string in the tuplespace. This is intended to add flexibility to the tuplespace, allowing the client to parse its own types if it wants to.

# valid instructions are as follows
# -out, to add the intended tuple to the tuplespace
# -rd, searches for the tuple template given using a matching algorithm, if no match is found the thread sleeps until out is called again. When a match is found, the first matching tuple will always return, but this is not guaranteed to be the same tuple everytime so it's technically random
# -in, does the same thing as read, but also deletes the tuple it finds
# -kill, a way to tell the server to shutoff (not how I'd do it in the real world, but its fine here)

# examples
# ./clientEX "-out (1, 1.1, ?, string)"
# ./clientEX "-rd (1, 1.1, ?, string)"
# ./clientEX "-in (1, 1.1, ?, string)"
# ./clientEX "-kill"



