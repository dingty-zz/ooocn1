Select-based Echo Server
======

Modules
------

This server is divided into the following modules:

- netservice

  This is the the place where the main framework of server's doing.

- select pool

  This provides the interfaces of preparing for read, write,
  and adding and removing clients in the select pool.

- clientsocket

  This provides the interfaces for client socket operation like receiving,
  sending data.

- Http objects

  This bundle of modules process the raw data sent from client
  and generate corresponding response messages which is put into write buffer.

Algorithm
------

### How client socket sends and receives data.

Each client has two buffers:

1. Read Buffer - reading data sent from client
2. Writer Buffer - sending data to client

### How to process raw data into HTTP requests and responses

Http Request:

1. Request line and Headers

I'm reading one byte at a time and the state machine will detect
when a line feed ('\r\n') happends. In this case, the server will
parse the data and save it to individual request objects.

2. Request Content
I'm reading as much as the content-length tells me.

Http Response:

1. Status line and headers

The response generator is a state machine that handles putting data into the write buffer
and maintain the previous state to resume working on.

2. Pipelining

When we don't need the request data anymore, we can pipelining more requests
while still sending data out.

