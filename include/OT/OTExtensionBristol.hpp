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

//#include "../cryptoInfra/SecurityLevel.hpp"
#include <OT/OTExtensionWithMatrix.h>
#include "OTBatch.hpp"
#include <Communication/Comm.hpp>
#include <memory>
#include <openssl/evp.h>

using namespace std;


/**
*
* The base OT class holds pointers to the ot extension of the bristol library implementation based on the paper :
* Actively Secure OT Extension with Optimal Overhead.
*
* There are three versions of OT extension: General, Correlated and Random. The bristol library currently support the randomized version only
*
* The inputs are different for each version for the sender side. The receiver inputs the same for all versions and are called .
* In general OT extension both x0 and x1 are given by the sender.
* In Correlated OT extension the user gives a delta array and x0, x1 arrays are chosen such that x0 = delta^x1.
* In random OT extension both x0 and x1 are chosen randomly.
* To allow the user decide which OT extension's version he wants, each option has a corresponding input class.
* The particular OT extension version is executed according to the given input instance;
* For example, if the user gave as input an instance of OTExtensionRandomSInput than the random OT Extension will be execute.
*
* NOTE: The size of element is fixed 128 bit.
*
* @author Cryptography and Computer Security Research Group Department of Computer Science Bar-Ilan University (Meital Levy)
*
*/
class OTExtensionBristolBase {

protected:

	unique_ptr<OTExtensionWithMatrix> pOtExt;   //Hold a pointer to the ot extension of the library.
	unique_ptr<TwoPartyPlayer> pParty;          //We hold a pointer to this object since it has to be alive when running the ot.
	shared_ptr<CommParty> channel;              //this is a shared pointer for the general case where there is one communication between the
								                //sender and the receiver after the bristol ot extension is done in providing random x0 and x1 for the sender.


	/**
	* The actual transfer that calls the ot extension Bristol library transfer. This is called by derived classes.
	* @param nOTs the number of ot's to transfer
	* @param receiverInput the receiver input. This passed by both sender and receiver since the underlying bristol library demands that even though
	* 		 the sender should not have this information. The derived sender class sends a dummy bitvector as input.
	*/
	void transfer(int nOTs, const BitVector& receiverInput);

	/**
	 * Initializes the underlying OTExtensionWithMatrix object, the communication and runs the base ot.
	 */
	void init(const string& senderAddress, int port, int my_num, bool isSemiHonest, const shared_ptr<CommParty> & channel);

    /**
     * Shrinks the given array to the given size.
     * For example, if the srcArr is an array of x*sourceElementSize bytes, the targetArr will be an array of
     * iterations*targetElementSize bytes.
     * @param sourceElementSize size of each element in the srcArr.
     * @param targetElementSize size of each element in the targetArr.
     * @param iterations required size of elements in targetArr
     * @param srcArr source  array to shrink
     * @param targetArr output array - the shrinked array.
     */
	void shrinkArray(int sourceElementSize, int targetElementSize, int iterations, byte* srcArr, byte* targetArr);

    /**
     * Creates the array of indices to be the plaintext for the AES.
     * @param factor the number of blocks that should be filled
     * @return the created array
     */
    byte* createCountersArray(int factor) const;

    /**
     * Expands the output to the required size using AES.
     * @param elementSize size of each element in the OT, in bits.
     * @param x0 the array where the expanded output should be located
     * @param factor the number of blocks that should be filled
     * @param counters array of indices that play as the plaintext
     * @param aes the OpenSSl's AES object.
     * @param output the temporary array that holds the AES output
     * @param i the current OT index
     */
    void expandOutput(int elementSize, byte * key, vector<byte> & output, int factor, const byte *counters,
                                               EVP_CIPHER_CTX *aes, byte * aesOutput, int i) const;
};


/**
 * A receiver side of the bristol ot extension for both the semi honest and malicious adversaries.
 * The receiver should provide the sender's address since that is the hostname it should connect to.
 *
 */
class OTExtensionBristolReceiver: public OTExtensionBristolBase,  public OTBatchReceiver{

public:
	//OTExtensionBristolReciever(const string& senderAddress, int port, bool isSemiHonest);
	OTExtensionBristolReceiver(const string& senderAddress, int port,
                               bool isSemiHonest, const shared_ptr<CommParty> & channel = nullptr);

	virtual shared_ptr<OTBatchROutput> transfer(OTBatchRInput * input) override;
};

/**
 * A sender side of the bristol ot extension for both the semi honest and malicious adversaries.
 * The sender does not need to provide an IP, since is listens on local host, It only needs to provide the
 * port on which it listens to.
 *
 */
class OTExtensionBristolSender: public OTExtensionBristolBase, public OTBatchSender{

public:
	OTExtensionBristolSender(int port, bool isSemiHonest,
                                 const shared_ptr<CommParty> &channel);

	virtual shared_ptr<OTBatchSOutput> transfer(OTBatchSInput * input) override;

};
