#ifndef MATRIX_H
#define MATRIX_H

#include <tuple>
#include <iostream>
#include <vector>
#include <functional>

using namespace std;


class Matrix{
    public:
        int M=0;
        int N=0;
        std::vector<int> dimensions;
        template<typename... Args>
        Matrix(const std::tuple<Args...>& t);
        Matrix(const Matrix& other);              // constructor de copia
        Matrix& operator=(const Matrix& other);   // operador de asignación
        Matrix operator*(const Matrix& other);   // operador de asignación
        Matrix operator*(float escalar)  const;
        Matrix softmax(const Matrix& other) const;
        float * data= NULL;
        friend std::ostream& operator<<(std::ostream& os, const Matrix& m);
        void get(float *);
        void for_each_index(const std::function<void(const std::vector<int>&)>& f) const {
            std::vector<int> idx(dimensions.size());
            recorrer(dimensions, idx, 0, f);
        }
        void Recorrer(const std::vector<int>& dims,std::vector<int>& actual,int nivel);
        ~Matrix();
    private:
        void print(const char *name, Matrix *T);
        static void recorrer(const std::vector<int>& dims,
                             std::vector<int>& actual,
                             size_t nivel,
                             const std::function<void(const std::vector<int>&)>& callback) {
            if (nivel == dims.size()) {
                callback(actual);
                return;
            }
            for (int i = 0; i < dims[nivel]; ++i) {
                actual[nivel] = i;
                recorrer(dims, actual, nivel + 1, callback);
            }
        }
        
           
};


//Ojo no se puede poner en el .cpp! El linker no lo encuentra
template<typename... Args>
Matrix::Matrix(const std::tuple<Args...>& t) {
    std::apply([this](const Args&... args) {
        (dimensions.push_back(static_cast<int>(args)), ...);
    }, t);

    int total = 1;
    for (int d : dimensions) total *= d;

    std::cout << "Total dimensions: " << total << "\n";

    data = (float *)malloc(total* sizeof(float));  // ✅ reserva espacio e inicializa en 0
}




#endif