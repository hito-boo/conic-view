#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

using namespace std;

// Núcleo Matemático - Álgebra Linear ----------------------------------------------------------------------------------------

struct Vetor3 {
    double x, y, z;

    // Sobrecarga de operador para subtração
    Vetor3 operator-(const Vetor3& b) const {
        return {x - b.x, y - b.y, z - b.z};
    }
};

struct Mat4 {
    double m[4][4];
};

double produtoEscalar(const Vetor3& a, const Vetor3& b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vetor3 produtoVetorial(const Vetor3& a, const Vetor3& b) {
    return {
        a.y * b.z - a.z * b.y,
        -(a.x * b.z - a.z * b.x),
        a.x * b.y - a.y * b.x
    };
}

Vetor3 normalizacao(const Vetor3& v) {
    double m = sqrt(produtoEscalar(v, v));
    if (m == 0) return {0, 0, 0};
    return {v.x / m, v.y / m, v.z / m};
}

vector<double> multMatrizVetor(const Mat4& M, const Vetor3& P) {
    vector<double> r(4); // Vetor homogêneo que será retornado
    r[0] = M.m[0][0]*P.x + M.m[0][1]*P.y + M.m[0][2]*P.z + M.m[0][3]*1;
    r[1] = M.m[1][0]*P.x + M.m[1][1]*P.y + M.m[1][2]*P.z + M.m[1][3]*1;
    r[2] = M.m[2][0]*P.x + M.m[2][1]*P.y + M.m[2][2]*P.z + M.m[2][3]*1;
    r[3] = M.m[3][0]*P.x + M.m[3][1]*P.y + M.m[3][2]*P.z + M.m[3][3]*1;
    return r;
}

// Estruturas de Dados da Cena -----------------------------------------------------------------------------------------------

struct Face {
    vector<int> indices; // Índices dos vértices (devem estar sequenciados corretamente)
};

struct Objeto3D {
    vector<Vetor3> vertices;
    vector<Face> faces;
};

struct PlanoProjecao {
    Vetor3 p1, p2, p3; // Três pontos não colineares que definem o plano
};

// Lógica de construção da cena ----------------------------------------------------------------------------------------------

Vetor3 calcularNormalPlano(const PlanoProjecao& plano) {
    Vetor3 v12 = plano.p1 - plano.p2;
    Vetor3 v32 = plano.p3 - plano.p2;
    Vetor3 N = produtoVetorial(v12, v32);
    return normalizacao(N);
}

Mat4 gerarMatrizPerspectiva(const Vetor3& C, const PlanoProjecao& plano) {
    Vetor3 N = calcularNormalPlano(plano);
    double d0 = produtoEscalar(plano.p1, N); 
    double d1 = produtoEscalar(C, N);
    double d = d0 - d1;

    Mat4 M;
    M.m[0][0] = d + C.x * N.x;  M.m[0][1] = C.x * N.y;      M.m[0][2] = C.x * N.z;      M.m[0][3] = -C.x * d0;
    M.m[1][0] = C.y * N.x;      M.m[1][1] = d + C.y * N.y;  M.m[1][2] = C.y * N.z;      M.m[1][3] = -C.y * d0;
    M.m[2][0] = C.z * N.x;      M.m[2][1] = C.z * N.y;      M.m[2][2] = d + C.z * N.z;  M.m[2][3] = -C.z * d0;
    M.m[3][0] = N.x;            M.m[3][1] = N.y;            M.m[3][2] = N.z;            M.m[3][3] = d1;

    return M;
}

Vetor3 projetarVertice(const Vetor3& original, const Mat4& matrizPersp) {
    vector<double> h = multMatrizVetor(matrizPersp, original);

    double w = h[3];
    if (abs(w) < 1e-9) return {0, 0, 0}; 

    return {
        h[0] / w, // xc
        h[1] / w, // yc
        h[2] / w  // zc
    };
}

// Função principal ----------------------------------------------------------------------------------------------------------

int main() {
    cout << fixed << setprecision(6);

    // Setup das entradas (hardcoded)
    
    // Definição do Objeto
    Objeto3D piramide;
    piramide.vertices = {
        {1, 1, 1}, {7, 1, 1}, {7, 1, 7}, {1, 1, 7}, {4, 7, 4}
    };
    piramide.faces = {
        {{1, 2, 3, 4}}, {{1, 2, 5}}, {{2, 3, 5}}, {{4, 4, 5}}, {{4, 1, 5}}
    };

    // Definição do Plano de Projeção
    PlanoProjecao plano;
    plano.p1 = {0, 0, 0};
    plano.p2 = {1, 0, 0};
    plano.p3 = {0, 1, 0};

    // Definição do Observador
    Vetor3 observador = {20, 10, 30};

    // Execução do Pipeline

    // Gerar matriz de projeção
    Mat4 matrizProj = gerarMatrizPerspectiva(observador, plano);

    // Debug: Mostrar a matriz gerada
    cout << "Matriz de Projecao Calculada:\n";
    for (int i=0; i<4; i++) {
        cout << "| ";
        for (int j=0; j<4; j++) cout << setw(8) << matrizProj.m[i][j] << " ";
        cout << "|\n";
    }
    cout << endl;

    // Processar os vértices
    cout << "--- Projecao dos Vertices ---\n";
    for (size_t i = 0; i < piramide.vertices.size(); i++) {
        Vetor3 pProjetado = projetarVertice(piramide.vertices[i], matrizProj);
        
        cout << "V" << i+1 << " Original: (" 
             << piramide.vertices[i].x << ", " 
             << piramide.vertices[i].y << ", " 
             << piramide.vertices[i].z << ") -> "
             << "Projetado: (" 
             << pProjetado.x << ", " 
             << pProjetado.y << ", " 
             << pProjetado.z << ")\n";
    }

    return 0;
}