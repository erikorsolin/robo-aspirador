#include <iostream>
#include <stdlib.h>
#include <string>
//#include "./array_stack.h"
#include <fstream>

using namespace std;

// Copyright [2019] <COLOQUE SEU NOME AQUI...>
#ifndef STRUCTURES_ARRAY_STACK_H
#define STRUCTURES_ARRAY_STACK_H

#include <cstdint>  // std::size_t
#include <stdexcept>  // C++ exceptions

namespace structures {

template<typename T>
//! CLASSE PILHA
class ArrayStack {
 public:
    //! construtor simples
    ArrayStack();
    //! construtor com parametro tamanho
    explicit ArrayStack(std::size_t max);
    //! destrutor
    ~ArrayStack();
    //! metodo empilha
    void push(const T& data);
    //! metodo desempilha
    T pop();
    //! metodo retorna o topo
    T& top();
    //! metodo limpa pilha
    void clear();
    //! metodo retorna tamanho
    std::size_t size();
    //! metodo retorna capacidade maxima
    std::size_t max_size();
    //! verifica se esta vazia
    bool empty();
    //! verifica se esta cheia
    bool full();

 private:
    T* contents;
    int top_;
    std::size_t max_size_;

    static const auto DEFAULT_SIZE = 10u;
};

}  // namespace structures

#endif


template<typename T>
structures::ArrayStack<T>::ArrayStack() {
    max_size_ = DEFAULT_SIZE;
    contents = new T[max_size_];
    top_ = -1;
}

template<typename T>
structures::ArrayStack<T>::ArrayStack(std::size_t max) {
    max_size_ = max;
    contents = new T[max_size_];
    top_ = -1;
}

template<typename T>
structures::ArrayStack<T>::~ArrayStack() {
    delete [] contents;
}

template<typename T>
void structures::ArrayStack<T>::push(const T& data) {
    if (full()) {
        throw std::out_of_range("pilha cheia");
    } else {
        top_++;
        contents[top_] = data;
    }
}

template<typename T>
T structures::ArrayStack<T>::pop() {
    if (empty()) {
        throw std::out_of_range("pilha vazia");
    } else {
        int index_retorno = top_;
        top_--;
        return contents[index_retorno];
    }
}

template<typename T>
T& structures::ArrayStack<T>::top() {
    return contents[top_];
}

template<typename T>
void structures::ArrayStack<T>::clear() {
    top_ = -1;
}

template<typename T>
std::size_t structures::ArrayStack<T>::size() {
    int index_retorno = top_ + 1;
    return index_retorno;
}

template<typename T>
std::size_t structures::ArrayStack<T>::max_size() {
    return max_size_;
}

template<typename T>
bool structures::ArrayStack<T>::empty() {
    if (top_ == -1) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
bool structures::ArrayStack<T>::full() {
    if ((top_ + 1) == static_cast<int>(max_size_)) {
        return true;
    } else {
        return false;
    }
}

/// @brief Essa função lê um arquivo dado arquivo, guarda as 
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
void verifica_vetor(char * buffer) {
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

    if (!pilha.empty()) {
        resultado = false;
    }
    
    if (!resultado || !count) {
        cout << "erro";
    }
}

int main() {

    char xmlfilename[100];
    char * buffer;
    cin >> xmlfilename;  // entrada

    buffer = arquivo_para_vetor(xmlfilename);

    verifica_vetor(buffer);

    delete[] buffer;
    return 0;
}
