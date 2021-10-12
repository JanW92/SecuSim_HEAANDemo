/*
 * BasicSetup.hpp
 *
 *  Created on: 14 Jul 2021
 *      Author: barbarossa
 */

#ifndef SRC_MATRIXVECTORMULTIPLICATION_HPP_
#define SRC_MATRIXVECTORMULTIPLICATION_HPP_
#include "../src/HEAAN.h"
#include <math.h>
#include <exception>
#include <vector>
#include <algorithm>

double _expectedValue = 2.01234567890123456789;
// Gets a diagonal from a matrix U


inline void SaveInFile(string completePath, string content)
{
    std::ofstream myfile;
    myfile.open(completePath, std::ios::app);
    myfile << content;
    myfile.close();
}


inline std::vector<complex<double>> get_diagonal(int position, std::vector<std::vector<complex<double>>> &U)
{

    std::vector<complex<double>> diagonal(U.size());

    int k = 0;
    // U(0,l) , U(1,l+1), ... ,  U(n-l-1, n-1)
    for (int i = 0, j = position; (i < U.size() - position) && (j < U.size()); i++, j++)
    {
        diagonal[k] = U[i][j];
        k++;
    }
    for (int i = U.size() - position, j = 0; (i < U.size()) && (j < position); i++, j++)
    {
        diagonal[k] = U[i][j];
        k++;
    }

    return diagonal;
}

/*
 * Generates a diagonal matrix with 1.1 as value on the main diagonal.
 */
std::vector<std::vector<complex<double>>> GenerateMatrix(int dimension)
{
	double val (1.1);
	std::vector<std::vector<complex<double>>> matrix(dimension,std::vector<complex<double>>(dimension));
	for(int i = 0; i < dimension; i++)
	{
		for(int j = 0; j < dimension; j++)
		{
			if(j == i)
			{
				matrix[i][j].real(val);
			}
			else
			{
				matrix[i][j].real(0);
				//matrix[i][j].real(val * pow(-1,j+i+1));
			}
		}
	}
	return matrix;
}

/*
 * Returns the maximum absolute error
 */
double DeterminePrecision (std::complex<double> * vector, int n, double expected)
{
	std::vector<double>errordResult(n);
	try
	{

	    for (int i = 0; i < n; i++)
	    {
	        errordResult[i] = abs(vector[i].real() - expected);
	    }
	    // Find the maximum element in error vector
	    auto maxError = std::max_element(errordResult.begin(), errordResult.end());
	    double indx = std::distance(errordResult.begin(), maxError);
	    return *maxError;
	}
	catch (std::exception e)
	{
		std::cout << e.what();
	}
}


