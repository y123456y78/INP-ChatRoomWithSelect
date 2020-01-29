This is a pratice project of Intro. to Network Programming, using select fuction to implement a multiple clients Chat Room.

Recommended: If you are a student who is taking this subject, please solve it on yourself first, before moving on to the solution.

./make all
##Compile server and client programs
./server <SERVER PORT> 
##To run the server on specific port
./client <SERVER IP> <SERVER PORT>
##To run the client on specific port and ip addrese <!using '127.0.0.1' to connect to local host>

The clients can send and get several types command to the server illustrated below.
Hello Message
##When a client connects to the server, the server will send a hello message to the client and then broadcasts this user’s presence to other clients.
Offline Message
##When a client disconnect from the server, the server will send an offline message to all the other online clients to tell them someone has been offline.
Change Username Message
##Client can type command 'name <NEW USERNAME>' to change the username.
Who Message
##Client can type command ’who‘ to list all online users.
Private Message
##Client can type command 'tell <USERNAME> <MESSAGE>' to send a private message to a specific client.
Broadcast Message
##Client can type command 'yell <MESSAGE>' to broadcast message to all clients.
Error Command
##Server will reply 'Error command' to the client who typed an error command.
