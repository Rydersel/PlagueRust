#ifndef _MATRIX_HPP
#define _MATRIX_HPP

/* Represents a 4x4 matrix */
struct Matrix
{
    union
    {
        struct
        {
            float        _11, _12, _13, _14;
            float        _21, _22, _23, _24;
            float        _31, _32, _33, _34;
            float        _41, _42, _43, _44;

        };
        float m[4][4];
    };
};


/* Used to transpose a 4x4 matrix */
Matrix* MatrixTranspose(Matrix* pout, const Matrix* pm);


#endif // _MATRIX_HPP