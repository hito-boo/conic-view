#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <fstream>

using namespace std;

// Alunos:
// - Lucas de Oliveira Fratus (RA: 134698)
// - Vitor da Rocha Machado (RA: 132769)

// Orientações para execução disponíveis no arquivo README.md

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
    Vetor3 eixoXLocal, eixoYLocal; // Base vetorial do plano
};

struct Janela {
    double xmin, xmax;
    double ymin, ymax;
};

struct Viewport {
    int xmin, xmax;
    int ymin, ymax;
};

// Lógica de construção da cena ----------------------------------------------------------------------------------------------

Vetor3 calcularNormalPlano(const PlanoProjecao& plano) {
    Vetor3 v12 = plano.p1 - plano.p2;
    Vetor3 v32 = plano.p3 - plano.p2;
    Vetor3 N = produtoVetorial(v12, v32);
    return normalizacao(N);
}

void calcularBaseDoPlano(PlanoProjecao& plano) {
    // Define o eixo X
    plano.eixoXLocal = normalizacao(plano.p2 - plano.p1);
    
    // Define o eixo Y
    plano.eixoYLocal = normalizacao(produtoVetorial(calcularNormalPlano(plano), plano.eixoXLocal));
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
    M.m[3][0] = N.x;            M.m[3][1] = N.y;            M.m[3][2] = N.z;            M.m[3][3] = -d1;

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

void ajustarJanela(Janela& j, const Viewport& v) {
    double j_largura = j.xmax - j.xmin;
    double j_altura = j.ymax - j.ymin;
    double Rj = j_largura / j_altura;

    double v_largura = (double)(v.xmax - v.xmin);
    double v_altura = (double)(v.ymax - v.ymin);
    double Rv = v_largura / v_altura;

    if (Rj > Rv) {
        double novaAltura = j_largura / Rv;
        double centroY = (j.ymax + j.ymin) / 2.0;

        j.ymin = centroY - (novaAltura / 2.0);
        j.ymax = centroY + (novaAltura / 2.0);
    } else {
        double novaLargura = j_altura * Rv;
        double centroX = (j.xmax + j.xmin) / 2.0;

        j.xmin = centroX - (novaLargura / 2.0);
        j.xmax = centroX + (novaLargura / 2.0);
    }
    
    cout << "--- Janela Ajustada para Aspect Ratio ---\n";
    cout << "X: " << j.xmin << " ate " << j.xmax << "\n";
    cout << "Y: " << j.ymin << " ate " << j.ymax << "\n\n";
}

Vetor3 converterParaSistemaDoPlano(const Vetor3& pontoProjetado, const PlanoProjecao& plano) {
    Vetor3 vetorPonto = pontoProjetado - plano.p1;
    
    double u = produtoEscalar(vetorPonto, plano.eixoXLocal);
    double v = produtoEscalar(vetorPonto, plano.eixoYLocal);
    
    return {u, v, 0};
}

struct PontoPixel {
    int u, v;
};

PontoPixel mundoParaViewport(const Vetor3& pMundo, const Janela& j, const Viewport& v) {
    PontoPixel pixel;
    double nx = (pMundo.x - j.xmin) / (j.xmax - j.xmin);
    double ny = (pMundo.y - j.ymin) / (j.ymax - j.ymin);

    pixel.u = v.xmin + (int)(nx * (v.xmax - v.xmin));
    pixel.v = v.ymin + (int)((1.0 - ny) * (v.ymax - v.ymin));

    return pixel;
}

void salvarSVG(string nomeArquivo, const vector<PontoPixel>& pixels, const vector<Face>& faces, const Viewport& v) {
    ofstream arquivo(nomeArquivo);
    
    if (!arquivo.is_open()) {
        cout << "Erro ao criar arquivo SVG!\n";
        return;
    }

    arquivo << "<svg width=\"" << v.xmax << "\" height=\"" << v.ymax 
            << "\" xmlns=\"http://www.w3.org/2000/svg\">\n";
            
    arquivo << "<rect width=\"100%\" height=\"100%\" fill=\"white\" />\n";

    arquivo << "<g stroke=\"black\" stroke-width=\"2\" fill=\"none\">\n";

    for (const auto& face : faces) {
        arquivo << "  <polygon points=\"";
        for (int indice : face.indices) {
            int i = indice - 1; 

            if (i >= 0 && i < (int)pixels.size()) {
                arquivo << pixels[i].u << "," << pixels[i].v << " ";
            }
        }
        
        arquivo << "\" />\n";
    }

    arquivo << "</g>\n";
    arquivo << "</svg>";
    
    arquivo.close();
    cout << "Arquivo '" << nomeArquivo << "' gerado com sucesso!\n";
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
    plano.p1 = {0, 0, 1};
    plano.p2 = {0, 0, 0};
    plano.p3 = {1, 0, 0};
    calcularBaseDoPlano(plano);

    // Definição do Observador
    Vetor3 observador = {-4, 20, 4};

    // Definição da Janela (Área 20x20 centrada na origem)
    Janela janela;
    janela.xmin = -10.0;
    janela.xmax = 10.0;
    janela.ymin = -10.0;
    janela.ymax = 10.0;

    // 5. Definição da Viewport (800x600 pixels)
    Viewport viewport;
    viewport.xmin = 0;
    viewport.xmax = 800;
    viewport.ymin = 0;
    viewport.ymax = 600;

    // Execução do Pipeline

    // Ajustar tamanho da janela em relação à viewport
    ajustarJanela(janela, viewport);

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

    // Vetor para guardar os pontos convertidos em 2D
    vector<PontoPixel> pontosTela;

    // Processar os vértices
    cout << "--- Processando Vertices ---\n";
    for (size_t i = 0; i < piramide.vertices.size(); i++) {
        // Projeção
        Vetor3 pProjetado = projetarVertice(piramide.vertices[i], matrizProj);
        
        // Converter orientação
        Vetor3 pLocal = converterParaSistemaDoPlano(pProjetado, plano);
        
        // Mapeamento
        PontoPixel pixel = mundoParaViewport(pLocal, janela, viewport);
        
        // Guardar no vetor
        pontosTela.push_back(pixel);
    }

    // Desenhar (Gerar arquivo)
    salvarSVG("resultado.svg", pontosTela, piramide.faces, viewport);

    return 0;
}