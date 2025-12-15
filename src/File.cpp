#include "inc/File.h"
#include "Data.h"

// Construtor
File::File(const string& n, size_t s, Data d)
    : name(n), size(s), created(d), modified(d) 
{
}

// Destrutor
File::~File() {
}

// Getters Simples
string File::getName() const { return name; }
size_t File::getSize() const { return size; }

// Getters de Data (Usam a classe Data)
string File::getCreationDate() const {
    return created.toString(); 
}

string File::getModificationDate() const {
    return modified.toString(); 
}

// Setters
void File::rename(const string& newName, Data dateOfChange) {
    name = newName;
    modified = dateOfChange;
}

void File::setSize(size_t newSize) {
    size = newSize;
}