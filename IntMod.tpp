template<class T, int pwr>
Z2k<T, pwr> Z2k<T, pwr>::operator+(Z2k<T, pwr> rhs) {
    return Z2k(this->m_data + rhs.m_data);
}

template<class T, int pwr>
Z2k<T, pwr> Z2k<T, pwr>::operator-(Z2k<T, pwr> rhs) {
    return Z2k(this->m_data - rhs.m_data);
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
    assert(this->getLength() == rhs.getLength());
    vector<Z2k<T, pwr>> result(this->getLength());
    for (int i = 0; i < this->getLength(); i++){
        result[i] = this->m_data[i] + rhs.m_data[i];
    }
    return result;
}

template<class T, int pwr>
vZ2k<T, pwr> vZ2k<T, pwr>::operator-(vZ2k<T, pwr> rhs) {
    assert(this->getLength() == rhs.getLength());
    vector<Z2k<T, pwr>> result(this->getLength());
    for (int i = 0; i < this->getLength(); i++){
        result[i] = this->m_data[i] - rhs.m_data[i];
    }
    return result;
}

//template<class T, int pwr>
//vZ2k<T, pwr>::vZ2k(vector<vector<byte>> data) : size(data.size()) {
//
//}