#include "Matrix.hpp"
#include <algorithm> // For std::max (used in Cholesky decomposition)

// Default constructor
Matrix::Matrix() : rows(0), cols(0), data() {}

// Zero-initialized matrix of given dimensions
Matrix::Matrix(size_t r, size_t c) : rows(r), cols(c), data(r, std::vector<double>(c, 0.0)) {}

// Construct from 2D vector with size validation
Matrix::Matrix(const std::vector<std::vector<double>>& d) : data(d), rows(d.size()), cols(0) {
    if (rows > 0) 
    {
        cols = d[0].size();
        for (const auto& row : d)
            if (row.size() != cols)
                throw std::invalid_argument("All rows must have the same number of columns.");
    }
}

// Dimension accessors
size_t Matrix::num_rows() const { return rows; }
size_t Matrix::num_cols() const { return cols; }

// Element access
double& Matrix::operator()(size_t r, size_t c) { return data[r][c]; }
double Matrix::operator()(size_t r, size_t c) const { return data[r][c]; }

// Stream output operator
std::ostream& operator<<(std::ostream& out, const Matrix& mat) 
{
    for (size_t i = 0; i < mat.rows; ++i) {
        for (size_t j = 0; j < mat.cols; ++j)
            out << std::setw(12) << std::fixed << std::setprecision(6) << mat(i, j) << " ";
        out << std::endl;
    }
    return out;
}

// Transpose of the matrix
Matrix Matrix::transpose() const 
{
    Matrix result(cols, rows);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            result(j, i) = data[i][j];
    return result;
}

// Matrix multiplication (dot product)
Matrix Matrix::dot(const Matrix& other) const 
{
    if (cols != other.rows)
        throw std::invalid_argument("Matrix dimension mismatch for multiplication.");
    
    Matrix result(rows, other.cols);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < other.cols; ++j)
            for (size_t k = 0; k < cols; ++k)
                result(i, j) += data[i][k] * other(k, j);
    return result;
}

// Scalar multiplication
Matrix Matrix::operator*(double scalar) const 
{
    Matrix result(*this);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            result(i, j) *= scalar;
    return result;
}

// Matrix addition
Matrix Matrix::operator+(const Matrix& other) const 
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match for addition.");
    
    Matrix result(*this);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            result(i, j) += other(i, j);
    return result;
}

// Matrix subtraction
Matrix Matrix::operator-(const Matrix& other) const 
{
    if (rows != other.rows || cols != other.cols)
        throw std::invalid_argument("Matrix dimensions must match for subtraction.");
    
    Matrix result(*this);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            result(i, j) -= other(i, j);
    return result;
}

// Cholesky decomposition: A = L * Lᵀ
Matrix Matrix::cholesky_decompose() const 
{
    if (rows != cols)
        throw std::invalid_argument("Cholesky decomposition requires a square matrix.");

    size_t n = rows;
    Matrix L(n, n);

    for (size_t i = 0; i < n; ++i) 
    {
        for (size_t j = 0; j <= i; ++j) 
        {
            double sum = 0.0;
            for (size_t k = 0; k < j; ++k)
                sum += L(i, k) * L(j, k);

            if (i == j) 
            {
                double val = data[i][i] - sum;
                if (val <= 0.0)
                    throw std::runtime_error("Matrix is not positive-definite.");
                L(i, j) = std::sqrt(val);
            } 
            else 
            {
                L(i, j) = (data[i][j] - sum) / L(j, j);
            }
        }
    }
    return L;
}

// Inverse using Cholesky decomposition
Matrix Matrix::inverse() const 
{
    Matrix L = cholesky_decompose();
    size_t n = L.num_rows();
    Matrix invL(n, n), invA(n, n);

    // Forward substitution to compute L⁻¹
    for (size_t i = 0; i < n; ++i) 
    {
        invL(i, i) = 1.0 / L(i, i);
        for (size_t j = i + 1; j < n; ++j) 
        {
            double sum = 0.0;
            for (size_t k = i; k < j; ++k)
                sum -= L(j, k) * invL(k, i);
            invL(j, i) = sum / L(j, j);
        }
    }

    // Final inverse: A⁻¹ = (L⁻¹)ᵀ * L⁻¹
    invA = invL.transpose().dot(invL);
    return invA;
}

// Mean of each column
std::vector<double> Matrix::mean_per_column() const 
{
    if (rows == 0 || cols == 0) return {};
    
    std::vector<double> mean(cols, 0.0);
    for (size_t j = 0; j < cols; ++j)
        for (size_t i = 0; i < rows; ++i)
            mean[j] += data[i][j];

    for (double& m : mean)
        m /= rows;

    return mean;
}

// Unbiased sample covariance matrix (N x N)
Matrix Matrix::covariance_matrix() const 
{
    if (rows < 2)
        throw std::runtime_error("At least 2 rows required for covariance matrix.");

    std::vector<double> means = mean_per_column();
    Matrix cov(cols, cols);

    for (size_t i = 0; i < cols; ++i) 
    {
        for (size_t j = 0; j < cols; ++j) 
        {
            double sum = 0.0;
            for (size_t k = 0; k < rows; ++k)
                sum += (data[k][i] - means[i]) * (data[k][j] - means[j]);
            cov(i, j) = sum / (rows - 1);
        }
    }
    return cov;
}