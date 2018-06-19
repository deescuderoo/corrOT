//
// Created by daniel on 18/06/18.
//

#ifndef OT1_INTMOD_HPP
#define OT1_INTMOD_HPP

#include <iostream>
#include <libscapi/include/interactive_mid_protocols/OTExtensionBristol.hpp>
#include <libscapi/include/primitives/Prg.hpp>

/// Holds a class for computation modulo powers of 2
/// The current supported modulus are 2^32, 2^64 and 2^128
template <class T, int pwr>
class Z2k {
public:
    // Constructors
    Z2k() : m_data(T()){};
    Z2k(T val) : m_data(val){
        assert (sizeof(T) == pwr/8);
    };
    Z2k(byte * bytes){
//        assert(sizeof(*bytes) == sizeof(T));
        auto ptr = (T *) bytes;
        m_data = *ptr;
    };
    Z2k(vector<byte> bytes){
        assert(bytes.size() == sizeof(T));
        byte buffer[sizeof(T)];
        copy(bytes.begin(), bytes.end(), buffer);
        auto ptr = (T *) buffer;
        m_data = *ptr;
    };

    // Attributes
    T m_data;

    // Operators
    Z2k operator+(Z2k rhs);
    Z2k operator*(Z2k rhs);
//    Z2k operator*(vZ2k rhs);
    };

template <class T, int pwr>
class vZ2k {
public:
    // Constructors
    vZ2k() : m_data(vector<Z2k<T, pwr>>()), size(uint()){};
    vZ2k(vector<Z2k<T, pwr>> data) : m_data(data), size(data.size()){
        assert (sizeof(T) == pwr/8);
    };
    vZ2k(vector<vector<byte>> data){
        for (vector<byte> entry : data){
            m_data.push_back(Z2k<T,pwr>(entry));
        }
    };

    // Attributes
    vector<Z2k<T, pwr>> m_data;
    unsigned long size;

    // Operators
    vZ2k operator+(vZ2k rhs);
//    Z2k operator*(Z2k rhs);

    // Factory
//    static vZ2k CreateFromRandom();

};

template<class T, int pwr>
Z2k<T, pwr> Z2k<T, pwr>::operator+(Z2k<T, pwr> rhs) {
    return Z2k(this->m_data + rhs.m_data);
}

template<class T, int pwr>
Z2k<T, pwr> Z2k<T, pwr>::operator*(Z2k<T, pwr> rhs) {
    return Z2k(this->m_data * rhs.m_data);
}

//template<class T, int pwr>
//Z2k<T, pwr> Z2k<T, pwr>::operator*(vZ2k<T, pwr> rhs) {
//    vector<Z2k<T, pwr>> result(rhs.size);
//    for (int i = 0; i < rhs.size; i++){
//        result[i] = this->m_data * rhs.m_data[i];
//    }
//    return result;
//}

template<class T, int pwr>
vZ2k<T, pwr> vZ2k<T, pwr>::operator+(vZ2k<T, pwr> rhs) {
    assert(this->size == rhs.size);
    vector<Z2k<T, pwr>> result(this->size);
    for (int i = 0; i < this->size; i++){
        result[i] = this->m_data[i] + rhs.m_data[i];
    }
    return result;
}

//template<class T, int pwr>
//vZ2k<T, pwr>::vZ2k(vector<vector<byte>> data) : size(data.size()) {
//
//}

#endif //OT1_INTMOD_HPP
