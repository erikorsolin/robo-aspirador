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
        // Le o próximo caractere
        char_atual = buffer[++i];
    }

    // Se a pilha não estiver vazia ou se não houver nenhuma chave
    if (!pilha.empty() || !count) {
        resultado = false;
    }
    
    return resultado;
}


/// @brief Estrutura para armazenar os dados de um cenário
struct Cenario {
    int** matriz;
    std::pair<int, int> coord_robo, dim_matriz;
    std::string nome;  
};


/// @brief Função para extrair os dados dos cenários e armazenar em um vetor de estruturas Cenario
/// @param buffer Vetor de caracteres a ser extraido os dados
/// @return Vetor contendo todos os cenários que o vetor continha
std::vector<Cenario> vetor_para_matriz(char* buffer) {
    std::vector<Cenario> cenarios;
    char* cenario_ptr = strstr(buffer, "<cenario>"); // Criando ponteiro para a primeira ocorrência de <cenario> do buffer

    // Enquanto houver ocorrências de <cenario> no buffer o laço continua
    while (cenario_ptr) {
        Cenario cenario;

        char* nome_ptr = strstr(cenario_ptr, "<nome>");       // Criando ponteiro para a primeira ocorrência de <nome> do cenário atual
        char* altura_ptr = strstr(cenario_ptr, "<altura>");   // Criando ponteiro para a primeira ocorrência de <altura> do cenário atual
        char* largura_ptr = strstr(cenario_ptr, "<largura>"); // Criando ponteiro para a primeira ocorrência de <largura> do cenário atual
        char* matriz_ptr = strstr(cenario_ptr, "<matriz>");   // Criando ponteiro para a primeira ocorrência de <matriz> do cenário atual
        char* x_ptr = strstr(cenario_ptr, "<x>");             // Criando ponteiro para a primeira ocorrência de <x> do cenário atual
        char* y_ptr = strstr(cenario_ptr, "<y>");             // Criando ponteiro para a primeira ocorrência de <y> do cenário atual 

        // Se todos os ponteiros forem válidos, extrair os dados do cenário
        if (nome_ptr && altura_ptr && largura_ptr && matriz_ptr && x_ptr && y_ptr) {
            cenario.nome = std::string(nome_ptr + strlen("<nome>"), strstr(nome_ptr, "</nome>") - (nome_ptr + strlen("<nome>")));  // Extrair o nome do cenário atual
            cenario.dim_matriz.second = atoi(altura_ptr + strlen("<altura>"));     // Extrair a altura do cenário atual
            cenario.dim_matriz.first = atoi(largura_ptr + strlen("<largura>"));  // Extrair a largura do cenário atual
            cenario.coord_robo.first = atoi(x_ptr + strlen("<x>"));  // Extrair a coordenada x do cenário atual
            cenario.coord_robo.second = atoi(y_ptr + strlen("<y>"));  // Extrair a coordenada y do cenário atual

            matriz_ptr += strlen("<matriz>");         // Avançar o ponteiro para o início da matriz
            std::string str_matriz;                   // Criar uma string para armazenar a matriz

            // Extrair a matriz do cenário atual
            for (int i = 0; i < cenario.dim_matriz.second * cenario.dim_matriz.first; i++) {
                while (*matriz_ptr == '\n' || *matriz_ptr == '\r') {    // Ignorar quebras de linha
                        *matriz_ptr++;
                }
                str_matriz.push_back(*matriz_ptr++);                    // Adicionar o caractere atual à string da matriz
            }
            
            // Alocar memória para a matriz do cenário atual
            cenario.matriz = new int*[cenario.dim_matriz.second];
            for (int i = 0; i < cenario.dim_matriz.second; i++) {
                cenario.matriz[i] = new int[cenario.dim_matriz.first];
            }

            // Preencher a matriz do cenário atual com os dados extraídos
            int k = 0;
            for (int i = 0; i < cenario.dim_matriz.second; i++) {
                for (int j = 0; j < cenario.dim_matriz.first; j++) {
                    cenario.matriz[i][j] = str_matriz[k++] - '0';
                }
            }

            // Adicionar o cenário atual ao vetor de cenários
            cenarios.push_back(cenario);
        }

        // Avançar o ponteiro para a próxima ocorrência de <cenario>
        cenario_ptr = strstr(cenario_ptr + strlen("<cenario>"), "<cenario>");
    }

    return cenarios;
}



/// @brief Função para verificar a área do componente conexo que contém a posição (x, y) na matriz
/// @param matriz Matriz contendo todo o espaço em que o robo sera inserido
/// @param coord_robo Coordenada em que o robo está na matriz (x,y)
/// @param dim_matriz Dimensões da matriz
void verifica_area(int** matriz, std::pair<int,int> coord_robo,std::pair<int,int> dim_matriz) {
    int x = coord_robo.first;
    int y = coord_robo.second;
    int largura = dim_matriz.first;
    int altura = dim_matriz.second;
    structures::ArrayQueue<std::pair<int, int>> fila; // Criando uma fila de pares de inteiros
    
    // Alocando memória para a matriz R
    int** R = new int*[altura];
    for (int i = 0; i < altura; i++) {
        R[i] = new int[largura];
    }

    // Inicializando a matriz R com 0s
    for (int linha = 0; linha < altura; linha++) {
        for (int coluna = 0; coluna < largura; coluna++) {
            R[linha][coluna] = 0;
        }
    }

    // Se a posição (x, y) for 0, a área é 0
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
    char xmlfilename[100];
    char * buffer;

    // Recebe o nome do arquivo a ser lido
    cin >> xmlfilename;

    // Lê o arquivo e guarda os dados em um buffer
    buffer = arquivo_para_vetor(xmlfilename);

    // Verifica se o buffer segue a regra de aninhamento de arquivos .xml
    if (!verifica_vetor(buffer)) {
        cout << "erro";
        delete[] buffer;
        return 0;
    }

    // Extrai os dados dos cenários e armazena em um vetor de estruturas Cenario
    std::vector<Cenario> cenarios = vetor_para_matriz(buffer);

    // Para cada cenário, verificar a área do componente conexo que contém a posição (x, y) na matriz
    for (const Cenario& cenario : cenarios) {
        cout << cenario.nome << " ";
        verifica_area(cenario.matriz, cenario.coord_robo, cenario.dim_matriz);

        // Libera a memória alocada pela matriz do cenário atual
        for (int i = 0; i < cenario.dim_matriz.second; i++) {
            delete[] cenario.matriz[i];
        }
        delete[] cenario.matriz;
    }

    delete[] buffer;
    cout << endl;

    return 0;
}