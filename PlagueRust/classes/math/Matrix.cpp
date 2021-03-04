#include "Matrix.hpp"


/* Implementation for MatrixTranspose */
Matrix* MatrixTranspose(Matrix* pout, const Matrix* pm)
{
    const Matrix m = *pm;
    int i, j;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++) pout->m[i][j] = m.m[j][i];

    return pout;
}