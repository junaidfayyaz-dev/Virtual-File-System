/*
--------------------------------------------------
Project: Simple Virtual File System (VFS)
Course: Operating Systems Lab
Instructor: Noreen Khalid/

Team Members:
Junaid Fayyaz   
Rayan Ashfaq    
Sarmad Ejaz     
Ahmad Jaan      
Fesses Amir     
--------------------------------------------------
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
using namespace std;

#define BLOCK_SIZE 1024
#define TOTAL_BLOCKS 64
#define MAX_FILES 20
#define DISK_NAME "vdisk.bin"

struct Inode {
    char name[32];
    int size;
    int block;
    bool used;
};

struct Superblock {
    bool bitmap[TOTAL_BLOCKS];
};

Superblock sb;
Inode inode[MAX_FILES];

// Initialize data structures
void initialize() {
    memset(&sb, 0, sizeof(sb));
    for(int i = 0; i < MAX_FILES; i++) {
        inode[i].used = false;
        inode[i].name[0] = '\0';
        inode[i].size = 0;
        inode[i].block = -1;
    }
}

// Save superblock & inode table to disk
void save() {
    fstream disk(DISK_NAME, ios::binary | ios::in | ios::out);
    if(!disk) {
        
        disk.open(DISK_NAME, ios::binary | ios::out);
        disk.close();
        disk.open(DISK_NAME, ios::binary | ios::in | ios::out);
    }
    disk.seekp(0);
    disk.write((char*)&sb, sizeof(sb));
    disk.write((char*)inode, sizeof(inode));
    disk.close();
}

// Format disk
void format() {
    initialize();
    ofstream disk(DISK_NAME, ios::binary);
    disk.write((char*)&sb, sizeof(sb));
    disk.write((char*)inode, sizeof(inode));
    char buf[BLOCK_SIZE] = {0};
    for(int i = 0; i < TOTAL_BLOCKS; i++) {
        disk.write(buf, BLOCK_SIZE);
    }
    disk.close();
    cout << "Disk formatted successfully\n";
}

// Create a file
void create(string name) {
    for(int i = 0; i < MAX_FILES; i++) {
        if(inode[i].used && strcmp(inode[i].name, name.c_str()) == 0) {
            cout << "File '" << name << "' already exists\n";
            return;
        }
    }
    
    for(int i = 0; i < MAX_FILES; i++) {
        if(!inode[i].used) {
            for(int b = 0; b < TOTAL_BLOCKS; b++) {
                if(!sb.bitmap[b]) {
                    sb.bitmap[b] = true;
                    strncpy(inode[i].name, name.c_str(), 31);
                    inode[i].name[31] = '\0';
                    inode[i].block = b;
                    inode[i].size = 0;
                    inode[i].used = true;
                    save();
                    cout << "File '" << name << "' created successfully\n";
                    return;
                }
            }
            cout << "No free blocks available\n";
            return;
        }
    }
    cout << "Maximum number of files reached\n";
}

// Write data to file
void writeFile(string name, string text) {
    if(!text.empty() && text.front() == '"' && text.back() == '"') {
        text = text.substr(1, text.size() - 2);
    }

    for(int i = 0; i < MAX_FILES; i++) {
        if(inode[i].used && strcmp(inode[i].name, name.c_str()) == 0) {
            fstream disk(DISK_NAME, ios::binary | ios::in | ios::out);
            int pos = sizeof(sb) + sizeof(inode) + inode[i].block * BLOCK_SIZE;
            disk.seekp(pos);
            
            char buf[BLOCK_SIZE] = {0};
            size_t textSize = text.size();
            if(textSize > BLOCK_SIZE) textSize = BLOCK_SIZE;
            
            memcpy(buf, text.c_str(), textSize);
            disk.write(buf, BLOCK_SIZE);
            
            inode[i].size = textSize;
            disk.close();
            save();
            cout << "Data written to '" << name << "'\n";
            return;
        }
    }
    cout << "File '" << name << "' not found\n";
}

// Read data from file
void readFile(string name) {
    for(int i = 0; i < MAX_FILES; i++) {
        if(inode[i].used && strcmp(inode[i].name, name.c_str()) == 0) {
            ifstream disk(DISK_NAME, ios::binary);
            int pos = sizeof(sb) + sizeof(inode) + inode[i].block * BLOCK_SIZE;
            disk.seekg(pos);
            
            char buf[BLOCK_SIZE] = {0};
            disk.read(buf, inode[i].size);
            disk.close();
            
            cout << "Content of '" << name << "': " << buf << endl;
            return;
        }
    }
    cout << "File '" << name << "' not found\n";
}

// Find files matching a pattern
void findFile(string name) {
    bool found = false;
    for(int i = 0; i < MAX_FILES; i++) {
        if(inode[i].used && string(inode[i].name).find(name) != string::npos) {
            cout << "Found: " << inode[i].name;
            cout << " (Size: " << inode[i].size << " bytes, Block: " << inode[i].block << ")\n";
            found = true;
        }
    }
    if(!found) cout << "No files found matching '" << name << "'\n";
}

// List all files
void listFiles() {
    bool anyFiles = false;
    cout << "Files in VFS:\n";
    cout << "--------------------------------\n";
    for(int i = 0; i < MAX_FILES; i++) {
        if(inode[i].used) {
            cout << "Name: " << inode[i].name;
            cout << " | Size: " << inode[i].size << " bytes";
            cout << " | Block: " << inode[i].block << endl;
            anyFiles = true;
        }
    }
    if(!anyFiles) cout << "No files found\n";
    cout << "--------------------------------\n";
}

// Delete a file
void deleteFile(string name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (inode[i].used && strcmp(inode[i].name, name.c_str()) == 0) {
            sb.bitmap[inode[i].block] = false;
            inode[i].used = false;
            inode[i].name[0] = '\0';
            inode[i].size = 0;
            inode[i].block = -1;
            save();
            cout << "File '" << name << "' deleted successfully\n";
            return;
        }
    }
    cout << "File '" << name << "' not found\n";
}

// Disk usage report
void diskUsage() {
    int usedBlocks = 0;
    for(int i = 0; i < TOTAL_BLOCKS; i++) {
        if(sb.bitmap[i]) usedBlocks++;
    }
    int freeBlocks = TOTAL_BLOCKS - usedBlocks;
    double usedPercent = (double)usedBlocks / TOTAL_BLOCKS * 100.0;

    cout << "Disk Usage Report:\n";
    cout << "----------------------------\n";
    cout << "Total blocks: " << TOTAL_BLOCKS << "\n";
    cout << "Used blocks:  " << usedBlocks << "\n";
    cout << "Free blocks:  " << freeBlocks << "\n";
    cout << "Usage:        " << usedPercent << "%\n";
    cout << "----------------------------\n";
}

// Rename a file
void renameFile(string oldName, string newName) {
    for(int i = 0; i < MAX_FILES; i++) {
        if(inode[i].used && strcmp(inode[i].name, oldName.c_str()) == 0) {
            
            for(int j = 0; j < MAX_FILES; j++) {
                if(inode[j].used && strcmp(inode[j].name, newName.c_str()) == 0) {
                    cout << "File '" << newName << "' already exists\n";
                    return;
                }
            }
            strncpy(inode[i].name, newName.c_str(), 31);
            inode[i].name[31] = '\0';
            save();
            cout << "File renamed from '" << oldName << "' to '" << newName << "'\n";
            return;
        }
    }
    cout << "File '" << oldName << "' not found\n";
}

// Split command line into tokens
vector<string> splitCommand(const string& cmd) {
    vector<string> tokens;
    bool inQuotes = false;
    string current;

    for(size_t i = 0; i < cmd.length(); i++) {
        if(cmd[i] == '"') inQuotes = !inQuotes;
        else if(cmd[i] == ' ' && !inQuotes) {
            if(!current.empty()) {
                tokens.push_back(current);
                current.clear();
            }
        } else {
            current += cmd[i];
        }
    }
    if(!current.empty()) tokens.push_back(current);
    return tokens;
}

// Load existing VFS data
void loadExistingData() {
    ifstream disk(DISK_NAME, ios::binary);
    if(disk) {
        disk.read((char*)&sb, sizeof(sb));
        disk.read((char*)inode, sizeof(inode));
        disk.close();
    }
}

int main() {
    string line;
    loadExistingData();

    while(getline(cin, line)) {
        line.erase(0, line.find_first_not_of(" \t\n\r\f\v"));
        line.erase(line.find_last_not_of(" \t\n\r\f\v") + 1);
        if(line.empty()) continue;

        vector<string> args = splitCommand(line);
        if(args.empty()) continue;

        string cmd = args[0];

        if(cmd == "format") format();
        else if(cmd == "create" && args.size() > 1) create(args[1]);
        else if(cmd == "write" && args.size() > 2) {
            string text = "";
            for(size_t i = 2; i < args.size(); i++) {
                if(i > 2) text += " ";
                text += args[i];
            }
            writeFile(args[1], text);
        }
        else if(cmd == "read" && args.size() > 1) readFile(args[1]);
        else if(cmd == "find" && args.size() > 1) findFile(args[1]);
        else if(cmd == "list") listFiles();
        else if(cmd == "delete" && args.size() > 1) deleteFile(args[1]);
        else if(cmd == "diskusage") diskUsage();
        else if(cmd == "rename" && args.size() > 2) renameFile(args[1], args[2]);
        else if(cmd == "help") {
            cout << "Available commands:\n";
            cout << "  format                 - Format the virtual disk\n";
            cout << "  create <filename>      - Create a new file\n";
            cout << "  write <filename> <text>- Write text to a file\n";
            cout << "  read <filename>        - Read content of a file\n";
            cout << "  find <pattern>         - Search for files\n";
            cout << "  list                   - List all files\n";
            cout << "  delete <filename>      - Delete a file\n";
            cout << "  diskusage              - Show disk usage report\n";
            cout << "  rename <old> <new>     - Rename a file\n";
            cout << "  help                   - Show this help\n";
        }
        else {
            cout << "Unknown command: " << cmd << "\n";
            cout << "Type 'help' for available commands\n";
        }
    }
    return 0;
}
