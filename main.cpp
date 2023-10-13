#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <cstring>
#include <vector>
#include <utility>  // para std::pair
#include "array_stack.h"
#include "array_queue.h"
using namespace std;

/// @brief Essa função lê um arquivo dado, guarda as 
/// informações em um vetor e retorna o ponteiro desse vetor
/// @param xmlfilename Nome do arquivo que queremos transformar
/// em vetor de caracteres
/// @return Retorna um ponteiro para o buffer alocado em memória
char * arquivo_para_vetor(char * xmlfilename) {
    
    // Abre o arquivo xml
    ifstream myfile;
    myfile.open(xmlfilename);  
    // Pega o ponteiro do buffer
    filebuf* pbuf = myfile.rdbuf();

    // Pega o tamanho do buffer
    size_t size = pbuf->pubseekoff (0,myfile.end,myfile.in);
    pbuf->pubseekpos (0,myfile.in);

    // Aloca memória para os caracteres
    char* buffer=new char[size];

    // Le os caracteres
    pbuf->sgetn (buffer,size);

    // Fecha o arquivo
    myfile.close();

    return buffer;
}

/// @brief Verifica se um dado vetor de caracteres segue a regra
/// de aninhamento de arquivos .xml
/// @param buffer Vetor de caracteres contendo os dados que
/// deseja-se verificar
bool verifica_vetor(char * buffer) {
    structures::ArrayStack<string> pilha(1000);

    char char_atual = buffer[0];
    int tamanho_buffer = string(buffer).size();
    bool resultado = true;
    
    int i = 0;
    int count = 0;
    while (i < tamanho_buffer) {

        // Entra no laço de uma das chaves do XML
        if (char_atual == '<') {
            count++;
            string temp = string();
            // Le o nome compledo da chave
            while (char_atual != '>' && i < tamanho_buffer) {
                temp.push_back(char_atual);
                char_atual = buffer[++i];
            }
            if (char_atual == 26) {
                break;
            }
            temp.push_back('>');

            // Checa se é uma chave de abertura ou fechamento

            // Se for de abertura coloca na pilha
            if (temp[1] != '/') {
                pilha.push(temp);
            } else {
                // Se for de fechamento verifica se o topo da pilha
                // tem a chave correspondente, caso não tenha imprime
                // "erro"
                temp.erase(1, 1);
                if (pilha.top() != temp) {
                    resultado = false;
                    break;
                } else {
                    pilha.pop();
                }
            }

        }
        char_atual = buffer[++i];
    }

    if (!pilha.empty() || !count) {
        resultado = false;
    }
    
    return resultado;
}



struct Cenario {
    int** matriz;
    int x, y, altura, largura;
    std::string nome;  // Adicionar membro para armazenar o nome do cenário
};

std::vector<Cenario> vetor_para_matriz(char* buffer) {
    std::vector<Cenario> cenarios;
    char* cenario_ptr = strstr(buffer, "<cenario>");

    while (cenario_ptr) {
        Cenario cenario;

        char* nome_ptr = strstr(cenario_ptr, "<nome>");
        char* altura_ptr = strstr(cenario_ptr, "<altura>");  // altura é y
        char* largura_ptr = strstr(cenario_ptr, "<largura>"); // largura é x
        char* matriz_ptr = strstr(cenario_ptr, "<matriz>");
        char* x_ptr = strstr(cenario_ptr, "<x>");
        char* y_ptr = strstr(cenario_ptr, "<y>");

        if (nome_ptr && altura_ptr && largura_ptr && matriz_ptr && x_ptr && y_ptr) {
            cenario.nome = std::string(nome_ptr + strlen("<nome>"), strstr(nome_ptr, "</nome>") - (nome_ptr + strlen("<nome>")));  // Extrair o nome do cenário
            cenario.altura = atoi(altura_ptr + strlen("<altura>"));
            cenario.largura = atoi(largura_ptr + strlen("<largura>"));
            cenario.x = atoi(x_ptr + strlen("<x>"));
            cenario.y = atoi(y_ptr + strlen("<y>"));

            matriz_ptr += strlen("<matriz>");
            std::string str_matriz;
            for (int i = 0; i < cenario.altura * cenario.largura; i++) {
                while (*matriz_ptr == '\n' || *matriz_ptr == '\r') {
                        *matriz_ptr++;
                }
                str_matriz.push_back(*matriz_ptr++);
            }
            
            cenario.matriz = new int*[cenario.altura];
            for (int i = 0; i < cenario.altura; i++) {
                cenario.matriz[i] = new int[cenario.largura];
            }

            int k = 0;
            for (int i = 0; i < cenario.altura; i++) {
                for (int j = 0; j < cenario.largura; j++) {
                    cenario.matriz[i][j] = str_matriz[k++] - '0';
                }
            }

            cenarios.push_back(cenario);
        }

        cenario_ptr = strstr(cenario_ptr + strlen("<cenario>"), "<cenario>");
    }

    return cenarios;
}




