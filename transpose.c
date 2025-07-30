#include <stdio.h>

#define B   1
#define L   2
#define H   2
#define d_k 4
#define d_model  H * d_k

int main() {
    
    

    // Simulamos X[B][L][d_model]
    float X[B][L][d_model] = {
        {
            {0, 1, 2, 3, 4, 5, 6, 7},
            {8, 9,10,11,12,13,14,15}
        }
    };

    // Reshape + permute → X_perm[B][H][L][d_k]
    float X_perm[B][H][L][d_k];

    // Reordenamiento explícito
    for (int b = 0; b < B; b++) {
        for (int l = 0; l < L; l++) {
            for (int h = 0; h < H; h++) {
                for (int k = 0; k < d_k; k++) {
                    int idx = h * d_k + k;
                    X_perm[b][h][l][k] = X[b][l][idx];
                }
            }
        }
    }

    // Imprimir resultado
    for (int b = 0; b < B; b++) {
        for (int h = 0; h < H; h++) {
            printf("Head %d:\n", h);
            for (int l = 0; l < L; l++) {
                printf("  Token %d: [", l);
                for (int k = 0; k < d_k; k++) {
                    printf(" %.0f", X_perm[b][h][l][k]);
                }
                printf(" ]\n");
            }
        }
    }

    return 0;
}
