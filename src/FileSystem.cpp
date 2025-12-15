#include "inc/FileSystem.h"
#include "inc/Directory.h"
#include "inc/File.h"
#include "inc/Data.h"
#include <iostream>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;

// ============================================================================
// CONSTRUTOR & DESTRUTOR
// ============================================================================
FileSystem::FileSystem() {
    cout << "Debug: Sistema a iniciar... \n";
    root = new Directory("ROOT");
}

FileSystem::~FileSystem() {
    cout << "Debug: Sistema a desligar... \n";
    delete root;
}



// ============================================================================
// 1. LOAD
// ============================================================================
bool FileSystem::Load(const string &path) { 
    fs::path fsPath(path);

    if (!fs::exists(fsPath) || !fs::is_directory(fsPath)) {
        cout << "Error: Path does not exist or is not a directory.\n";
        return false;
    }

    if (root != nullptr) delete root;

    string newRootName = fsPath.filename().string();
    root = new Directory(newRootName, nullptr);

    cout << "Debug: Loading files into memory.\n";
    try {
        loadRecursive(fsPath, root);
    } catch (const fs::filesystem_error& e) {
        cerr << "SYSTEM ERROR: " << e.what() << "\n";
        return false;
    }
    return true;
}

void FileSystem::loadRecursive(const fs::path& currentPath, Directory* currentNode) {
    for (const auto& item : fs::directory_iterator(currentPath)) {
        string itemName = item.path().filename().string();

        auto ftime = fs::last_write_time(item);
        auto sctp = std::chrono::time_point_cast<std::chrono::system_clock::duration>(
            ftime - fs::file_time_type::clock::now() + std::chrono::system_clock::now()
        );
        
        std::time_t cftime = std::chrono::system_clock::to_time_t(sctp);
        std::tm* ltm = std::localtime(&cftime);
        
        int day = ltm->tm_mday;
        int month = ltm->tm_mon + 1;  
        int year = ltm->tm_year + 1900;
        Data realDate(day, month, year);

        if (fs::is_directory(item)) {
            try {
                Directory* newSubDir = new Directory(itemName, currentNode, realDate);
                currentNode->addSubDirectory(newSubDir);
                
                loadRecursive(item.path(), newSubDir);
            } catch (...) {
                cout << "WARNING: Ignoring folder '" << itemName << "'.\n";
            }
        } 
        else if (fs::is_regular_file(item)) {
            try {
                size_t fileSize = fs::file_size(item);
                
                // Passar a data real para o ficheiro
                File* newFile = new File(itemName, fileSize, realDate);
                currentNode->addFile(newFile);
            
            } catch (...) {
                cerr << "WARNING: Could not read file '" << itemName << "'\n";
            }
        }
    }
}

// ============================================================================
// 2. COUNT FILES
// ============================================================================
int FileSystem::countFiles() {
    if (root == nullptr) {
        cout << "AVISO: Sistema de ficheiros ainda nao foi carregado.\n";
        return 0;
    }
    return countFilesRecursive(root);
}

int FileSystem::countFilesRecursive(Directory* currentNode) const {
    int totalCount = currentNode->getFiles().size();

    for (Directory* subDir : currentNode->getSubDirectories()) {
        totalCount += countFilesRecursive(subDir);
    }

    return totalCount;
}

// ============================================================================
// 3. COUNT DIRECTORIES
// ============================================================================
int FileSystem::countDirectories() {
    if (root == nullptr) {
        cout << "AVISO: Sistema de ficheiros ainda nao foi carregado.\n";
        return 0;
    }
    return countDirectoriesRecursive(root);
}

int FileSystem::countDirectoriesRecursive(Directory* currentNode) const {
    int totalCount = currentNode->getSubDirectories().size();

    for (Directory* subDir : currentNode->getSubDirectories()) {
        totalCount += countDirectoriesRecursive(subDir);
    }

    return totalCount;
}

// ============================================================================
// 4. CALCULATE MEMORY
// ============================================================================
int FileSystem::calcMemory() {
    if (root == nullptr) return 0;
    size_t totalBytes = calcMemoryRecursive(root);
    return static_cast<int>(totalBytes);
}

size_t FileSystem::calcMemoryRecursive(Directory* currentNode) const {
    size_t totalSize = 0;

    for (File* f : currentNode->getFiles()) {
        totalSize += f->getSize();
    }

    for (Directory* subDir : currentNode->getSubDirectories()) {
        totalSize += calcMemoryRecursive(subDir);
    }

    return totalSize;
}

