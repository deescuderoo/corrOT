template<class T, int pwr>
void ReceiverOT<T,pwr>::generateChoiceBitsOT() {
    size_t size = choice_bits.size();
    memcpy(&choice_bits[0], prg->getPRGBytesEX(size), size);
    for (unsigned char &choice_bit : choice_bits) {
        choice_bit %= 2; // Project byte to bit
//        cout << (int)choice_bits[i] << endl;
    }
//    cout << "Choice bits" << endl;
//    printN(choice_bits);
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::run_baseOT(vector<byte> sigma, int nOT, int elementSizeBits) {

    OTExtensionBristolReceiver receiver("localhost", 12001, true, getChannel());
    sigma.resize(nOT);
    OTBatchRInput * input = new OTExtensionRandomizedRInput(sigma, elementSizeBits);
    auto output = receiver.transfer(input);
    vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();

//    cout << "Output strings" << endl;
//    printN(outputbytes);

    keys_bOT = vectorConversion(outputbytes, CONST_k, SIZE_OT);
//    for (auto row : keys_bOT){
//        printN(row);
//    }

}

template<class T, int pwr>
void ReceiverOT<T,pwr>::applyPRF() {
    for (int i = 0; i < CONST_k; i++) {
        int sizeBytes = (CONST_pwr * (CONST_n + CONST_k_))/8;
        vector<byte> tmp(sizeBytes);
        prfCall(prf, keys_bOT[i], tmp, sizeBytes);
        t[i] = vZ2k<T,pwr>(vectorConversion(tmp, CONST_n + CONST_k_, pwr/8));
    }
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::receiveUi() {
    int length = sizeof(Z2k<T,pwr>) * ui[0].getLength();
    vector<byte> tmp(length);
    for (int i = 0; i < CONST_k; i++){
        getChannel()->read(tmp.data(), length);
//        printN(tmp);
        ui[i] = vZ2k<T,pwr>(vectorConversion(tmp, CONST_n + CONST_k_, pwr/8));
    }
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::computeAi() {
    for (int i = 0; i < CONST_k; i++) {
        if (choice_bits[i]){
            // bit = 1
            ai[i] = ui[i] - t[i];
        } else {
            ai[i] = t[i];
        }
    }
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::runInitialize() {
    run_baseOT(choice_bits, CONST_k, CONST_k);
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::runCreateCorrelation() {
    applyPRF();
    receiveUi();
    computeAi();

    /*cout << "\n--------RECEIVER s0 = " << (int)choice_bits[0] << " --------" << endl;
    vZ2k<T,pwr>::printVector(ai[0]);*/
}