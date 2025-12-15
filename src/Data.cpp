#include "inc/Data.h"

// Construtor
Data::Data(int d, int m, int a) : dia(d), mes(m), ano(a) {

    // Validação para garantir que é uma data válida
    if (d < 1 || d > 31 || m < 1 || m > 12 || a < 1900) { 
        throw invalid_argument("Data inválida");
    }
}

// Getters
string Data::toString() const {
    ostringstream oss;
    oss << ano << "/" 
        << setfill('0') << setw(2) << mes << "/" 
        << setfill('0') << setw(2) << dia;
    return oss.str();
}

// Métodos