void RunMatrixMultiplication()
{
    string fileName = "SecuSim_HEEANMeasurement.txt";
    string path = "/home/barbarossa/Desktop/";

	//Number of sequentially performed operations
	const int circuitDepth = 12;

	//HEAAN key objects
	Ring ring;
	SecretKey sk(ring);
	Scheme scheme(sk, ring);

	//HEAAN parameter. Should make 12 - 13 operations possible
	long logq = 800; ///< Ciphertext Modulus. Should be smaller (or equal?) than logQ in "src/Params.h" : 800

	long logp = 60; ///Scaling factor.
	//Due to https://github.com/snucrypto/HEAAN/issues/22
	//r = logT + logI
	long logr = 2; //Amount of rotations; 0 = 1 rotation
	long logT = 0;//?
	long logI = 4;//?
	//Should be smaller than logN in "src/Params.h" : 16 => Max Slots = 65536
	long logn = 7; // -> 128 slots
	//long logn = 14; // -> 16384 slots
	long n = 1 << logn;
	long r = (1 << logr);

	string content = "Start of sequential matrix vector multiplications.\nParameter:\nlogq: "+ std::to_string(logq) +"\n"+
			"logp: "+std::to_string(logp)+"\n"+
			"logr: "+std::to_string(logr)+"\n"+
			"logT: "+std::to_string(logT)+"\n"+
			"logI: "+std::to_string(logI)+"\n"+
			"logn: " +std::to_string(logn)+"\n";
	SaveInFile(path+fileName, content);

	//Rotation key for rotation by 1
	scheme.addLeftRotKey(sk, r);
	//scheme.addBootKey(sk,logn,logq + logI);

	//Dummy matrix and vector. The matrix has to be stored as multiple vectors, containing the diagonals of the original matrix
	std::vector<std::vector<complex<double>>> plain_diagonals(n, std::vector<complex<double>>(n));
	std::vector<std::vector<complex<double>>> plain_matrix = GenerateMatrix(n);
	complex<double> * fieldVector = new complex<double>[n];
	complex<double> * solution = new complex<double>[n];
	complex<double> * zeroVector = new complex<double>[n];

	//Create the vectors containing the diagonals of the original matrix.
	for (int i = 0; i < n; i++)
	{
		plain_diagonals[i] = get_diagonal(i, plain_matrix);
	}
	plain_matrix.clear();
	plain_matrix.shrink_to_fit();

	//Initializing the field vector and solution vector as additive identity
	for (int i =0; i< n; i++)
	{
		fieldVector[i] = real(_expectedValue);
		zeroVector[i] = real(0.);
	}

	//Encryption
	std::vector<Ciphertext> c_matrix(n);
	complex<double> * temp = nullptr;
	Ciphertext c_fieldVec, c_solution, c_zeroVector, c_Temp;

	auto before_Encryption = std::chrono::high_resolution_clock::now();
	scheme.encrypt(c_fieldVec,fieldVector,n,logp,logq);
	scheme.encrypt(c_zeroVector, zeroVector, n, logp, logq);
	/*
	 * c_solution is going to be added with the result of the multiplication. The multiplication reduces the modulus which has to be equal for an operation between the two variables.
	 * Thus c_solutions mod has to be reduced
	 */


	try
	{
	scheme.modDownByAndEqual(c_zeroVector, logp);
	c_solution.copy(c_zeroVector);

		for (int i = 0; i < n; i++)
		{
			//Aus irgeneinem Grund kann das Vektorelement hier nicht direkt verwendet werden. Daher der umweg über locale variable testC.
			temp = &plain_diagonals[i][0];
			scheme.encrypt(c_Temp, temp, n, logp, logq);
			//scheme.encrypt(c_matrix[i], temp, n, logp, logq);
			c_matrix[i].copy(c_Temp);
//			temp =scheme.decrypt(sk, c_Temp);
//			temp =scheme.decrypt(sk, c_matrix[i]);
			c_Temp.free();
		}
		auto after_Encryption = std::chrono::high_resolution_clock::now();
		double elapsedTimings=0;
		double precision=0;
		double precisionAfterBootstrapping=0;
		double elapsedTimeBootstrapping =0;

		//Matrix-Vector multiplication
 		for (int j = 0; j < circuitDepth; j ++)
		{
			std::cout << "Multiplication " << j +1 << std::endl;
			auto before_Multiplication = std::chrono::high_resolution_clock::now();
			for(int i = 0; i< n; i++)
			{
//				temp =scheme.decrypt(sk, c_matrix[i]);
//				temp =scheme.decrypt(sk, c_fieldVec);
				//Perform matrix-vector multiplication
				scheme.mult(c_Temp,c_matrix[i],c_fieldVec);
//				temp =scheme.decrypt(sk, c_Temp);

				//new cM: logq = logq², logp = logp - logq -> rescale, so that it can be added with c_solution
				scheme.reScaleByAndEqual(c_Temp, logp);
//				temp =scheme.decrypt(sk, c_Temp);

				scheme.addAndEqual(c_solution, c_Temp);
//				temp =scheme.decrypt(sk, c_Temp);
				c_Temp.free();

				scheme.leftRotateFastAndEqual(c_fieldVec, r);
				//At the end of the process c_fieldVec will get the value from c_result, which has the same logp and logq as cM now.
				//For the next multiplication process they have to match.
				scheme.modDownByAndEqual(c_matrix[i],logp);
			}
			auto after_Multiplication = std::chrono::high_resolution_clock::now();
			elapsedTimings = (std::chrono::duration_cast<std::chrono::duration<double>>(after_Multiplication- before_Multiplication)).count();
			temp =scheme.decrypt(sk, c_solution);
			precision = DeterminePrecision(temp,n,_expectedValue * pow(1.1,j+1));

			content = "Multiplication " + std::to_string(j) + "\nLargest absolute error: " + std::to_string(precision) + "\nElapsed Time: " +std::to_string(elapsedTimings) + "\n";
			SaveInFile(path + fileName, content);

			if(c_zeroVector.logq >= logp)
			{
				//Setting the field vector to the result of this iteration step
				c_fieldVec.free();
				c_fieldVec.copy(c_solution);
				c_solution.free();
				//c_solution must be prepared to take the solution of another multiplication in (reduce modulus)
				scheme.modDownByAndEqual(c_zeroVector,logp);
				c_solution.copy(c_zeroVector);
			}
			else
			{
				auto before_Bootstrapping = std::chrono::high_resolution_clock::now();
				//Setting the field vector to the result of this iteration step
				c_fieldVec.free();
				c_fieldVec.copy(c_solution);
				c_solution.free();

				//Bootstrapping fieldvector
				std::cout << "Fieldvector logq: " << c_fieldVec.logq << std::endl;
				scheme.bootstrapAndEqual(c_fieldVec, logq, logQ,logT);
				std::cout << "Fieldvector logq after bootstrapping: " <<  c_fieldVec.logq << std::endl;

				//Check the precision that remains after bootstrapping
				temp =scheme.decrypt(sk, c_fieldVec);
				precisionAfterBootstrapping = DeterminePrecision(temp,n,_expectedValue * pow(1.1,j+1));

				//Before c_solution can be set back to zero vector it has to be bootstrapped as well
				scheme.bootstrapAndEqual(c_zeroVector, logq, logQ,logT);
				scheme.modDownByAndEqual(c_zeroVector,logp);
				c_solution.copy(c_zeroVector);
				std::cout << "Solution logq after bootstrapping: " <<  c_solution.logq << std::endl;

				for(int i = 0; i < n; i++)
				{
					scheme.bootstrapAndEqual(c_matrix[i], logq, logQ,logT);
				}
				std::cout << "Matrix logq after bootstrapping: " <<  c_matrix[0].logq << std::endl;
				auto after_Bootstrapping = std::chrono::high_resolution_clock::now();
				elapsedTimeBootstrapping = (std::chrono::duration_cast<std::chrono::duration<double>>(after_Bootstrapping- before_Bootstrapping)).count();
				content = "Bootstrapping\nLargest absolute error: " + std::to_string(precisionAfterBootstrapping) + "\nElapsed Time: " +std::to_string(elapsedTimeBootstrapping) + "\n";
				SaveInFile(path + fileName, content);
			}

		}
	}
	catch (std::exception e)
	{
		std::cout << e.what();
	}

}


#endif /* SRC_MATRIXVECTORMULTIPLICATION_HPP_ */
