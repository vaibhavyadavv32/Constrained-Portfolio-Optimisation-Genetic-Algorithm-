#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <cmath>

class Matrix 
{
private:
    size_t rows, cols;
    std::vector<std::vector<double>> data;

    // Performs Cholesky decomposition (lower triangular)
    Matrix cholesky_decompose() const;

public:
    // Constructors
    Matrix();                                                 // Empty matrix
    Matrix(size_t r, size_t c);                                // Zero matrix of size r x c
    Matrix(const std::vector<std::vector<double>>& d);         // Initialize with data

    // Element access
    double& operator()(size_t r, size_t c);
    double operator()(size_t r, size_t c) const;

    // Dimension info
    size_t num_rows() const;
    size_t num_cols() const;

    // Basic matrix operations
    Matrix transpose() const;
    Matrix dot(const Matrix& other) const;
    Matrix operator*(double scalar) const;
    Matrix operator+(const Matrix& other) const;
    Matrix operator-(const Matrix& other) const;

    // Matrix inverse using Cholesky decomposition
    Matrix inverse() const;

    // Statistical operations
    std::vector<double> mean_per_column() const;
    Matrix covariance_matrix() const;

    // Output stream
    friend std::ostream& operator<<(std::ostream& out, const Matrix& mat);
};

#endif // MATRIX_HPP