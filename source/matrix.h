#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>

class Matrix {
public:
    Matrix();
    Matrix(const char* file);
    Matrix(std::istream& in);

    void print(std::ostream& out);
    void print(const char* file);

    size_t getDim() const;
    std::string getName(size_t index) const;
    std::vector<std::string> getNames() const;

    long double& at(size_t i, size_t j);
    const long double& at(size_t i, size_t j) const;
    long double& at(const std::string& x, const std::string& y);

    bool operator==(const Matrix& other) const;
    bool operator!=(const Matrix& other) const;

private:
    size_t dim_;
    std::vector<std::vector<long double>> matrix_;
    std::vector<std::string> names_;
    std::unordered_map<std::string, size_t> nameToIndex_;
};