// ============================================================================
// 5. FIND DIRECTORY WITH MOST ELEMENTS
// ============================================================================
string* FileSystem::findDirWithMostElements() { 
    if (root == nullptr) return nullptr;

    int maxCount = -1;
    string* bestPath = nullptr;

    findDirWithMostElementsRecursive(root, root->getName(), maxCount, bestPath);

    return bestPath;
}

void FileSystem::findDirWithMostElementsRecursive(Directory* current, string currentPath, int& maxCount, string*& bestPath) {
    int localCount = (int)(current->getFiles().size() + current->getSubDirectories().size());

    if (current != root && localCount > maxCount) {
        maxCount = localCount;
        if (bestPath != nullptr) delete bestPath;
        bestPath = new string(currentPath);
    }

    for (Directory* sub : current->getSubDirectories()) {
        string nextPath = currentPath + "/" + sub->getName();
        findDirWithMostElementsRecursive(sub, nextPath, maxCount, bestPath);
    }
}

// ============================================================================
// 6. FIND DIRECTORY WITH LEAST ELEMENTS
// ============================================================================
string* FileSystem::findDirWithLeastElements() { 
    if (root == nullptr) return nullptr;

    int minCount = -1;
    string* bestPath = nullptr;

    findDirWithLeastElementsRecursive(root, root->getName(), minCount, bestPath);

    return bestPath;
}

void FileSystem::findDirWithLeastElementsRecursive(Directory* current, string currentPath, int& minCount, string*& bestPath) {
    int currentCount = (int)(current->getFiles().size() + current->getSubDirectories().size());

    if (current != root) {
        if (minCount == -1 || currentCount < minCount) {
            minCount = currentCount;
            if (bestPath != nullptr) delete bestPath;
            bestPath = new string(currentPath);
        }
    }

    for (Directory* sub : current->getSubDirectories()) {
        string nextPath = currentPath + "/" + sub->getName();
        findDirWithLeastElementsRecursive(sub, nextPath, minCount, bestPath);
    }
}

// ============================================================================
// 7. FIND LARGEST FILE
// ============================================================================
string* FileSystem::findLargestFile() { 
    if (!root) return nullptr;

    size_t bestSize = 0;
    string* bestPath = nullptr;

    findLargestFileRecursive(root, root->getName(), bestSize, bestPath);

    return bestPath;
}

void FileSystem::findLargestFileRecursive(Directory* current, string currentPath, size_t& bestSize, string*& bestPath) {
    for (File* f : current->getFiles()) {
        if (f->getSize() > bestSize) {
            bestSize = f->getSize();
            if (bestPath != nullptr) delete bestPath;
            bestPath = new string(currentPath + "/" + f->getName());
        }
    }

    for (Directory* sub : current->getSubDirectories()) {
        string nextPath = currentPath + "/" + sub->getName();
        findLargestFileRecursive(sub, nextPath, bestSize, bestPath);
    }
}

// ============================================================================
// 8. FIND LARGEST DIRECTORY
// ============================================================================
string* FileSystem::findLargestDir() { 
    if (!root) return nullptr;

    size_t maxSpace = 0;
    string* bestPath = nullptr;

    findLargestDirRecursive(root, root->getName(), maxSpace, bestPath);

    return bestPath;
}

size_t FileSystem::findLargestDirRecursive(Directory* current, string currentPath, size_t& maxSpace, string*& bestPath) {
    size_t currentTotalSize = 0;

    for (File* f : current->getFiles()) {
        currentTotalSize += f->getSize();
    }

    for (Directory* sub : current->getSubDirectories()) {
        string nextPath = currentPath + "/" + sub->getName();
        currentTotalSize += findLargestDirRecursive(sub, nextPath, maxSpace, bestPath);
    }

    if (current != root) {
        if (bestPath == nullptr || currentTotalSize >= maxSpace) {
            maxSpace = currentTotalSize;
            if (bestPath != nullptr) delete bestPath;
            bestPath = new string(currentPath);
        }
    }

    return currentTotalSize;
}

// ============================================================================
// 9. SEARCH
// ============================================================================
string* FileSystem::search(const string &s, int type) {
    if (!root) return nullptr;
    return searchRecursive(root, root->getName(), s, type);
}

