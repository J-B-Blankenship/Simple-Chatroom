# Chat Room

The following repository embodies a simple chat room of N clients (chat participants) to 1 server (chat room). Throughout the code may be references to an assessment (original cause of source code). However, this repository will be incrementally updated as an example of my development work and environment setups.

## Features

* The `!echo` command has the server room's "bot" reply with the command stripped.
* The `!eval` command evaluates two unsigned integers for the following basic operations: +, -, /, *.
* The chat room can be run on a Windows and Linux environment.

## Possible Future Features

* Identification of chat room participants and chat room "bot"
* '!eval` command expaneded to signed integers
* '!eval' command support additional operations
* `!eval` command support additional integers
* Server stores history of chat room between sessions
* Server offloads chat history when reaching the limitations of available RAM

## Possible Alterations to Development Environment

* Support GitHub actions to verify merge requests with the passing of all unit tests for Linux and Windows
* Proper organization of the source code into a directory structure

## Open Issues

* Multiplication of an unsigned integer with another unsigned integer can result in integer overflow. The reasons and a possible solution are recorded directly in the source code for future work and bug fixes.
* Unit testing needs to be expanded for entire coverage of business logic of the source code. The only function currently tested is the `!eval` function.
