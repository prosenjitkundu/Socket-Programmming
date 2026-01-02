Online Collaborative File Editor (Client–Server System)

Overview

This project implements an online collaborative file editor using a client–server architecture. A single server handles multiple concurrent clients (up to 5 active connections) and allows clients to upload files, share access with other clients, and collaboratively read, insert, and delete file contents in real time.

The system is built using C, TCP sockets, UNIX system calls, and multithreading/forking, following the specifications of Programming Assignment 3.

⸻

Key Features
	•	Supports up to 5 concurrent clients
	•	Unique 5-digit Client ID generated on connection
	•	File ownership and permission-based collaboration
	•	Two collaboration roles:
	•	Viewer (V): Read-only access
	•	Editor (E): Read and write access
	•	Server-side enforcement of permissions
	•	Real-time collaboration invites and responses
	•	Robust error handling for invalid operations

⸻

Architecture & Design Choices

Server Side
	•	Uses TCP sockets to listen for client connections
	•	Each client connection is handled using fork() (multi-process model)
	•	Maintains in-memory / file-based records for:
	•	Active clients (Client ID, Socket ID)
	•	Files, owners, collaborators, and permissions
	•	Uses semaphores to guard critical sections such as:
	•	Client record updates
	•	Permission record updates
	•	All file operations (read / insert / delete) are performed strictly on the server side

Client Side
	•	Client connects to the server and then forks into two processes:
	1.	Sender Process – takes user input and sends commands to the server
	2.	Receiver Process – listens for server responses and notifications
	•	Uses two semaphores:
	•	To ensure only one pending operation at a time
	•	To ensure only one process accesses the terminal at a time
	•	Collaboration invites are handled asynchronously, without requiring explicit polling by the user

⸻

Supported Commands (Client Side)

Command	Description
/users	View all active clients
/files	View all uploaded files with owners, collaborators, permissions, and line counts
/upload <filename>	Upload a local file to the server
/download <filename>	Download a file if permission is available
/invite <filename> <client_id> <V/E>	Invite another client as Viewer or Editor
/read <filename> <start> <end>	Read specified lines from a file
/insert <filename> <idx> "<message>"	Insert message at a specific line index
/delete <filename> <start> <end>	Delete specified lines from a file
/exit	Disconnect from server and delete owned files

Note: Files with the same name are not allowed on the server.

⸻

File Operations Rules
	•	Only owners and collaborators can access a file
	•	Only owners and editors can modify a file
	•	Line indices can be in the range [-N, N) where N is the total number of lines
	•	If no indices are provided:
	•	/read → reads entire file
	•	/insert → appends to end of file
	•	/delete → clears entire file contents

⸻

Error Handling

The server returns appropriate error messages for:
	•	Maximum client limit reached
	•	File does not exist
	•	Duplicate filename upload
	•	Invalid line indices
	•	Insufficient permissions
	•	Invalid client ID in collaboration requests
	•	Invite rejection by collaborator

All client commands receive some response from the server, as mandated.

⸻

File Structure

<RollNo>_CL2_A3/
│── server.c
│── client.c
│── readme.txt


⸻

Compilation & Execution

Compile

gcc server.c -o server -lpthread
gcc client.c -o client -lpthread

Run Server

./server <port>

Run Client

./client <server_ip> <port>


⸻

Assumptions
	•	Server and clients run on the same machine
	•	Uploaded files are placed in predefined directories
	•	Client IDs are unique for the session
	•	Server runs indefinitely unless terminated manually

⸻

Evaluation Notes (For Instructors)
	•	Collaboration requests are handled in real time
	•	Permissions are strictly enforced at the server
	•	Concurrency issues are mitigated using semaphores
	•	The design prioritizes correctness, clarity, and compliance with assignment requirements

⸻

Author

Prosenjit Kundu
M.Tech CSE, IIT Kharagpur

⸻

This ReadMe is intentionally concise and structured for quick evaluation, as recommended in the assignment instructions.# Socket-Programmming
