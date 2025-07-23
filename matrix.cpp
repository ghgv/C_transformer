#include <stdio.h>
#include <math.h>
#include <tuple>
#include <iostream>
#include <cstring>
#include <vector>
#include <vector>
#include <functional>


#include "matrix.h"

using namespace std;

void Matrix::print(const char *name, Matrix *T){
    printf("%s: \n",name);
    int i=0;
    int j=0;
    for (i=0;i<this->M;i++){
        for (j=0;j<this->N;j++){
            printf("%2.2f ",T->data[i+j]);
        }
        printf("\n");
    }
};

std::ostream& operator<<(std::ostream& os, const Matrix& m) {
    os << "shape(";
    for (size_t i = 0; i < m.dimensions.size(); ++i) {
        os << m.dimensions[i];
        if (i + 1 < m.dimensions.size()) os << ", ";
    }
    os << ")\n";
    os <<"Matrix:\n";
    for (int i =0;i<(m.dimensions[0]*m.dimensions[0]*m.dimensions[0]);i++){
            os << m.data[i]<<" ";
    }
    os <<"\n";
    return os;
}

//Asignacion
Matrix::Matrix(const Matrix& other) {
    dimensions = other.dimensions;
    int total = 1;
    for (int d : dimensions) total *= d;
    data = (float*) malloc(total * sizeof(float));
    if (!data) throw std::bad_alloc();
    std::memcpy(data, other.data, total * sizeof(float));
}

Matrix& Matrix::operator=(const Matrix& other) {
    if (this != &other) {
        free(data);  // libera el actual
        dimensions = other.dimensions;
        int total = 1;
        for (int d : dimensions) total *= d;
        data = (float*) malloc(total * sizeof(float));
        if (!data) throw std::bad_alloc();
        std::memcpy(data, other.data, total * sizeof(float));
    }
    return *this;
}

Matrix Matrix::operator*(const Matrix& other) {   
    if (other.dimensions.size() != this->dimensions.size()){
        cout <<"Error, dimensions are different\n";
        cout <<other.dimensions.size()<<" "<<this->dimensions.size()<<"\n";
        exit(-1);
    }

    if ((other.dimensions[dimensions.size()-2] != this->dimensions[dimensions.size()-2] )||other.dimensions[dimensions.size()-1] != this->dimensions[dimensions.size()-1] ){
        cout <<"Error, dimensions are different. ";
        cout <<"First-First term  " <<  this->dimensions[this->dimensions.size()-2] <<". ";
        cout <<"Second-Second term " <<  other.dimensions[other.dimensions.size()-2]   <<". ";
        cout <<"Second first term  " <<  this->dimensions[this->dimensions.size()-1] <<". ";
        cout <<"First second term " <<  other.dimensions[other.dimensions.size()-1]   <<"\n";
        exit(-1);
    }
    
    int M = this->dimensions[this->dimensions.size()-2] ;
    int N = other.dimensions[other.dimensions.size()-2];
    int K = other.dimensions[other.dimensions.size()-1];
    auto x = std::make_tuple(this->dimensions[0],M,N);
    
    Matrix P =Matrix(x);
    P.data = (float *)malloc(this->dimensions[0]*M*N* sizeof(float));
    for (int BATCH=0; BATCH< this->dimensions[0];BATCH++)
        for (int i=0; i< M; i++)
            for (int j=0;j<N; j++){
                for(int k=0;k<K;k++){
                    /*printf("(B,i,j): (%i,%i,%i), k: %i M: %i N: %i\n",BATCH,i,j,k, M,N);
                    printf("C:%i ",BATCH*(M*N)+i*N+j);
                    printf(" A = %i %f   ", BATCH*(M*N)+i*N+k,this->data[BATCH*(M*N)+i*N+k]);
                    printf(" B = %i %f \n", BATCH*(M*N)+k*N+j,other.data[BATCH*(M*N)+k*N+j]);*/
                    P.data[BATCH*(M*N)+i*N+j]+=this->data[BATCH*(M*N)+i*N+k]*other.data[BATCH*(M*N)+k*N+j];
                }
            }
    return P;
}

Matrix Matrix::operator*(float escalar) const {
    Matrix result = *this;
    size_t total = 1;
    for (int d : dimensions) total *= d;
    for (size_t i = 0; i < total; i++) {
        result.data[i] *= escalar;
    }
    return result;
}

//This get numbers from a lineal array.
void Matrix::get(float * M){
    int total =1; //Ojo no 0 o se queda en cero
    for (int d : dimensions) total *= d;
    printf("\nTotal spaces: %i\n",total);
    for (int j=0; j<total;j++){
        this->data[j]=M[j];
        printf("%2.2f ",M[j]);
    }
}

 Matrix Matrix::softmax(const Matrix& other) const{
    /*Matrix salida(*this);
    int B = other.dimensions[0];
    int H = other.dimensions[1];
    int T = other.dimensions[2];
    int C = other.dimensions[3];
    for (int b=0; b<B;b++)
        for (int h=0; h<H; h++)
            for (int t=0; t<T; t++)
                for (int c=0; c<C; c++){
                    salida.data[b*(H*T)+i*t+j]+=exp(this->data[b*(h*t)+i*t+k]);
                }*/
}

void Matrix::Recorrer(const std::vector<int>& dims,std::vector<int>& actual,int nivel){

    
    std::string acumulador;
    if(nivel==dims.size()){
        printf("\n");
        std::cout << acumulador;
        for (auto a : actual){
            printf("%i ",a);
            for (auto b :dims)
            printf("%f ",this->data[a*b]);
        }
        nivel =0;
        return;
    }
    for(int n=0; n<dims[nivel]; n++){
        actual[nivel] = n;
        acumulador += ' ';
        Recorrer(dimensions,actual,nivel+1);
    }

    


}


Matrix::~Matrix(){
     if (data) free(data);
};