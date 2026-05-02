 
## Web-Based Virtual File System (VFS)

This project is a Web-Based Virtual File System (VFS) developed as part of our Operating Systems Lab. It combines low-level system programming in C++ with a modern web interface to simulate how file systems actually work behind the scenes.

## Project Overview

The main goal of this project was to understand file system internals by building a custom VFS from scratch and making it accessible through a browser. Users can perform file operations like create, read, write, and delete using a terminal-style interface.

## How It Works

### 1. Core System (C++)
- Implemented the main file system logic  
- Uses **inodes** to store file metadata  
- A **superblock + bitmap** tracks free and used space  
- Manages a virtual disk file (`vdisk.bin`)  
- Fixed block size of 1024 bytes  
- Handles storage and persistence  

### 2. Backend (Node.js & Express)
- Acts as a bridge between frontend and C++ program  
- Runs the C++ executable as a child process  
- Sends commands to C++ via standard input  
- Streams output back to the browser in real-time  

### 3. Frontend (HTML, CSS, JavaScript)
- Terminal-style interface  
- Users can enter commands like:
  - `format`
  - `write`
  - `read`
  - `delete`
- Communicates with backend using API calls  

## Features

- Disk formatting (initializes system structures)  
- File operations (Create, Read, Write, Delete)  
- Automatic block allocation and management  
- Disk usage report (shows memory usage and fragmentation)  
- File search using `find` command  
- Persistent storage (data remains after restart)  

## Technologies Used

- C++  
- Node.js (Express)  
- HTML  
- CSS  
- JavaScript  

## Learning Outcomes

- Understanding of file system architecture  
- Working with inodes, superblocks, and block allocation  
- Process communication between Node.js and C++  
- Building a simple full-stack system  
- Handling real-time input/output  

## Conclusion

- Overall, this project was a good learning experience where we explored file system concepts and successfully connected system-level programming with web technologies.

