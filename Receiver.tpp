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
        if (choice_bits[i] == 1){
            // bit = 1
            ai[i] = ui[i].subtract(t[i]);
        } else {
            ai[i] = t[i];
        }
    }
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::receiveHashes() {
    int length = HASH_OUTPUT_LENGTH * CONST_k * CONST_k;
    getChannel()->read(h00, length);
    getChannel()->read(h01, length);
    getChannel()->read(h10, length);
    getChannel()->read(h11, length);
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::checkHashes() {
    int size = t[0].getLength() * sizeof(Z2k<T,pwr>);;
    byte tmpBuffer0[HASH_OUTPUT_LENGTH];
    byte tmpBuffer1[HASH_OUTPUT_LENGTH];

    vZ2k<T,pwr> tmpVector0(CONST_n + CONST_k_);
    vZ2k<T,pwr> tmpVector1(CONST_n + CONST_k_);

//    cout << "choicebits " << (int)choice_bits[0] << (int)choice_bits[1] << endl;
//    vZ2k<T,pwr>::printVector(t[0] - t[1]);

    for (int alpha = 0; alpha < CONST_k; alpha++){
        for (int beta = 0; beta < CONST_k; beta++){
            tmpVector0 = t[alpha].subtract(t[beta]);
            tmpVector1 = ui[alpha].subtract(ui[beta]).subtract(tmpVector0);
            Hash_value(hash, tmpVector0.getBytePtr(), tmpBuffer0, size);
            Hash_value(hash, tmpVector1.getBytePtr(), tmpBuffer1, size);

            if (memcmp(ui[alpha].getBytePtr(), ui[beta].getBytePtr(), size) == 0 && alpha != beta){
                cout << "I COMPLAIN!!" << endl;
                assert(false);
            }

            if (choice_bits[alpha] == 0 && choice_bits[beta] == 0){
                if (memcmp(tmpBuffer0, h00 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(tmpBuffer1, h11 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0){
                    cout << "I COMPLAIN!!" << endl;
                    assert(false);
                }
            } else
            if (choice_bits[alpha] == 0 && choice_bits[beta] == 1){

                if (memcmp(tmpBuffer0, h01 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(tmpBuffer1, h10 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(ui[alpha].getBytePtr(), ui[beta].getBytePtr(), size) == 0){
                    cout << "I COMPLAIN!!" << endl;
                    assert(false);
                }
            } else
            if (choice_bits[alpha] == 1 && choice_bits[beta] == 0){
                if (memcmp(tmpBuffer0, h10 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(tmpBuffer1, h01 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(ui[alpha].getBytePtr(), ui[beta].getBytePtr(), size) == 0){
                    cout << "I COMPLAIN!!" << endl;
                    assert(false);
                }
            } else
            if (choice_bits[alpha] == 1 && choice_bits[beta] == 1){
                if (memcmp(tmpBuffer0, h11 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0||
                    memcmp(tmpBuffer1, h00 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0){
                    cout << "I COMPLAIN!!" << endl;
                    assert(false);
                 }
            }

//            printN(h00 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), 32);
//            printf("%p\n", (void*)h00);
        }
    }
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::runInitialize() {
    cout << "---- Running base OT ----" << endl;
    run_baseOT(choice_bits, CONST_k, CONST_k);
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::runCreateCorrelation() {
    cout << "---- Applying PRF OT ----" << endl;
    applyPRF();
    cout << "---- Receiving u_i's ----" << endl;
    receiveUi();
    cout << "---- Computing a_i's ----" << endl;
    computeAi();

    /*cout << "\n--------RECEIVER s0 = " << (int)choice_bits[0] << " --------" << endl;
    vZ2k<T,pwr>::printVector(ai[0]);*/
}

template<class T, int pwr>
void ReceiverOT<T,pwr>::runConsistencyCheck() {
    cout << "---- Receiving hashes ----" << endl;
    receiveHashes();
    cout << "---- Checking hashes ----" << endl;
    checkHashes();
}