This is a pratice project of Intro. to Network Programming.
====================================
***Recommendation: If you are taking this course, please solve it on yourself first.***
        
./make all  
`Compile server and client programs`   
./server &lt;SERVER PORT&gt;  
`Run the server on specific port`  
./client &lt;SERVER IP&gt; &lt;SERVER PORT&gt;  
`Run the client on specific port and ip addrese <!using '127.0.0.1' to connect to local host>`  

The clients can send several types command to the server illustrated below.  
------------------------------------
Hello Message  
`When a client connects to the server, the server will broadcasts new client message to all clients.`  
Offline Message   
`When a client disconnect from the server, the server will send an offline message to other clients.`  
Change Username Message  
`Client can type command 'name <NEW USERNAME>' to change the username.`  
Who Message  
`Client can type command ’who‘ to list all online users.`  
Private Message  
`Client can type command 'tell <USERNAME> <MESSAGE>' to send a private message to a specific client.`  
Broadcast Message  
`Client can type command 'yell <MESSAGE>' to broadcast message to all clients.`  
Error Command  
`Server will reply 'Error command' to the client who typed an error command.`
