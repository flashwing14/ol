#pragma once
#include <string>
#include <vector>
#include <list>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <set>
#include <sstream>
#include <iomanip>

using namespace std;
namespace fs = std::filesystem;

// Forward Declaration
class Directory;
class File;
class Data;

class FileSystem {
    private:
        Directory* root;

        // Recursive helpers
        void loadRecursive(const fs::path& currentPath, Directory* currentNode);
        int countFilesRecursive(Directory* currentNode) const;
        int countDirectoriesRecursive(Directory* currentNode) const;
        size_t calcMemoryRecursive(Directory* currentNode) const;
        void findDirWithMostElementsRecursive(Directory* current, string currentPath, int& maxCount, string*& bestPath);
        void findDirWithLeastElementsRecursive(Directory* current, string currentPath, int& minCount, string*& bestPath);
        void findLargestFileRecursive(Directory* current, string currentPath, size_t& bestSize, string*& bestPath);
        size_t findLargestDirRecursive(Directory* current, string currentPath, size_t& maxSpace, string*& bestPath);
        string* searchRecursive(Directory* current, string currentPath, const string& targetName, int type);
        bool removeAllRecursive(Directory* current, const string& name, const string& type);
        void writeXMLRecursive(Directory* current, ofstream& out, int depth) const;
        string getXMLAttribute(const string& line, const string& key);
        pair<File*, Directory*> findFileAndParentRecursive(Directory* current, const string& name);
        Directory* findDirectoryRecursive(Directory* current, const string& name);
        File* findFileRecursive(Directory* current, const string& name) const;
        pair<Directory*, Directory*> findDirAndParentRecursive(Directory* current, const string& name);
        bool isDescendant(Directory* possibleAncestor, Directory* child) const;
        void treeRecursive(Directory* current, string prefix, ostream& out) const;
        void searchAllDirsRecursive(Directory* current, string currentPath, const string& target, list<string>& results) const;
        void searchAllFilesRecursive(Directory* current, string currentPath, const string& target, list<string>& results) const;
        void renameFilesRecursive(Directory* current, const string& oldName, const string& newName, const Data& date);
        bool checkDuplicatesRecursive(Directory* current, set<string>& namesSeen) const;
        void copyBatchRecursive(Directory* current, const string& pattern, Directory* targetDir, int& count);
        string generateUniqueName(Directory* target, const string& originalName) const;

    public:
        // Construtor & Destrutor
        FileSystem();
        ~FileSystem();

        // Métodos
        bool Load(const string &path);
        int countFiles();
        int countDirectories();
        int calcMemory();
        string* findDirWithMostElements();
        string* findDirWithLeastElements();
        string* findLargestFile();
        string* findLargestDir();
        string* search(const string &s, int type);
        bool removeAll(const string &s, const string &type);
        void writeXML(const string &s);
        bool readXML(const string &s); 
        bool moveFile(const string &Fich, const string &DirNova);
        bool moveDirectory(const string &DirOld, const string &DirNew);
        string* fileDate(const string &ficheiro);
        void tree(const string *fich = nullptr);
        bool duplicateFiles();
        void searchAllDirectories(list<string> &lres, const string &dir);
        void searchAllFiles(list<string> &lres, const string &file);
        void renameFiles(const string &fich_old, const string &fich_new);
        bool copyBatch(const string &padrao, const string &DirOrigem, const string &DirDestino);
};