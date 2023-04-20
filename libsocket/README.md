# LibSocket
## A C++ wrapper around system socket library functions

### About

The BSD socket libraries for dealing with various socket types have documentation spread across several different man pages, and since they are global system calls it is difficult to know every single system call and macro definition that's available to you when dealing with sockets. This library aims to consolidate all of that functionality into an easy to use, encapsulated, and well documented API for C++.

### Handler system

All of the classes in LibSocket have handler maps which allow you to associate any error that occurs during a syscall with a handler. It's good practice to properly handle all errors that occur, and now you can guarantee all errors get handled!

[//]: # (### Structure)

[//]: # ()
[//]: # (#### LibSocket::Socket)

[//]: # ()
[//]: # (This is the base class that contains the functionality every socket will have. It holds the file descriptor of the socket, automatic closing functionality, and various ways to send and receive data.)

[//]: # ()
[//]: # (It also contains additional functionality where if you wanted to have a simple header that describes the size of a payload, it will automatically size the header and perform validation on sent and received messages over TCP and UDP. This is the purpose of the template parameter. This allows you to derive a class from `LibSocket::Socket` and that functionality will remain consistent.)

[//]: # ()
[//]: # (#### LibSocket::ServerSocket)

[//]: # ()
