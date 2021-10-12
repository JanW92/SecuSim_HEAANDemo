/*
* Copyright (c) by CryptoLab inc.
* This program is licensed under a
* Creative Commons Attribution-NonCommercial 3.0 Unported License.
* You should have received a copy of the license along with this
* work.  If not, see <http://creativecommons.org/licenses/by-nc/3.0/>.
*/

#include "../src/HEAAN.h"
/**
  * This file is for test HEAAN library
  * You can find more in src/TestScheme.h
  * "./TestHEAAN Encrypt" will run Encrypt Test
  * There are Encrypt, EncryptSingle, Add, Mult, iMult, RotateFast, Conjugate Tests
  */
void TestHEAANFunctions(string function, long int DecryptForShare) {

	long logq = 800; ///< Ciphertext Modulus
	long logp = 30; ///< Real message will be quantized by multiplying 2^40
	long logn = 4; ///< log2(The number of slots)

//----------------------------------------------------------------------------------
//   STANDARD TESTS
//----------------------------------------------------------------------------------

	if(function == "Encrypt") TestScheme::testEncrypt(logq, logp, logn);
	if(function == "EncryptBySk") TestScheme::testEncryptBySk(logq, logp, logn);
	if(function == "DecryptForShare") TestScheme::testDecryptForShare(logq, logp, logn, DecryptForShare);
	if(function == "EncryptSingle") TestScheme::testEncryptSingle(logq, logp);
	if(function == "Add") TestScheme::testAdd(logq, logp, logn);
	if(function == "Mult") TestScheme::testMult(logq, logp, logn);
	if(function == "iMult") TestScheme::testiMult(logq, logp, logn);

//----------------------------------------------------------------------------------
//   ROTATE & CONJUGATE
//----------------------------------------------------------------------------------

	long r = 1; ///< The amout of rotation
	if(function == "RotateFast") TestScheme::testRotateFast(logq, logp, logn, r);
	if(function == "Conjugate") TestScheme::testConjugate(logq, logp, logn);
    
//----------------------------------------------------------------------------------
//   BOOTSTRAPPING
//----------------------------------------------------------------------------------
    
    logq = logp + 10; //< suppose the input ciphertext of bootstrapping has logq = logp + 10
    logn = 3; //< larger logn will make bootstrapping tech much slower
    long logT = 4; //< this means that we use Taylor approximation in [-1/T,1/T] with double angle fomula
    if(function == "Bootstrapping") TestScheme::testBootstrap(logq, logp, logn, logT);

}
