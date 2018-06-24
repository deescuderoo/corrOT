template<class T, int pwr>
void SenderOT<T,pwr>::run_baseOT(vector<vector<byte>> data0, vector<vector<byte>> data1, int nOT, int elementSizeBits) {
//    byte * p0 = data0[0].bytes;
//    byte * p1 = data1[0].bytes;
//
//    vector<byte> data0_;
//    vector<byte> data1_;
//    copy(p0, p0 + data0.size() * sizeof(data_t), back_inserter(data0_));
//    copy(p1, p1 + data1.size() * sizeof(data_t), back_inserter(data1_));

    OTExtensionBristolSender sender(12001, true, this->getChannel());
    OTBatchSInput * input = new OTExtensionRandomizedSInput(nOT, elementSizeBits);

    auto output = sender.transfer(input);

    auto output_ = ((OTExtensionRandomizedSOutput *) output.get());
    vector<byte> outputbytes0 = output_->getR0Arr();
    vector<byte> outputbytes1 = output_->getR1Arr();

//    cout << "Strings " << outputbytes0.size() << endl;
//    printN(outputbytes0);
//    printN(outputbytes1);

    keys0_bOT = vectorConversion(outputbytes0, CONST_k, SIZE_OT);
    keys1_bOT = vectorConversion(outputbytes1, CONST_k, SIZE_OT);
}

template<class T, int pwr>
void SenderOT<T,pwr>::sampleCorrelation() {
    int length = CONST_n + CONST_k_;
    int size = (pwr * length)/8;
    vector<byte> buffer(size);
//    memcpy(&buffer[0], prg->getPRGBytesEX(size), size);

    prg->getBytes(buffer.data(), size);
    this->correlation = vZ2k<T,pwr>(vectorConversion(buffer,length,pwr/8)); // Double copy

//    cout << "Correlation" << endl;
//    vZ2k<T, pwr>::printVector(this->correlation);
}

template<class T, int pwr>
void SenderOT<T,pwr>::applyPRF() {
    int sizeBytes = (CONST_pwr * (CONST_n + CONST_k_))/8;
    vector<byte> tmp0(sizeBytes);
    vector<byte> tmp1(sizeBytes);

    for (int i = 0; i < keys0_bOT.size(); i++) {
//        cout << "Key 0," << i << endl;
//        printN(keys0_bOT[i]);
//        cout << "Key 1," << i << endl;
//        printN(keys1_bOT[i]);

        prfCall(prf, keys0_bOT[i], tmp0, sizeBytes);
        prfCall(prf, keys1_bOT[i], tmp1, sizeBytes);

        t0[i] = vZ2k<T,pwr>(vectorConversion(tmp0, CONST_n + CONST_k_, pwr/8));
        t1[i] = vZ2k<T,pwr>(vectorConversion(tmp1, CONST_n + CONST_k_, pwr/8));
    }
};

template<class T, int pwr>
void SenderOT<T,pwr>::sendUi() {
    int length = sizeof(Z2k<T,pwr>) * t0[0].getLength();
    vZ2k<T,pwr> tmpVector(CONST_n + CONST_k_);
    vZ2k<T,pwr> ui(CONST_n + CONST_k_);

    for (int i = 0; i < CONST_k; i++){
        assert(correlation.getLength() == t0[0].getLength());
        assert(correlation.getLength() == t1[0].getLength());

        t0[i].add(t1[i], tmpVector);
        tmpVector.add(correlation, ui);
        byte * ptr = ui.getBytePtr();
//        printN(ptr, length);
        getChannel()->write(ptr, length);
    }
};

template<class T, int pwr>
void SenderOT<T,pwr>::generateHashes() {
    int size = t0[0].getLength() * sizeof(Z2k<T,pwr>);
    vZ2k<T,pwr> tmpVector00(CONST_n + CONST_k_);
    vZ2k<T,pwr> tmpVector01(CONST_n + CONST_k_);
    vZ2k<T,pwr> tmpVector10(CONST_n + CONST_k_);
    vZ2k<T,pwr> tmpVector11(CONST_n + CONST_k_);


    for (int alpha = 0; alpha < CONST_k; alpha++){
        for (int beta = 0; beta < CONST_k; beta++){
//            cout << alpha << " " << beta << endl;
            t0[alpha].subtract(t0[beta], tmpVector00);
            t0[alpha].subtract(t1[beta], tmpVector01);
            t1[alpha].subtract(t0[beta], tmpVector10);
            t1[alpha].subtract(t1[beta], tmpVector11);

            Hash_value(hash, tmpVector00.getBytePtr(), h00 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), size);
            Hash_value(hash, tmpVector01.getBytePtr(), h01 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), size);
            Hash_value(hash, tmpVector10.getBytePtr(), h10 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), size);
            Hash_value(hash, tmpVector11.getBytePtr(), h11 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), size);

//            printN(h00 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), 32);
//            printf("%p\n", (void*)h00);
        }
    }
    /*cout << "---------------- 00 ----------------" << endl;
    vZ2k<T,pwr>::printVector(t0[0] - t0[1]);
    cout << "---------------- 01 ----------------" << endl;
    vZ2k<T,pwr>::printVector(t0[0] - t1[1]);
    cout << "---------------- 10 ----------------" << endl;
    vZ2k<T,pwr>::printVector(t1[0] - t0[1]);
    cout << "---------------- 11 ----------------" << endl;
    vZ2k<T,pwr>::printVector(t1[0] - t1[1]);*/
}

template<class T, int pwr>
void SenderOT<T,pwr>::sendHashes() {
    int length = HASH_OUTPUT_LENGTH * CONST_k * CONST_k;
//    int length = 32;
//    printf("%p\n", (void*)h00);
//    printN(h00, length);
    getChannel()->write(h00, length);
    getChannel()->write(h01, length);
    getChannel()->write(h10, length);
    getChannel()->write(h11, length);
//    cout << "SEND FINISHED" << endl;


//    printN(h00, length);
//    printf("%p\n", (void*)h00);
//    getChannel()->write(h01, length);
    /*getChannel()->write(h10, length);
    getChannel()->write(h11, length);*/

};

template<class T, int pwr>
void SenderOT<T,pwr>::runInitialize() {
    cout << "---- Running base OT ----" << endl;
    run_baseOT(keys0_bOT, keys1_bOT, CONST_k, CONST_k);
}

template<class T, int pwr>
void SenderOT<T,pwr>::runCreateCorrelation() {
    cout << "---- Sampling correlation ----" << endl;
    sampleCorrelation();
    cout << "---- Applying PRF ----" << endl;
    applyPRF();
    cout << "---- Sending u_i's ----" << endl;
    sendUi();

    /*cout << "\n--------SENDER s0 = 0--------" << endl;
    vZ2k<T,pwr>::printVector(t0[0]);
    cout << "\n--------SENDER s0 = 1--------" << endl;
    vZ2k<T,pwr>::printVector(t0[0] + correlation);*/
}

template<class T, int pwr>
void SenderOT<T,pwr>::runConsistencyCheck() {
    cout << "---- Generating hashes ----" << endl;
    generateHashes();
    cout << "---- Sending hashes ----" << endl;
    sendHashes();
}