string* FileSystem::searchRecursive(Directory* current, string currentPath, const string& targetName, int type) {
    if (type == 1 && current->getName() == targetName) {
        return new string(currentPath); 
    }

    if (type == 0) {
        for (File* f : current->getFiles()) {
            if (f->getName() == targetName) {
                return new string(currentPath + "/" + f->getName());
            }
        }
    }

    for (Directory* sub : current->getSubDirectories()) {
        string nextPath = currentPath + "/" + sub->getName();
        string* found = searchRecursive(sub, nextPath, targetName, type);
        
        if (found != nullptr) {
            return found;
        }
    }

    return nullptr;
}

// ============================================================================
// 10. REMOVE ALL
// ============================================================================
bool FileSystem::removeAll(const string &s, const string &type) { 
    if (root == nullptr) return false;

    if (type=="DIR" && s == root->getName()) {
        cout << "ERROR: Cannot remove root directory.\n";
    }

    return removeAllRecursive(root, s, type);
}

bool FileSystem::removeAllRecursive(Directory* current, const string& name, const string& type) {
    bool somethingRemoved = false;

    if (type == "DIR") {
        if (current->removeSubDirectory(name)) {
            somethingRemoved = true;
        }

        for (Directory* sub : current->getSubDirectories()) {
            if (removeAllRecursive(sub, name, type)) {
                somethingRemoved = true;
            }
        }
    } 
    else {
        while (current->removeFile(name)) {
            somethingRemoved = true;
        }

        for (Directory* sub : current->getSubDirectories()) {
            if (removeAllRecursive(sub, name, type)) {
                somethingRemoved = true;
            }
        }
    }

    return somethingRemoved;
}

// ============================================================================
// 11. WRITE XML
// ============================================================================
void FileSystem::writeXML(const string &s) {
    if (root == nullptr) return;

    string filename = s;
    if (filename.substr(filename.length() - 4) != ".xml") {
        filename += ".xml";
    }

    ofstream outFile(filename);

    if (!outFile.is_open()) {
        cout << "ERROR: Could not open file '" << filename << "' for writing.\n";
        return;
    }

    outFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";

    writeXMLRecursive(root, outFile, 0);

    outFile.close();
    cout << "DEBUG: XML export completed to '" << filename << "'.\n";
}

void FileSystem::writeXMLRecursive(Directory* current, ofstream& out, int depth) const {
    string indent(depth * 2, ' ');

    out << indent << "<Directoria nome=\"" << current->getName() << "\">\n";

    for (const auto& file : current->getFiles()) {
        out << indent << "  <Ficheiro nome=\"" << file->getName() 
            << "\" tamanho=\"" << file->getSize() 
            << "\" data=\"" << file->getCreationDate() << "\"/>\n";
    }

    for (const auto& subDir : current->getSubDirectories()) {
        writeXMLRecursive(subDir, out, depth + 1);
    }

    out << indent << "</Directoria>\n";
}

// ============================================================================
// 12. READ XML
// ============================================================================
bool FileSystem::readXML(const string &s) {
    ifstream inFile(s);
    if (!inFile.is_open()) {
        cout << "ERROR: Could not open file '" << s << "' for reading.\n";
        return false;
    }

    if (root != nullptr) {
        delete root;
        root = nullptr;
    }

    string line;
    Directory* currentDir = nullptr;

    while (getline(inFile, line)) {
        if (line.find("<Directoria") != string::npos && line.find("</Directoria>") == string::npos) {
            string name = getXMLAttribute(line, "nome");
            
            if (root == nullptr) {
                root = new Directory(name, nullptr);
                currentDir = root;
            } else {
                if (currentDir != nullptr) {
                    Directory* newDir = new Directory(name, currentDir); 
                    currentDir->addSubDirectory(newDir);
                    currentDir = newDir; 
                }
            }
        }
        else if (line.find("</Directoria>") != string::npos) {
            if (currentDir != nullptr) {
                currentDir = currentDir->getParent();
            }
        }
        else if (line.find("<Ficheiro") != string::npos) {
            if (currentDir != nullptr) {
                string name = getXMLAttribute(line, "nome");
                string sizeStr = getXMLAttribute(line, "tamanho");
                string dateStr = getXMLAttribute(line, "data");

                size_t size = stoull(sizeStr);

                int d = 1, m = 1, y = 2025;
                if (!dateStr.empty()) {
                    char sep;
                    stringstream ss(dateStr);
                    ss >> y >> sep >> m >> sep >> d;
                }
                
                Data fDate(d, m, y);
                File* newFile = new File(name, size, fDate);
                currentDir->addFile(newFile);
            }
        }
    }

    inFile.close();
    
    if (root == nullptr) return false; 
    
    return true;
}

