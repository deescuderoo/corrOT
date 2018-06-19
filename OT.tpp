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
void SenderOT<T,pwr>::runInitialize() {
    run_baseOT(keys0_bOT, keys1_bOT, CONST_k, CONST_k);
}

template<class T, int pwr>
void SenderOT<T,pwr>::sampleCorrelation(int length) {
    int size = (pwr * length)/8;
    vector<byte> buffer(size);
    memcpy(&buffer[0], prg->getPRGBytesEX(size), size);
    this->correlation = vZ2k<T,pwr>(vectorConversion(buffer,length,pwr/8)); // Double copy
}

//template<class T, int pwr>
//void SenderOT<T,pwr>::applyPRF() {
//    // Call runPRF
//    runPRF(keys0_bOT,keys1_bOT);
//};