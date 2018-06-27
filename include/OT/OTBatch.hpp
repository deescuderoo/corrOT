/**
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 * Copyright (c) 2016 LIBSCAPI (http://crypto.biu.ac.il/SCAPI)
 * This file is part of the SCAPI project.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * We request that any publication and/or code referring to and/or based on SCAPI contain an appropriate citation to SCAPI, including a reference to
 * http://crypto.biu.ac.il/SCAPI.
 *
 * Libscapi uses several open source libraries. Please see these projects for any further licensing issues.
 * For more information , See https://github.com/cryptobiu/libscapi/blob/master/LICENSE.MD
 *
 * %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 *
 */

#pragma once
#include "OT.hpp"
#include <emmintrin.h>
#ifndef _WIN32
#include <OT/BitMatrix.h>
#include <OT/BitVector.h>
#endif
/**
 * This is a marker class for OT sender output, where there is a derived class for each OT protocol that has an output.
 * Most OT senders output nothing. However in the batch scenario there may be cases where the protocol wishes to output x0 and x1 instead of inputting it.
 * Every concrete protocol outputs different data. But all must return an implemented class of this interface or null.
 */

class OTBatchSOutput {
};

/**
 * Output for OT extension in the random mode.
 * In this mode the output are the random created X0 and X1 vectors.
 */
class OTExtensionRandomizedSOutput: public OTBatchSOutput {

protected:

	vector<byte> r0Arr;
	vector<byte> r1Arr;

public:

    OTExtensionRandomizedSOutput(){}
    OTExtensionRandomizedSOutput(const vector<byte> & r0Arr, const vector<byte> & r1Arr) : r0Arr(r0Arr), r1Arr(r1Arr) {}


    /**
	 * @return the array that holds all the x0 for all the senders serially.
	 */
	vector<byte> getR0Arr() {
		return r0Arr;
	}
	;
	/**
	 * @return the array that holds all the x1 for all the senders serially.
	 */
	vector<byte> getR1Arr() {
		return r1Arr;
	}
};

/**
* Output for OT extension in the correlated mode.
* In this mode the output are the created X0 and X1 vectors.
*/
class OTExtensionCorrelatedSOutput: public OTBatchSOutput {

protected:

	vector<byte> x0Arr;
	vector<byte> x1Arr;

public:

	OTExtensionCorrelatedSOutput(const vector<byte> &x0Arr, const vector<byte> &x1Arr) : x0Arr(x0Arr), x1Arr(x1Arr){};

	/**
	 * @return the array that holds all the x0 for all the senders serially.
	 */
	vector<byte> getx0Arr() {
		return x0Arr;
	}
	;
	/**
	 * @return the array that holds all the x1 for all the senders serially.
	 */
	vector<byte> getx1Arr() {
		return x1Arr;
	}
};

#ifndef _WIN32
class OTExtensionBristolRandomizedSOutput: public OTExtensionRandomizedSOutput {

public:

	OTExtensionBristolRandomizedSOutput(const vector<byte> & r0Arr, const vector<byte> & r1Arr) {

		this->r0Arr = move(r0Arr);
		this->r1Arr = move(r1Arr);

	};

};

#endif
/**
 * This enum class represents the options for OT extension.
 * THe options are - General, Random and Correlated.
 */
enum class OTBatchSInputTypes {
	OTExtensionGeneralSInput, OTExtensionRandomizedSInput, OTExtensionCorrelatedSInput
};

/**
 * Every Batch OT sender needs inputs during the protocol execution, but every concrete protocol needs
 * different inputs.
 * This is a marker class for OT Batch sender input, where there is a derived class for each OT protocol.
 */
class OTBatchSInput {
public:
	virtual OTBatchSInputTypes getType() = 0;
};

/**
 * A concrete class for OT extension input for the sender.
 * In the general OT extension scenario the sender gets x0 and x1 for each OT.
 */