string FileSystem::getXMLAttribute(const string& line, const string& key) {
    string searchKey = key + "=\"";
    size_t pos = line.find(searchKey);
    if (pos == string::npos) return "";

    pos += searchKey.length();
    size_t endPos = line.find("\"", pos);
    if (endPos == string::npos) return "";

    return line.substr(pos, endPos - pos);
}

// ============================================================================
// 13. MOVE FILE
// ============================================================================
bool FileSystem::moveFile(const string &filename, const string &newDirName) {
    if (root == nullptr) return false;

    pair<File*, Directory*> fileInfo = findFileAndParentRecursive(root, filename);
    File* fileToMove = fileInfo.first;
    Directory* currentParent = fileInfo.second;

    if (fileToMove == nullptr || currentParent == nullptr) {
        cout << "ERROR: File '" << filename << "' not found.\n";
        return false;
    }

    Directory* targetDir = nullptr;
    if (newDirName == "ROOT" || newDirName == root->getName()) {
        targetDir = root;
    } else {
        targetDir = findDirectoryRecursive(root, newDirName);
    }

    if (targetDir == nullptr) {
        cout << "ERROR: Target directory '" << newDirName << "' not found.\n";
        return false;
    }

    if (currentParent == targetDir) {
        cout << "WARNING: File is already in destination directory.\n";
        return false;
    }

    targetDir->addFile(fileToMove);
    currentParent->detachFile(fileToMove);

    cout << "DEBUG: Moved '" << filename << "' from '" << currentParent->getName() 
         << "' to '" << targetDir->getName() << "'.\n";
    
    return true;
}

pair<File*, Directory*> FileSystem::findFileAndParentRecursive(Directory* current, const string& name) {
    for (File* f : current->getFiles()) {
        if (f->getName() == name) {
            return {f, current};
        }
    }

    for (Directory* sub : current->getSubDirectories()) {
        pair<File*, Directory*> result = findFileAndParentRecursive(sub, name);
        if (result.first != nullptr) return result;
    }

    return {nullptr, nullptr};
}

Directory* FileSystem::findDirectoryRecursive(Directory* current, const string& name) {
    if (current->getName() == name) return current;

    for (Directory* sub : current->getSubDirectories()) {
        Directory* found = findDirectoryRecursive(sub, name);
        if (found != nullptr) return found;
    }
    return nullptr;
}


// ============================================================================
// 14. MOVE DIRECTORY
// ============================================================================

bool FileSystem::moveDirectory(const string &oldDirName, const string &newDirName) {
    if (root == nullptr) return false;

    if (oldDirName == root->getName()) {
        cout << "ERROR: Cannot move root directory.\n";
        return false;
    }

    pair<Directory*, Directory*> oldInfo = findDirAndParentRecursive(root, oldDirName);
    Directory* dirToMove = oldInfo.first;
    Directory* oldParent = oldInfo.second;

    if (dirToMove == nullptr || oldParent == nullptr) {
        cout << "ERROR: Source directory '" << oldDirName << "' not found.\n";
        return false;
    }

    Directory* targetDir = nullptr;
    if (newDirName == root->getName()) {
        targetDir = root;
    } else {
        targetDir = findDirAndParentRecursive(root, newDirName).first;
    }

    if (targetDir == nullptr) {
        cout << "ERROR: Target directory '" << newDirName << "' not found.\n";
        return false;
    }

    if (isDescendant(dirToMove, targetDir)) {
        cout << "ERROR: Invalid move! Destination '" << newDirName 
             << "' is a sub-directory of '" << oldDirName << "'.\n";
        return false;
    }

    if (oldParent == targetDir) {
        cout << "WARNING: Directory is already in destination.\n";
        return false;
    }

    cout << "DEBUG: Moving folder '" << oldDirName << "' to '" << newDirName << "'...\n";
    
    targetDir->addSubDirectory(dirToMove);
    oldParent->detachSubDirectory(dirToMove);

    return true;
}

