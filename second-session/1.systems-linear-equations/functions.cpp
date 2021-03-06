#include "functions.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <gsl/gsl_linalg.h>

const int PRINT_WIDTH = 13;
const int PRINT_PRECISION = 5;

void printVector(const gsl_vector * v, std::string string) {
    std::cout << "Vector " << string << ":\n";
    for (unsigned int i = 0; i < v->size; i++) {
        std::cout << std::setw(PRINT_WIDTH) << std::setprecision(PRINT_PRECISION) << gsl_vector_get(v, i) << "\n";
    }
    std::cout << "\n";
}

void printVector(const gsl_vector * v, std::string string, std::ostream &out) {
    out << "Vector " << string << ":\n";
    for (unsigned int i = 0; i < v->size; i++) {
        out << std::setw(PRINT_WIDTH) << std::setprecision(PRINT_PRECISION) << gsl_vector_get(v, i) << "\n";
    }
    out << "\n";
}

void printVectorCoutAndFile(const gsl_vector * v, std::string string, std::ostream &out) {
    std::cout << "Vector " << string << ":\n";
    out << "Vector " << string << ":\n";
    for (unsigned int i = 0; i < v->size; i++) {
        std::cout << std::setw(PRINT_WIDTH) << std::setprecision(PRINT_PRECISION) << gsl_vector_get(v, i) << "\n";
        out << std::setw(PRINT_WIDTH) << std::setprecision(PRINT_PRECISION) << gsl_vector_get(v, i) << "\n";
    }
    std::cout << "\n";
    out << "\n";
}