class OTExtensionGeneralSInput: public OTBatchSInput {
private:
	vector<byte> x0Arr; // An array that holds all the x0 for all the senders serially.
	// For optimization reasons, all the x0 inputs are held in one dimensional array one after the other
	// rather than a two dimensional array.
	// The size of each element can be calculated by x0ArrSize/numOfOts.
	vector<byte> x1Arr; // An array that holds all the x1 for all the senders serially.
	int numOfOts; // Number of OTs in the OT extension.

public:
	OTBatchSInputTypes getType()override {return OTBatchSInputTypes::OTExtensionGeneralSInput;};
	/**
	 * Constructor that sets x0, x1 for each OT element and the number of OTs.
	 * @param x1Arr holds all the x0 for all the senders serially.
	 * @param x0Arr holds all the x1 for all the senders serially.
	 * @param numOfOts Number of OTs in the OT extension.
	 */
    OTExtensionGeneralSInput(const vector<byte> & x0Arr, const vector<byte> & x1Arr,
			int numOfOts) : x0Arr(x0Arr), x1Arr(x1Arr){
		this->numOfOts = numOfOts;
	}
	;
	/**
	 * @return the array that holds all the x0 for all the senders serially.
	 */
	vector<byte> & getX0Arr() {
		return x0Arr;
	}
	;
	/**
	 * @return the array that holds all the x1 for all the senders serially.
	 */
	vector<byte> & getX1Arr() {
		return x1Arr;
	}
	;
	/**
	 * @return the number of OT elements.
	 */
	int getNumOfOts() {
		return numOfOts;
	}
	;
	int getX0ArrSize() {
		return x0Arr.size();
	}
	;
	int getX1ArrSize() {
		return x1Arr.size();
	}
	;
};


/**
 * A concrete class for randomized OT extension input for the sender.
 * In the radomized OT extension scenario the sender has no x0 and x1 as input, rather this is an output from the protocol.
 */
class OTExtensionRandomizedSInput: public OTBatchSInput {
private:
	int numOfOts; // Number of OTs in the OT extension.
	int elementSize; // The size of each element in the ot extension. All elements must be of the same size.

public:
	OTBatchSInputTypes getType() override {return OTBatchSInputTypes::OTExtensionRandomizedSInput;};
	/**
	 * Constructor that sets the number of OTs.
	 * @param x1Arr holds all the x0 for all the senders serially.
	 * @param x0Arr holds all the x1 for all the senders serially.
	 * @param numOfOts Number of OTs in the OT extension.
	 */
	OTExtensionRandomizedSInput(int numOfOts, int elementSize) :
		numOfOts(numOfOts), elementSize(elementSize) {
	}

	int getNumOfOts() {
		return numOfOts;
	}


	int getElementSize() {
        return elementSize;
	}


};

/**
 * A concrete class for correlated OT extension input for the sender.
 * In the correlated OT extension scenario the sender gets the delta as input, and calculated x0 and x1 such as x0 = x1^delta.
 */
class OTExtensionCorrelatedSInput: public OTBatchSInput {
private:
	vector<byte> deltaArr; // An array that holds all the delta of xo and x1.
    // For optimization reasons, all the delta inputs are held in one dimensional array one after the other
    // rather than a two dimensional array.
    // The size of each element can be calculated by the delta size divided by the numOfOts.
	int numOfOts; // Number of OTs in the OT extension.


public:
	OTBatchSInputTypes getType() override {return OTBatchSInputTypes::OTExtensionCorrelatedSInput;};

	OTExtensionCorrelatedSInput(const vector<byte>& deltaArr, int numOfOts): deltaArr(deltaArr), numOfOts(numOfOts){}

	/**
	 * @return the array that holds all the x0 for all the senders serially.
	 */
	vector<byte> getDeltaArr() { return deltaArr; }

	/**
	 * @return the number of OT elements.
	 */
	int getNumOfOts() {	return numOfOts;}

	int getDeltaArrSize() {
		return deltaArr.size();
	}

};

/**
 * General class for Batch OT Sender.
 * Every class that derives it is signed as Batch Oblivious Transfer sender.
 */
class OTBatchSender {

public:
	/**
	 * The transfer stage of OT Batch protocol which does multiple ots in parallel.
	 */
	virtual shared_ptr<OTBatchSOutput> transfer(OTBatchSInput * input) = 0;
	virtual ~OTBatchSender(){};
};


/**
* This enum class represents the options for OT extension receiver.
* THe options are - General, Random and Correlated.
*/
enum class OTBatchRInputTypes {
	OTExtensionGeneralRInput, OTExtensionRandomizedRInput, OTExtensionCorrelatedRInput
};

/**
 * Every Batch OT receiver needs inputs during the protocol execution, but every concrete protocol needs
 * different inputs.
 * This is a marker class for OT receiver input, where there is a derived class for each OT protocol.
 */
