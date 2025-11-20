#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

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
    };
};


// Matriz 4x4

struct Mat4 {
    double m[4][4];
};


vector<double> mult(const Mat4& M, const Vetor3& P) {
    vector<double> r(4);
    r[0] = M.m[0][0]*P.x + M.m[0][1]*P.y + M.m[0][2]*P.z + M.m[0][3]*1;
    r[1] = M.m[1][0]*P.x + M.m[1][1]*P.y + M.m[1][2]*P.z + M.m[1][3]*1;
    r[2] = M.m[2][0]*P.x + M.m[2][1]*P.y + M.m[2][2]*P.z + M.m[2][3]*1;
    r[3] = M.m[3][0]*P.x + M.m[3][1]*P.y + M.m[3][2]*P.z + M.m[3][3]*1;
    return r;
}


Mat4 construirMatrizPerspectiva(Vetor3 C, Vetor3 N, double d0, double d1, double d) {
    Mat4 M;

    M.m[0][0] = d + C.x * N.x;
    M.m[0][1] = C.x * N.y;
    M.m[0][2] = C.x * N.z;
    M.m[0][3] = -C.x * d0;

    M.m[1][0] = C.y * N.x;
    M.m[1][1] = d + C.y * N.y;
    M.m[1][2] = C.y * N.z;
    M.m[1][3] = -C.y * d0;

    M.m[2][0] = C.z * N.x;
    M.m[2][1] = C.z * N.y;
    M.m[2][2] = d + C.z * N.z;
    M.m[2][3] = -C.z * d0;

    M.m[3][0] = N.x;
    M.m[3][1] = N.y;
    M.m[3][2] = N.z;
    M.m[3][3] = d1;

    return M;
}


int main() {
    cout << fixed << setprecision(6); // quantidade de casas decimais
    

    // definição do objeto
    // TO DO: trocar para um novo 
    vector<Vetor3> V = {
        {1, 1, 1},
        {7, 1, 1},
        {7, 1, 7},
        {1, 1, 7},
        {4, 7, 4},
    };

    vector<Face> faces = {
        {{1, 2, 3, 4}},
        {{1, 2, 5}},
        {{2, 3, 5}},
        {{4, 4, 5}},
        {{4, 1, 5}},
    };


    // dados do plano (três pontos)
    // plano Z=0
    Vetor3 P1 = {0, 0, 0};
    Vetor3 P2 = {1, 0, 0};
    Vetor3 P3 = {0, 1, 0};
    
    // vetor normal ao plano
    Vetor3 v12 = P1 - P2;
    Vetor3 v32 = P3 - P2;

    Vetor3 N = produtoVetorial(v12, v32);
    N = normalizacao(N);


    // ponto de vista C

    Vetor3 C = {20, 10, 30};

    double d0 = produtoEscalar(P1, N); // ponto no plano
    double d1 = produtoEscalar(C, N);
    double d = d0 - d1;

    // construção da matriz de perspectiva

    Mat4 M = construirMatrizPerspectiva(C, N, d0, d1, d);

    cout << "Matriz de projeção:\n";
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++){
            cout << setw(10) << M.m[i][j] << " ";
        }
        cout << "\n";
    }

    cout << "\n\nProjeção dos vértices\n";


    // projeção dos vértices

    for (int i=0; i<V.size(); i++) {
        auto h = mult(M, V[i]); // homogênea

        double xc = h[0] / h[3];
        double yc = h[1] / h[3];
        double zc = h[2] / h[3];

        cout << "V" << i+1 << " -> ("
                << xc << ", " << yc << ", " << zc << ")\n";
    }

    return 0;
}