pair<Directory*, Directory*> FileSystem::findDirAndParentRecursive(Directory* current, const string& name) {
    for (Directory* sub : current->getSubDirectories()) {
        if (sub->getName() == name) {
            return {sub, current};
        }
        
        pair<Directory*, Directory*> found = findDirAndParentRecursive(sub, name);
        if (found.first != nullptr) return found;
    }
    return {nullptr, nullptr};
}

bool FileSystem::isDescendant(Directory* possibleAncestor, Directory* child) const {
    Directory* curr = child;
    while (curr != nullptr) {
        if (curr == possibleAncestor) {
            return true;
        }
        curr = curr->getParent();
    }
    return false;
}

// ============================================================================
// 15. FILE DATE
// ============================================================================
string* FileSystem::fileDate(const string &filename) {
    if (root == nullptr) return nullptr;

    File* file = findFileRecursive(root, filename);

    if (file != nullptr) {
        return new string(file->getCreationDate());
    }

    return nullptr;
}

File* FileSystem::findFileRecursive(Directory* current, const string& name) const {
    for (File* f : current->getFiles()) {
        if (f->getName() == name) {
            return f;
        }
    }


    for (Directory* sub : current->getSubDirectories()) {
        File* found = findFileRecursive(sub, name);
        if (found != nullptr) {
            return found;
        }
    }

    return nullptr;
}

// ============================================================================
// 16. TREE
// ============================================================================
void FileSystem::tree(const string *outfile) {
    if (root == nullptr) return;

    if (outfile == nullptr) {
        treeRecursive(root, "", cout);
    } else {
        string filename = *outfile;
        if (filename.substr(filename.length() - 4) != ".txt") {
            filename += ".txt";
        }

        ofstream f(filename);
        if (f.is_open()) {
            treeRecursive(root, "", f);
            f.close();
            cout << "DEBUG: Tree written to '" << filename << "'.\n";
        } else {
            cout << "ERROR: Could not open file '" << filename << "' for writing.\n";
        }
    }
}

void FileSystem::treeRecursive(Directory* current, string prefix, ostream& out) const {
    out << prefix << "|-- [" << current->getName() << "]" << endl;

    string nextPrefix = prefix + "|   ";

    for (const auto& file : current->getFiles()) {
        out << nextPrefix << "|-- " << file->getName() 
            << " (" << file->getSize() << " bytes)" << endl;
    }

    for (const auto& subDir : current->getSubDirectories()) {
        treeRecursive(subDir, nextPrefix, out);
    }
}


// ============================================================================
// 17. SEARCH ALL DIRECTORIES
// ============================================================================
void FileSystem::searchAllDirectories(list<string> &lres, const string &dirname) {
    lres.clear(); 

    if (root == nullptr) return;

    searchAllDirsRecursive(root, root->getName(), dirname, lres);
}

void FileSystem::searchAllDirsRecursive(Directory* current, string currentPath, const string& target, list<string>& results) const {
    
    if (current->getName() == target) {
        results.push_back(currentPath);
    }

    for (Directory* sub : current->getSubDirectories()) {
        string nextPath = currentPath + "/" + sub->getName();
        searchAllDirsRecursive(sub, nextPath, target, results);
    }
}

// ============================================================================
// 18. SEARCH ALL FILES
// ============================================================================
void FileSystem::searchAllFiles(list<string> &lres, const string &filename) {
    lres.clear();

    if (root == nullptr) return;

    searchAllFilesRecursive(root, root->getName(), filename, lres);
}

void FileSystem::searchAllFilesRecursive(Directory* current, string currentPath, const string& target, list<string>& results) const {
    
    for (File* f : current->getFiles()) {
        if (f->getName() == target) {
            results.push_back(currentPath + "/" + f->getName());
        }
    }

    for (Directory* sub : current->getSubDirectories()) {
        string nextPath = currentPath + "/" + sub->getName();
        searchAllFilesRecursive(sub, nextPath, target, results);
    }
}

// ============================================================================
// 19. RENAME FILES
// ============================================================================
void FileSystem::renameFiles(const string &oldName, const string &newName) {
    if (root == nullptr) return;

    time_t now = time(0);
    tm *ltm = localtime(&now);
    
    Data currentDate(ltm->tm_mday, ltm->tm_mon + 1, ltm->tm_year + 1900);

    cout << "DEBUG: Renaming all occurrences of '" << oldName << "' to '" << newName << "'...\n";

    renameFilesRecursive(root, oldName, newName, currentDate);
}