void printMatrix(const gsl_matrix *m, std::string string) {
    std::cout << "Matrix " << string << ":\n";

    for (unsigned int i = 0; i < m->size1; i++) {
        for (unsigned int j = 0; j < m->size2; j++) {
            std::cout << std::setw(PRINT_WIDTH) << std::setprecision(PRINT_PRECISION) << gsl_matrix_get(m, i, j);
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

void printMatrix(const gsl_matrix *m, std::string string, std::ostream &out) {
    out << "Matrix " << string << ":\n";
    for (unsigned int i = 0; i < m->size1; i++) {
        for (unsigned int j = 0; j < m->size2; j++) {
            out << std::setw(PRINT_WIDTH) << std::setprecision(PRINT_PRECISION) << gsl_matrix_get(m, i, j);
        }
        out << "\n";
    }
    out << "\n";
}

void printMatrixCoutAndFile(const gsl_matrix *m, std::string string, std::ostream &out) {
    std::cout << "Matrix " << string << ":\n";
    out << "Matrix " << string << ":\n";
    for (unsigned int i = 0; i < m->size1; i++) {
        for (unsigned int j = 0; j < m->size2; j++) {
            std::cout << std::setw(PRINT_WIDTH) << std::setprecision(PRINT_PRECISION) << gsl_matrix_get(m, i, j);
            out << std::setw(PRINT_WIDTH) << std::setprecision(PRINT_PRECISION) << gsl_matrix_get(m, i, j);
        }
        std::cout << "\n";
        out << "\n";
    }
    std::cout << "\n";
    out << "\n";
}

void solve(gsl_matrix *A, gsl_vector *Y, std::ostream &out) {
    size_t size_i = A->size1;
    size_t size_j = A->size2;

    // initialize all the necessary matrices and vectors
    gsl_matrix *LU = gsl_matrix_alloc(size_i, size_j),
               *U  = gsl_matrix_alloc(size_i, size_j),
               *V  = gsl_matrix_alloc(size_j, size_j);

    gsl_vector *X    = gsl_vector_alloc(size_j),
               *r    = gsl_vector_alloc(size_j),
               *S    = gsl_vector_alloc(size_j),
               *work = gsl_vector_alloc(size_j);

    // gsl_permutation and pInt are necessary to do the LU decomposition
    gsl_permutation *P = gsl_permutation_alloc(size_i);
    int sigNum;

    // copy the contents in matrix A to matrices LU and U
    // we will work with LU and U so A won't be overwritten
    gsl_matrix_memcpy(LU, A);
    gsl_matrix_memcpy(U, A);

    // do the LU decomposition
    // the algorithm used in the decomposition is Gaussian Elimination with partial pivoting
    gsl_linalg_LU_decomp(LU, P, &sigNum);

    // solve the system of equations, put the result in X and the residual vector in r
    gsl_linalg_LU_solve(LU, P, Y, X);
    gsl_linalg_LU_refine(A, LU, P, Y, X, r);

    // do a singular value decomposition, we will use values of S to calculate the condition number of A
    gsl_linalg_SV_decomp(U, V, S, work);

    // the condition number we will use is max(S) / min(S)
    double condNumber, minS, maxS;
    minS = gsl_vector_get(S, 0); maxS = gsl_vector_get(S, 0);
    for (int j = 0; j < size_j; j++) {
        if (gsl_vector_get(S, j) < minS) minS = gsl_vector_get(S, j);
        if (gsl_vector_get(S, j) > maxS) maxS = gsl_vector_get(S, j);
    }
    condNumber = fabs(maxS) / fabs(minS);

    // write out all of the results
    printMatrixCoutAndFile(A,  "Input Matrix A", out);
    printVectorCoutAndFile(Y,  "Input Vector Y", out);
    printMatrixCoutAndFile(LU, "LU, result of LU decomposition", out);
    printVectorCoutAndFile(X,  "X, result of solving with LU decomposition", out);
    printVectorCoutAndFile(r,  "r, residual of solving by LU decomposition", out);
    printVectorCoutAndFile(S,  "S, singular values of A, result of doing SV decomposition", out);

    std::cout << "Calculating condition number by: abs(max(singular values)) / abs(min(singular values)):\n\t";
    std::cout << "Condition number = " << fabs(maxS) << " / " << fabs(minS) << " = " << condNumber << "\n";
    out << "Calculating condition number by: abs(max(singular values)) / abs(min(singular values)):\n\t";
    out << "Condition number = " << fabs(maxS) << " / " << fabs(minS) << " = " << condNumber << "\n\n";

    // free the memory
    gsl_matrix_free(LU); gsl_matrix_free(U); gsl_matrix_free(V);
    gsl_vector_free(X); gsl_vector_free(r); gsl_vector_free(S); gsl_vector_free(work);
    gsl_permutation_free(P);
}


// formula: sum_{j=1}^{n} ((1+i)^(j-1)) * x_j   =   ((1+i)^n - 1) / i
double formulaLeft(double i, double j)
{
    return pow(1+i, j-1);
}

// formula: sum_{j=1}^{n} ((1+i)^(j-1)) * x_j   =   ((1+i)^n - 1) / i
double formulaRight(double i, int n)
{
    return (pow(1+i, n) - 1) / i;
}


double checkValuesInFormula(double i, int n)
{
    double leftSide = 0, rightSide = 0;
    //i *= drand48()*100;

    for (int j = 1; j <= n; j++) {
        leftSide += formulaLeft(i, j);
    }
    rightSide = formulaRight(i, n);

    std::cout << "i = "           <<  std::setw(5) << std::setprecision(3) << i << ", n = " << n
              << ", left side:  " << std::setw(10) << std::setprecision(5) << leftSide
              << ", right side: " << std::setw(10) << std::setprecision(5) << rightSide << std::endl;
}


void fillSystem(gsl_matrix* A, gsl_vector *Y, int n)
{
    for (size_t row = 0; row < n; ++row) {
        for (size_t col = 0; col < n; ++col) {
            gsl_matrix_set(A, row, col, formulaLeft(row+1, col+1));
        }
        gsl_vector_set(Y, row, formulaRight(row+1, n));
        //checkValuesInFormula(row+1, n);
    }
}
