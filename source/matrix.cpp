#include <vector>
#include <cmath>
#include <stdexcept>
#include <iomanip>

#include "matrix.h"

void indexValidation(size_t index, size_t max) {
    if (index < 0 || index >= max) {
        throw std::out_of_range("");
    }
}

void nameValidation(const std::string& name, const std::unordered_map<std::string, size_t>& dict) {
    if (!dict.count(name)) {
        throw std::out_of_range("");
    }
}

bool equal(const long double& x, const long double& y) {
    long double eps = 1e-9;
    return abs(x - y) < eps;
}

Matrix::Matrix() : dim_(0), matrix_(), names_() {
}

Matrix::Matrix(const char* file) {
    std::ifstream in;
    in.open(file);
    *this = Matrix(in);
    in.close();
}

Matrix::Matrix(std::istream& in) {
    size_t n;
    in >> n;

    dim_ = n;
    matrix_.resize(n);
    names_.resize(n);

    for (size_t i = 0; i < n; i++) {
        in >> names_[i];
        nameToIndex_[names_[i]] = i;

        matrix_[i].resize(i + 1);
        for (size_t j = 0; j < n; j++) {
            long double x;
            in >> x;
            if (j <= i) {
                at(i, j) = x;
            }
        }
    }
}

void Matrix::print(const char* file) {
    std::ofstream out;
    out.open(file);
    print(out);
    out.close();
}

void Matrix::print(std::ostream& out) {
    out << dim_ << '\n';
    for (size_t i = 0; i < dim_; i++) {
        out << names_[i] << '\t';
        for (size_t j = 0; j < dim_; j++) {
            out << std::fixed << std::setprecision(12) << at(i, j) << '\t';
        }
        out << '\n';
    }
};

size_t Matrix::getDim() const {
    return dim_;
}

std::string Matrix::getName(size_t index) const {
    indexValidation(index, dim_);
    return names_[index];
}

std::vector<std::string> Matrix::getNames() const {
    return names_;
}

long double& Matrix::at(size_t i, size_t j) {
    indexValidation(i, dim_);
    indexValidation(j, dim_);
    return i > j ? matrix_[i][j] : matrix_[j][i];
}

const long double& Matrix::at(size_t i, size_t j) const {
    indexValidation(i, dim_);
    indexValidation(j, dim_);
    return i > j ? matrix_[i][j] : matrix_[j][i];
}

long double& Matrix::at(const std::string& x, const std::string& y) {
    nameValidation(x, nameToIndex_);
    nameValidation(y, nameToIndex_);
    return at(nameToIndex_[x], nameToIndex_[y]);
}

bool Matrix::operator==(const Matrix& other) const {
    if (this == &other) {
        return true;
    }

    if (dim_ != other.dim_ ) {
        return false;
    }

    for (int i = 0; i < dim_; i++) {
        for (int j = 0; j < dim_; j++) {
            if (!equal(at(i, j), other.at(i, j))) {
                return false;
            }
        }
    }
    return true;
}

bool Matrix::operator!=(const Matrix& other) const {
    return !(*this == other);
}
