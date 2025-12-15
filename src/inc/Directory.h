#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include "Data.h"

using namespace std;

class File;
class Directory;

class Directory {
    private:
        string name;
        Directory* parent;
        Data created;
        Data modified;

        vector<File*> files;
        vector<Directory*> subDirectories;

    public:
        // Construtor
        Directory(const string& n, Directory* p = nullptr, Data d = Data());
        ~Directory();

        string getName() const;
        string getCreationDate() const;
        string getModificationDate() const;
        

        // Getters
        const vector<File*>& getFiles() const;
        const vector<Directory*>& getSubDirectories() const;
        Directory* getParent() const;

        // Métodos de modificação
        void addFile(File* f);
        void addSubDirectory(Directory* d);
        bool removeFile(const string& name);
        bool removeSubDirectory(const string& name);
        void rename(const string& newName, Data dateOfChange);
        void detachFile(File* f);
        void detachSubDirectory(Directory* d);
        
};