void verifica_area(int** matriz, int x, int y, int altura, int largura) {
    structures::ArrayQueue<std::pair<int, int>> fila;
    
    // Criar uma matriz R de 0 (zeros) com o mesmo tamanho da matriz de entrada lida
    int** R = new int*[altura];
    for (int i = 0; i < altura; i++) {
        R[i] = new int[largura];
    }

    for (int linha = 0; linha < altura; linha++) {
        for (int coluna = 0; coluna < largura; coluna++) {
            R[linha][coluna] = 0;
        }
    }

    if (matriz[x][y] == 0) {
        cout << '0';
        cout << endl;
        return;
    }

    // Matriz[x][y] = Matriz[i][j]
    fila.enqueue({x, y});
    R[x][y] = 1;

    // Enquanto a fila não estiver vazia
    while (!fila.empty()) {
        auto coord = fila.dequeue();
        x = coord.first;
        y = coord.second;
        

        // Definindo os vizinhos-4
        std::pair<int, int> vizinhos[4] = {{x, y-1}, {x, y+1}, {x-1, y}, {x+1, y}};
        
        for (auto& vizinho : vizinhos) {
            int nx = vizinho.first;
            int ny = vizinho.second;
        

            // Verificar se o vizinho está dentro do domínio da matriz
            if (ny >= 0 && ny < largura && nx >= 0 && nx < altura) {
                // Verificar se o vizinho tem intensidade 1 em E e ainda não foi visitado em R
                if (matriz[nx][ny] == 1 && R[nx][ny] == 0) {
                    fila.enqueue({nx, ny});
                    R[nx][ny] = 1;  // Marcar como visitado
                }
            }
        }
    }

     // Contar a quantidade de 1s na matriz R para determinar a área do componente conexo
    int area = 0;
    for (int i = 0; i < altura; i++) {
        for (int j = 0; j < largura; j++) {
            if (R[i][j] == 1) {
                area++;
            }
        }
    }

    std::cout << area << std::endl;

    // Liberar memória alocada para R
    for (int i = 0; i < altura; i++) {
        delete[] R[i];
    }
    delete[] R;
}




int main() {
    int caseNumber = 1;
    char xmlfilename[100];
    char * buffer;

    cin >> xmlfilename;

    buffer = arquivo_para_vetor(xmlfilename);

    if (!verifica_vetor(buffer)) {
        cout << "erro";
        delete[] buffer;
        return 0;
    }

    std::vector<Cenario> cenarios = vetor_para_matriz(buffer);

    for (const Cenario& cenario : cenarios) {
        cout << cenario.nome << " ";
        verifica_area(cenario.matriz, cenario.x, cenario.y, cenario.altura, cenario.largura);
    }

    delete[] buffer;
    cout << endl;

    return 0;
}