void FileSystem::renameFilesRecursive(Directory* current, const string& oldName, const string& newName, const Data& date) {
    
    for (File* f : current->getFiles()) {
        if (f->getName() == oldName) {
            f->rename(newName, date);
        }
    }

    for (Directory* sub : current->getSubDirectories()) {
        renameFilesRecursive(sub, oldName, newName, date);
    }
}

// ============================================================================
// 20. DUPLICATE FILES
// ============================================================================
bool FileSystem::duplicateFiles() {
    if (root == nullptr) return false;

    set<string> namesSeen;

    return checkDuplicatesRecursive(root, namesSeen);
}

bool FileSystem::checkDuplicatesRecursive(Directory* current, set<string>& namesSeen) const {
    
    for (File* f : current->getFiles()) {
        string name = f->getName();
        if (namesSeen.count(name)) {
            cout << "DEBUG: Duplicate found: '" << name << "'\n";
            return true;
        }
        namesSeen.insert(name);
    }

    for (Directory* sub : current->getSubDirectories()) {
        if (checkDuplicatesRecursive(sub, namesSeen)) {
            return true; 
        }
    }

    return false;
}

// ============================================================================
// 21. COPY BATCH
// ============================================================================
bool FileSystem::copyBatch(const string &pattern, const string &srcDirName, const string &destDirName) {
    if (root == nullptr) return false;

    Directory* srcDir = (srcDirName == root->getName()) 
                        ? root 
                        : findDirectoryRecursive(root, srcDirName);

    if (srcDir == nullptr) {
        cout << "ERROR: Source directory '" << srcDirName << "' not found.\n";
        return false;
    }

    Directory* destDir = (destDirName == root->getName()) 
                         ? root 
                         : findDirectoryRecursive(root, destDirName);

    if (destDir == nullptr) {
        cout << "ERROR: Destination directory '" << destDirName << "' not found.\n";
        return false;
    }

    int count = 0;
    cout << "DEBUG: Copying files with pattern '" << pattern << "' from '" 
         << srcDirName << "' to '" << destDirName << "'...\n";
    
    copyBatchRecursive(srcDir, pattern, destDir, count);

    cout << "DEBUG: Copied " << count << " files.\n";
    return true;
}

void FileSystem::copyBatchRecursive(Directory* current, const string& pattern, Directory* targetDir, int& count) {
    
    for (File* f : current->getFiles()) {
        string name = f->getName();

        if (name.find(pattern) != string::npos) {
            
            string finalName = generateUniqueName(targetDir, name);
            string dateStr = f->getModificationDate();
            int y = 2025, m = 1, d = 1; 
            char sep;
            stringstream ss(dateStr);
            ss >> y >> sep >> m >> sep >> d;
            Data originalData(d, m, y);
            File* newFile = new File(finalName, f->getSize(), originalData);
            targetDir->addFile(newFile);
            count++;
            
            if (name != finalName) {
                cout << "   -> Copied '" << name << "' as '" << finalName << "'\n";
            } else {
                cout << "   -> Copied '" << name << "'\n";
            }
        }
    }

    for (Directory* sub : current->getSubDirectories()) {
        if (sub != targetDir) {
            copyBatchRecursive(sub, pattern, targetDir, count);
        }
    }
}

string FileSystem::generateUniqueName(Directory* target, const string& originalName) const {
    bool exists = false;
    for (File* f : target->getFiles()) {
        if (f->getName() == originalName) {
            exists = true;
            break;
        }
    }

    if (!exists) return originalName;

    string namePart = originalName;
    string extPart = "";
    
    size_t lastDot = originalName.find_last_of(".");
    if (lastDot != string::npos) {
        namePart = originalName.substr(0, lastDot);
        extPart = originalName.substr(lastDot);
    }

    int sequence = 1;
    while (true) {
        ostringstream oss;
        oss << namePart << setfill('0') << setw(3) << sequence << extPart;
        string newCandidate = oss.str();

        bool collision = false;
        for (File* f : target->getFiles()) {
            if (f->getName() == newCandidate) {
                collision = true;
                break;
            }
        }

        if (!collision) return newCandidate; 
        
        sequence++;
    }

}