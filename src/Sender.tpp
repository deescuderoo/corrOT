template<class T, int pwr>
/**
 *
 * @tparam T data type
 * @tparam pwr
 * @param data0 to store the input-0s of random nOTs
 * @param data1 to store the input-1s of random nOTs
 * @param nOT Number of OTs
 * @param elementSizeBits Element Size(in bits)
 */
void SenderOT<T,pwr>::run_baseOT(vector<vector<byte>> data0, vector<vector<byte>> data1, int nOT, int elementSizeBits) {

    OTExtensionBristolSender sender(12001, true, this->getChannel());
    OTBatchSInput * input = new OTExtensionRandomizedSInput(nOT, elementSizeBits);

    auto output = sender.transfer(input);

    /**
     * outputbytes0 returns the a long byte array of the all the Input-0s of all the Random nOTs
     * outputbytes1 returns the a long byte array of the all the Input-1s of all the Random nOTs
     */
    auto output_ = ((OTExtensionRandomizedSOutput *) output.get());
    vector<byte> outputbytes0 = output_->getR0Arr();
    vector<byte> outputbytes1 = output_->getR1Arr();

    /**
     * keys0_bOT converts the long byte array to vector of vector of bytes where i'th row is the input-0 of i'th random OT.
     * keys1_bOT converts the long byte array to vector of vector of bytes where i'th row is the input-1 of i'th random OT.
     */
    keys0_bOT = vectorConversion(outputbytes0, CONST_k, SIZE_OT);
    keys1_bOT = vectorConversion(outputbytes1, CONST_k, SIZE_OT);
}

template<class T, int pwr>
void SenderOT<T,pwr>::sampleCorrelation() {
    int length = CONST_n + CONST_k_;
    int size = (pwr * length)/8;

    vector<byte> buffer(size);

    /**
     * @param buffer.data returns starting address of the vector
     * @param size number bytes needed from PRG.
     * getBytes(,) generates random string and  stores it in the buffer array.
     */
    prg->getBytes(buffer.data(), size);

    /**
     * Converts the buffer (via vector of vector of bytes) into a vector of Group z2k elements.
     */
    this->correlation = vZ2k<T,pwr>(vectorConversion(buffer,length,pwr/8));
}

template<class T, int pwr>

void SenderOT<T,pwr>::applyPRF() {

    int sizeBytes = (CONST_pwr * (CONST_n + CONST_k_))/8;

    vector<byte> tmp0(sizeBytes);
    vector<byte> tmp1(sizeBytes);

    for (int i = 0; i < keys0_bOT.size(); i++) {

        /**
         * used to generate long array of PRF output (in bytes) with keys0_bOT and keys1_bOT as keys respectively.
         * Stores in tmp0 and tmp1.
         */
        prf->init(keys0_bOT[i].data(), sizeBytes);
        prf->getBytes(tmp0.data(), sizeBytes);
        prf->init(keys1_bOT[i].data(), sizeBytes);
        prf->getBytes(tmp1.data(), sizeBytes);

        /**
         * t0[i] and t1[i] stores the PRF outputs in the form of vector of Group elements.
         */

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

        /**
         * used to calculate ui = ti_0 + ti_1 + correlation;
         */
        t0[i].add(t1[i], tmpVector);
        tmpVector.add(correlation, ui);
        byte * ptr = ui.getBytePtr();

        /**
         * writing on the channel.
         */
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

            /**
             * Used to subtract tmp Vector = t0[alpha] - t1[beta]
             */
            t0[alpha].subtract(t0[beta], tmpVector00);
            t0[alpha].subtract(t1[beta], tmpVector01);
            t1[alpha].subtract(t0[beta], tmpVector10);
            t1[alpha].subtract(t1[beta], tmpVector11);

            /**
             * Used to generate hashes of the subtraction.
             */
            Hash_value(hash, tmpVector00.getBytePtr(), h00 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), size);
            Hash_value(hash, tmpVector01.getBytePtr(), h01 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), size);
            Hash_value(hash, tmpVector10.getBytePtr(), h10 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), size);
            Hash_value(hash, tmpVector11.getBytePtr(), h11 + HASH_OUTPUT_LENGTH*(beta + alpha*CONST_k), size);

        }
    }
}

template<class T, int pwr>
void SenderOT<T,pwr>::sendHashes() {
    int length = HASH_OUTPUT_LENGTH * CONST_k * CONST_k;

    /**
     * sends all the generated hashes
     */
    getChannel()->write(h00, length);
    getChannel()->write(h01, length);
    getChannel()->write(h10, length);
    getChannel()->write(h11, length);

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

}

template<class T, int pwr>
void SenderOT<T,pwr>::runConsistencyCheck() {
    cout << "---- Generating hashes ----" << endl;

    generateHashes();

    cout << "---- Sending hashes ----" << endl;

    sendHashes();
}