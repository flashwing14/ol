#pragma once
#include <string>
#include "Data.h"

using namespace std;

class File {
private:
    string name;
    size_t size;
    Data created;
    Data modified;

public:
    // Construtor
    File(const string& n, size_t s, Data d);

    // Destrutor (Faltava esta linha!)
    ~File();

    // Getters
    string getName() const;
    size_t getSize() const;
    string getCreationDate() const;
    string getModificationDate() const;
    string getData() const; // Mantido por compatibilidade

    // Setters
    void rename(const string& newName, Data modificationDate);
    void setSize(size_t newSize);
};