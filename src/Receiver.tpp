template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Populates the choice_bits buffer with random bits
 */
void ReceiverOT<T,pwr>::generateChoiceBitsOT() {
    prg->getBytes(&choice_bits[0], CONST_k);

    for (unsigned char &choice_bit : choice_bits) {
        choice_bit %= 2; // Map byte to bit
    }
}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * @param sigma to store the output of random nOTs
 * @param nOT Number of OTs
 * @param elementSizeBits Element Size (in bits)
 * Runs nOT base OTs with random strings
 */
void ReceiverOT<T,pwr>::run_baseOT(vector<byte> sigma, int nOT, int elementSizeBits) {

    OTExtensionBristolReceiver receiver("localhost", 12001, true, getChannel());
    sigma.resize(nOT);
    OTBatchRInput * input = new OTExtensionRandomizedRInput(sigma, elementSizeBits);
    auto output = receiver.transfer(input);
    vector<byte> outputbytes = ((OTOnByteArrayROutput *)output.get())->getXSigma();

    keys_bOT = vectorConversion(outputbytes, CONST_k, SIZE_OT);

}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Applies the PRF with the keys from the OT
 */
void ReceiverOT<T,pwr>::applyPRF() {
    for (int i = 0; i < CONST_k; i++) {
        int sizeBytes = (CONST_pwr * (CONST_n + CONST_k_))/8;
        vector<byte> tmp(sizeBytes);

        prf->init(keys_bOT[i].data(), sizeBytes);
        prf->getBytes(tmp.data(), sizeBytes);
        t[i] = vZ2k<T,pwr>(vectorConversion(tmp, CONST_n + CONST_k_, pwr/8));
    }
}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Receives the u_i values from the sender and stores them in ui
 */
void ReceiverOT<T,pwr>::receiveUi() {
    int length = sizeof(Z2k<T,pwr>) * ui[0].getLength();
    vector<byte> tmp(length);
    for (int i = 0; i < CONST_k; i++){
        getChannel()->read(tmp.data(), length);
        ui[i] = vZ2k<T,pwr>(vectorConversion(tmp, CONST_n + CONST_k_, pwr/8));
    }
}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Computes the values a_i in order to send them to the sender
 */
void ReceiverOT<T,pwr>::computeAi() {
    for (int i = 0; i < CONST_k; i++) {
        if (choice_bits[i] == 1){
            ui[i].subtract(t[i], ai[i]);
        } else {
            ai[i] = t[i];
        }
    }
}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Receives the hashes calculated by the sender and stores them in h00, h01, h10 and h11
 */
void ReceiverOT<T,pwr>::receiveHashes() {
    int length = HASH_OUTPUT_LENGTH * CONST_k * CONST_k;
    getChannel()->read(h00, length);
    getChannel()->read(h01, length);
    getChannel()->read(h10, length);
    getChannel()->read(h11, length);
}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Calculates hashes and compare with the ones received
 */
void ReceiverOT<T,pwr>::checkHashes() {
    int size = t[0].getLength() * sizeof(Z2k<T,pwr>);;
    byte tmpBuffer0[HASH_OUTPUT_LENGTH];
    byte tmpBuffer1[HASH_OUTPUT_LENGTH];

    vZ2k<T,pwr> tmpVector0(CONST_n + CONST_k_);
    vZ2k<T,pwr> tmpVector1(CONST_n + CONST_k_);
    vZ2k<T,pwr> tmpVector2(CONST_n + CONST_k_);

    for (int alpha = 0; alpha < CONST_k; alpha++){
        for (int beta = 0; beta < CONST_k; beta++){
            t[alpha].subtract(t[beta], tmpVector0);
            ui[alpha].subtract(ui[beta], tmpVector2);
            tmpVector2.subtract(tmpVector0, tmpVector1);
            Hash_value(hash, tmpVector0.getBytePtr(), tmpBuffer0, size);
            Hash_value(hash, tmpVector1.getBytePtr(), tmpBuffer1, size);

            if (memcmp(ui[alpha].getBytePtr(), ui[beta].getBytePtr(), size) == 0 && alpha != beta){
                cout << "Wrong hash for alpha=" << alpha << " and beta=" << beta << endl;
                assert(false);
            }

            if (choice_bits[alpha] == 0 && choice_bits[beta] == 0){
                if (memcmp(tmpBuffer0, h00 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(tmpBuffer1, h11 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0){
                    cout << "Wrong hash for alpha=" << alpha << " and beta=" << beta << endl;
                    assert(false);
                }
            } else
            if (choice_bits[alpha] == 0 && choice_bits[beta] == 1){

                if (memcmp(tmpBuffer0, h01 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(tmpBuffer1, h10 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(ui[alpha].getBytePtr(), ui[beta].getBytePtr(), size) == 0){
                    cout << "Wrong hash for alpha=" << alpha << " and beta=" << beta << endl;
                    assert(false);
                }
            } else
            if (choice_bits[alpha] == 1 && choice_bits[beta] == 0){
                if (memcmp(tmpBuffer0, h10 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(tmpBuffer1, h01 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0 ||
                    memcmp(ui[alpha].getBytePtr(), ui[beta].getBytePtr(), size) == 0){
                    cout << "Wrong hash for alpha=" << alpha << " and beta=" << beta << endl;
                    assert(false);
                }
            } else
            if (choice_bits[alpha] == 1 && choice_bits[beta] == 1){
                if (memcmp(tmpBuffer0, h11 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0||
                    memcmp(tmpBuffer1, h00 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), HASH_OUTPUT_LENGTH) != 0){
                    cout << "Wrong hash for alpha=" << alpha << " and beta=" << beta << endl;
                    assert(false);
                 }
            }
        }
    }
}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Runs the first phase of the protocol, where the base OT is executed
 */
void ReceiverOT<T,pwr>::runInitialize() {
    cout << "---- Running base OT ----" << endl;
    generateChoiceBitsOT();
    run_baseOT(choice_bits, CONST_k, CONST_k);
}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Runs the second phase of the protocol in which the correlation is established
 */
void ReceiverOT<T,pwr>::runCreateCorrelation() {
    cout << "---- Applying PRF OT ----" << endl;
    applyPRF();
    cout << "---- Receiving u_i's ----" << endl;
    receiveUi();
    cout << "---- Computing a_i's ----" << endl;
    computeAi();
}

template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * Runs the third and last phase of the protocol in which it is checked if the correlation was created correctly
 */
void ReceiverOT<T,pwr>::runConsistencyCheck() {
    cout << "---- Receiving hashes ----" << endl;
    receiveHashes();
    cout << "---- Checking hashes ----" << endl;
    checkHashes();
}