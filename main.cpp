#include <stdio.h>
#include <math.h>
#include "matrix.h"

using namespace std;

#define B 2
#define T 3
#define D_MODEL 12

int main(){
    auto x = std::make_tuple(2,2,2);
    auto y = std::make_tuple(2,2,2);
    auto z = std::make_tuple(2,3,12);

    float X[B][T][D_MODEL] = {
        {
            { 0.0, 0.2, 0.1, 0.2, 0.1, 0.0, 0.05, 0.05, 0.01, 0.0, 0.0, 0.0 },
            { 0.2, 0.1, 0.0,-0.3, 0.1, 0.2, 0.02, 0.03, 0.01, 0.0, 0.0, 0.0 },
            { 0.0, 0.0, 0.0, 0.1, 0.2, 0.1, 0.50, 0.00, 0.00, 0.0, 0.0, 0.0 }
        },
        {
            {-0.2,-0.1, 0.2, 0.0, 0.2, 0.1, 0.00, 0.01, 0.01, 0.0, 0.0, 0.0 },
            {-0.2,-0.2, 0.0, 0.6, 0.0, 0.1, 0.01, 0.00, 0.02, 0.0, 0.0, 0.0 },
            { 0.0, 0.0, 0.0,  0.1, 0.0, 0.0, 0.10, 0.00, 0.00, 0.0, 0.0, 0.0 }
        }
    };

    float A[2][2][2] = {
        {
            { 1.0, 2.0},
            { 3.0, 4.0},
        },
        {
            {-0.1,-0.2},
            {-0.3,-0.4},

        }
    };

    float AA[2][2][2] = {
        {
            { 5.0, 6.0},
            { 7.0, 8.0},
        },
        {
            {-0.5,-0.6},
            {-0.7,-0.8},

        }
    };

    Matrix  I=Matrix(x);
    Matrix  Y=Matrix(y);
    Matrix  Z=Matrix(z);
    
    //T=Y;
    cout <<"I: "<< I;
    cout <<"Y: "<< Y;
    I.get((float*) &A);
    Y.get((float*) &AA);
    Z.get((float*) &X);
    cout <<"I: "<< I;
    cout <<"Y: "<< Y;
    cout <<"Z: "<< Z;
    I=I*Y;
    cout << I;
    std::vector<int> idx(Z.dimensions.size());
    Z.Recorrer(Z.dimensions,idx,0);

    return 0;
}