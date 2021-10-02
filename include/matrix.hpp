#ifndef TR_INCLUDE_MATRIX_H_
#define TR_INCLUDE_MATRIX_H_

#include <array>
#include <cassert>
#include <cmath>
#include <iostream>

namespace TrMatrix {
namespace Base {
    template <typename _element_type, int _rows, int _cols>
    class Matrix {
        using _this_type = Matrix<_element_type, _rows, _cols>;
        using _ds_type = std::array<_element_type, _rows * _cols>;

        _ds_type _data;

        inline static int _get_pos(const int &x, const int &y) { return x * _cols + y; }

    public:
        // constructor
        Matrix() : _data(){};
        Matrix(const _element_type &x) : _data() {
            _data[0] = x;
        }
        Matrix(const _element_type &x, const _element_type &y) : _data() {
            _data[0] = x;
            _data[1] = y;
        }
        Matrix(const _element_type &x, const _element_type &y, const _element_type &z) : _data() {
            _data[0] = x;
            _data[1] = y;
            _data[2] = z;
        }

        Matrix(const std::initializer_list<_element_type> &list) : _data() {
            assert(list.size() <= _rows * _cols);
            int i = 0;
            for (const auto &elem : list) {
                _data[i++] = elem;
            }
        };

        Matrix(const Matrix &mat) : _data(mat.data()){};

        // util function
        inline const std::pair<int, int> dim() { return {_rows, _cols}; }
        inline _ds_type &data() { return _data; }
        inline const _ds_type &data() const { return _data; }

        inline _element_type &x() { return _data[0]; }
        inline const _element_type &x() const { return _data[0]; }
        inline _element_type &y() { return _data[1]; }
        inline const _element_type &y() const { return _data[1]; }
        inline _element_type &z() { return _data[2]; }
        inline const _element_type &z() const { return _data[2]; }
        inline _element_type &w() { return _data[3]; }
        inline const _element_type &w() const { return _data[3]; }

        // operator
        bool operator==(const _this_type &rhs) const {
            for (int i = 0; i < _rows * _cols; ++i) {
                if (this->_data[i] != rhs._data[i]) {
                    return false;
                }
            }
            return true;
        }

        bool operator!=(const _this_type &rhs) const {
            return !(*this == rhs);
        }

        _this_type operator-() const {
            _this_type ret;
            for (int i = 0; i < _rows * _cols; ++i) {
                ret._data[i] = -this->_data[i];
            }
            return ret;
        }

        _this_type &operator+=(const _this_type &rhs) {
            for (int i = 0; i < _rows * _cols; ++i) {
                this->_data[i] += rhs._data[i];
            }
            return *this;
        }

        _this_type &operator*=(const _element_type &val) {
            for (int i = 0; i < _rows * _cols; ++i) {
                this->_data[i] *= val;
            }
            return *this;
        }

        _this_type &operator/=(const _element_type &val) { return *this *= (1.0 / val); }

        _this_type &operator-=(const _this_type &rhs) { return *this += (-rhs); }

        _this_type operator+(const _this_type &rhs) const {
            _this_type ret(*this);
            ret += rhs;
            return ret;
        }

        _this_type operator-(const _this_type &rhs) const { return *this + (-rhs); }

        _this_type operator*(const _element_type &val) const {
            _this_type ret(*this);
            ret *= val;
            return ret;
        }

        _this_type operator/(const _element_type &val) const {
            _this_type ret(*this);
            ret /= val;
            return ret;
        }

        _element_type &operator()(const int &x, const int &y) { return _data[_get_pos(x, y)]; }
        const _element_type &operator()(const int &x, const int &y) const { return _data[_get_pos(x, y)]; }

        _element_type &operator()(const int &idx) { return _data[idx]; }
        const _element_type &operator()(const int &idx) const { return _data[idx]; }
    };

#define getMatrixType(Type, TypeSuffix, Size, SizeSuffix)            \
    using Matrix##SizeSuffix##TypeSuffix = Matrix<Type, Size, Size>; \
    using Vector##SizeSuffix##TypeSuffix = Matrix<Type, Size, 1>;

#define getAllMatrixType(Type, TypeSuffix) \
    getMatrixType(Type, TypeSuffix, 2, 2); \
    getMatrixType(Type, TypeSuffix, 3, 3); \
    getMatrixType(Type, TypeSuffix, 4, 4);

    getAllMatrixType(float, f);
    getAllMatrixType(double, d);
    getAllMatrixType(int, i);

} // namespace Base
namespace Util {

