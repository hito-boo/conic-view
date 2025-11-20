#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Estruturas

struct Vetor3 {
    double x, y, z;
};


struct Face {
    vector<int> indices; // índices dos vértices
};



Vetor3 operator-(const Vetor3& a, const Vetor3& b) {
    // sobrecarga do operador '-'
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z,
    };
}


Vetor3 produtoVetorial(const Vetor3& a, const Vetor3& b) {
    return {
        a.y * b.z - a.z * b.y,
        -(a.x * b.z - a.z * b.x),
        a.x * b.y - a.y * b.x
    };
}

double produtoEscalar (const Vetor3& a, const Vetor3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vetor3 normalizacao(const Vetor3& v) {
    double m = sqrt(produtoEscalar(v, v));
    return {
        v.x / m,
        v.y / m,
        v.z / m,
    }
}