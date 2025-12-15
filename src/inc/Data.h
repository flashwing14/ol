#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

class Data {
private:
    int dia;
    int mes;
    int ano;

public:
    // Construtor
    Data(int d = 1, int m = 1, int a = 2025);

    // Getters
    string toString() const;
    int getDia() const { return dia; }
    int getMes() const { return mes; }
    int getAno() const { return ano; }

    // Métodos
};