    // Matrix multiple
    template <typename _element_type, int _M, int _N, int _P>
    Base::Matrix<_element_type, _M, _P> operator*(const Base::Matrix<_element_type, _M, _N> &lhs,
                                                  const Base::Matrix<_element_type, _N, _P> &rhs) {
        Base::Matrix<_element_type, _M, _P> ret;
        for (int i = 0; i < _M; ++i) {
            for (int j = 0; j < _P; ++j) {
                for (int k = 0; k < _N; ++k) {
                    ret(i, j) += lhs(i, k) * rhs(k, j);
                }
            }
        }
        return ret;
    }

    template <typename _element_type, int _rows, int _cols>
    Base::Matrix<_element_type, _rows, _cols> operator*(const _element_type &lhs, const Base::Matrix<_element_type, _rows, _cols> &rhs) {
        return rhs * lhs;
    }

    // output a Matrix
    template <typename _element_type, int _rows, int _cols>
    std::ostream &operator<<(std::ostream &os, const Base::Matrix<_element_type, _rows, _cols> &mat) {
        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                os << mat(i, j) << ' ';
            }
            os << "\n";
        }
        return os;
    }

    // Square of vector's length
    template <typename _elem_type, int _size>
    _elem_type LengthSquared(const Base::Matrix<_elem_type, _size, 1> &vec) {
        _elem_type ret = 0.0;
        for (int i = 0; i < _size; ++i) {
            ret += vec(i) * vec(i);
        }
        return ret;
    }

    // Vector's length
    template <typename _elem_type, int _size>
    _elem_type Length(const Base::Matrix<_elem_type, _size, 1> &vec) {
        return std::sqrt(LengthSquared(vec));
    }

    // Vector dot product
    template <typename _elem_type, int _size>
    _elem_type DotProduct(const Base::Matrix<_elem_type, _size, 1> &lhs, const Base::Matrix<_elem_type, _size, 1> &rhs) {
        _elem_type ret = 0.0;
        for (int i = 0; i < _size; ++i) {
            ret += lhs(i) * rhs(i);
        }
        return ret;
    }

    // Vector cross product
    template <typename _elem_type>
    Base::Matrix<_elem_type, 3, 1> CrossProduct3D(const Base::Matrix<_elem_type, 3, 1> &lhs, const Base::Matrix<_elem_type, 3, 1> &rhs) {
        return {
        lhs.y() * rhs.z() - lhs.z() * rhs.y(),
        lhs.z() * rhs.x() - lhs.x() * rhs.z(),
        lhs.x() * rhs.y() - lhs.y() * rhs.x()};
    }

    // Normalize a vector
    template <typename _elem_type, int _size>
    Base::Matrix<_elem_type, _size, 1> Normalize(const Base::Matrix<_elem_type, _size, 1> &vec) {
        return vec / Length(vec);
    }

    // Get square root of each element
    template <typename _elem_type, int _rows, int _cols>
    Base::Matrix<_elem_type, _rows, _cols> Sqrt(const Base::Matrix<_elem_type, _rows, _cols> &mat) {
        Base::Matrix<_elem_type, _rows, _cols> ret;
        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                assert(mat(i, j) >= 0.0);
                ret(i, j) = std::sqrt(mat(i, j));
            }
        }
        return ret;
    }

    template <typename _elem_type, int _rows, int _cols>
    Base::Matrix<_elem_type, _rows, _cols> HadamardProduct(const Base::Matrix<_elem_type, _rows, _cols> &lhs, const Base::Matrix<_elem_type, _rows, _cols> &rhs) {
        Base::Matrix<_elem_type, _rows, _cols> ret;
        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                ret(i, j) = lhs(i, j) * rhs(i, j);
            }
        }
        return ret;
    }

    template <typename _elem_type, int _rows, int _cols>
    Base::Matrix<_elem_type, _rows, _cols> Inverse(const Base::Matrix<_elem_type, _rows, _cols> &mat) {
        Base::Matrix<_elem_type, _rows, _cols> ret;
        for (int i = 0; i < _rows; ++i) {
            for (int j = 0; j < _cols; ++j) {
                ret(i, j) = 1.0 / mat(i, j);
            }
        }
        return ret;
    }

}; // namespace Util
}; // namespace TrMatrix

using Color3f = TrMatrix::Base::Vector3f;
using Color3d = TrMatrix::Base::Vector3d;
using Point3f = TrMatrix::Base::Vector3f;
using Point3d = TrMatrix::Base::Vector3d;

#endif