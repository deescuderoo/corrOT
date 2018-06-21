//
// Created by daniel on 18/06/18.
//

#ifndef OT1_INTMOD_HPP
#define OT1_INTMOD_HPP

#include <iostream>
#include <libscapi/include/interactive_mid_protocols/OTExtensionBristol.hpp>
#include <libscapi/include/primitives/Prg.hpp>

template <class T, int pwr>
class vZ2k;

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
    Z2k operator-(Z2k rhs);
    Z2k operator*(Z2k rhs);
    Z2k operator*(vZ2k<T,pwr> rhs);

    // Methods
    void subtract(Z2k<T, pwr> & rhs);
    void add(Z2k<T, pwr> & rhs);

};

template <class T, int pwr>
class vZ2k {
public:
    // Constructors
    vZ2k() : m_data(vector<Z2k<T, pwr>>()){};
    vZ2k(int size) : m_data(vector<Z2k<T, pwr>>(size)){};
    vZ2k(vector<Z2k<T, pwr>> & data) : m_data(data){
        assert (sizeof(T) == pwr/8);
    };
    vZ2k(vector<vector<byte>> data) : m_data(vector<Z2k<T, pwr>>(data.size())){
        for (int i = 0; i < data.size(); i++){
            m_data[i] = Z2k<T,pwr>(data[i]);
        }
    };

    // Attributes
    vector<Z2k<T, pwr>> m_data;


    // Operators
    vZ2k operator+(vZ2k rhs); //TODO: Write space-saving operations
    vZ2k operator-(vZ2k rhs);
//    vZ2k operator*(Z2k rhs);

    // Methods;
    void subtract(vZ2k<T, pwr> & rhs, vZ2k<T, pwr> & result);
    void add(vZ2k<T, pwr> & rhs, vZ2k<T, pwr> & result);

    vector<Z2k<T, pwr>> getVector() {
        return m_data;
    };
    byte * getBytePtr() {
        return (byte *)&(m_data[0].m_data);
    };
    unsigned int getLength() {
        return m_data.size();
    };

    static void printVector(vZ2k<T, pwr> vect) {
        for (int i = 0; i < vect.m_data.size(); i++) {
            cout << vect.m_data[i].m_data << endl;
        }
    }

    // Factory
//    static vZ2k CreateFromRandom();

};

#include "IntMod.tpp"

#endif //OT1_INTMOD_HPP