class OTBatchRInput {
public:
	virtual OTBatchRInputTypes getType() = 0;
};

/**
 * An abstract OT receiver input.
 * All the concrete classes are the same and differ only in the name.
 * The reason a class is created for each version is due to the fact that a respective class is created for the sender and we wish to be consistent.
 * The name of the class determines the version of the OT extension we wish to run.
 * In all OT extension scenarios the receiver gets i bits. Each byte holds a bit for each OT in the OT extension protocol.
 */
class OTExtensionRInput: public OTBatchRInput {
public:
	/**
	 * Constructor that sets the sigma array and the number of OT elements.
	 * @param sigmaArr An array of sigma for each OT.
	 * @param elementSize The size of each element in the OT extension, in bits.
	 */
	OTExtensionRInput(const vector<byte> & sigmaArr, int elementSize) {
		this->sigmaArr = sigmaArr;
		this->elementSize = elementSize;
	}
	;
	vector<byte> getSigmaArr() {
		return sigmaArr;
	}
	;
	int getSigmaArrSize() {
		return sigmaArr.size();
	}
	;
	int getElementSize() {
		return elementSize;
	}
	;

private:
	vector<byte> sigmaArr; // Each byte holds a sigma bit for each OT in the OT extension protocol.
	int elementSize; // The size of each element in the ot extension. All elements must be of the same size.
};

/**
 * A concrete class for OT extension input for the receiver.
 * All the classes are the same and differ only in the name.
 * The reason a class is created for each version is due to the fact that a respective class is created for the sender and we wish to be consistent.
 * The name of the class determines the version of the OT extension we wish to run and in this case the general case.
 */
class OTExtensionGeneralRInput: public OTExtensionRInput {
public:
	/**
	 * Constructor that sets the sigma array and the number of OT elements.
	 * @param sigmaArr An array of sigma for each OT.
	 * @param elementSize The size of each element in the OT extension, in bits.
	 */
	OTExtensionGeneralRInput(const vector<byte> & sigmaArr, int elementSize) :
		OTExtensionRInput(sigmaArr, elementSize) {
	}

	OTBatchRInputTypes getType() {
		return OTBatchRInputTypes::OTExtensionGeneralRInput;
	}

};


/**
 * A concrete class for correlated OT extension input for the receiver.
 * All the classes are the same and differ only in the name.
 * The reason a class is created for each version is due to the fact that a respective class is created for the sender and we wish to be consistent.
 * The name of the class determines the version of the OT extension we wish to run and in this case the general case.
 */
class OTExtensionCorrelatedRInput: public OTExtensionRInput {
public:
	/**
	 * Constructor that sets the sigma array and the number of OT elements.
	 * @param sigmaArr An array of sigma for each OT.
	 * @param elementSize The size of each element in the OT extension, in bits.
	 */
	OTExtensionCorrelatedRInput(const vector<byte> & sigmaArr, int elementSize) :
		OTExtensionRInput(sigmaArr, elementSize) {
	}

	OTBatchRInputTypes getType() {
		return OTBatchRInputTypes::OTExtensionCorrelatedRInput;
	}

};

/**
* A concrete class for randomized OT extension input for the receiver.
* All the classes are the same and differ only in the name.
* The reason a class is created for each version is due to the fact that a respective class is created for the sender and we wish to be consistent.
* The name of the class determines the version of the OT extension we wish to run and in this case the general case.
*/
class OTExtensionRandomizedRInput: public OTExtensionRInput{
public:
	OTExtensionRandomizedRInput(const vector<byte> & sigmaArr, int elementSize) : OTExtensionRInput(sigmaArr,elementSize) {}
	OTBatchRInputTypes getType() override {	return OTBatchRInputTypes::OTExtensionRandomizedRInput;}
};

/**
 * General class for Batch OT Receiver.
 * Every class that derives it is signed as Batch Oblivious Transfer receiver.
 */
class OTBatchReceiver {
	/**
	 * The transfer stage of OT Batch protocol which does multiple ots in parallel.
	 * @param input The parameters given in the input must match the DlogGroup member of this class, which given in the constructor.
	 * @return OTBatchROutput, the output of the protocol.
	 */
public:
	virtual shared_ptr<OTBatchROutput> transfer(OTBatchRInput * input) = 0;
	virtual ~OTBatchReceiver() {};
};
