#include "inc/Directory.h"
#include "inc/File.h"
#include "Data.h"

// Construtor
Directory::Directory(const string& n, Directory* p, Data d)
    : name(n), parent(p), created(d), modified(d) 
{
}

Directory::~Directory() {
    for (File* f : files) delete f;
    files.clear();
    for (Directory* d : subDirectories) delete d;
    subDirectories.clear();
}

// Getters
string Directory::getName() const { return name; }
const vector<File*>& Directory::getFiles() const { return files; }
const vector<Directory*>& Directory::getSubDirectories() const { return subDirectories; }
Directory* Directory::getParent() const { return parent; }
string Directory::getCreationDate() const { return created.toString(); }
string Directory::getModificationDate() const { return modified.toString(); }

// Métodos 
void Directory::addFile(File* f) {
    files.push_back(f);
}

bool Directory::removeFile(const string& name) {
    for (auto it = files.begin(); it != files.end(); ++it) {
        if ((*it)->getName() == name) {
            delete *it;
            files.erase(it);
            return true;
        }
    }
    return false;
}

void Directory::addSubDirectory(Directory* d) {
    d->parent = this;
    subDirectories.push_back(d);
}

bool Directory::removeSubDirectory(const string& name) {
    for (auto it = subDirectories.begin(); it != subDirectories.end(); ++it) {
        if ((*it)->getName() == name) {
            delete *it;    
            subDirectories.erase(it);
            return true;
        }
    }
    return false;
}

void Directory::rename(const string& newName, Data dateOfChange) {
    name = newName;
    modified = dateOfChange;
}

void Directory::detachFile(File* f) {
    for (auto it = files.begin(); it != files.end(); ++it) {
        if (*it == f) {
            files.erase(it);
            return;
        }
    }
}

void Directory::detachSubDirectory(Directory* d) {
    for (auto it = subDirectories.begin(); it != subDirectories.end(); ++it) {
        if (*it == d) {
            subDirectories.erase(it);
            return;
        }
    }
}