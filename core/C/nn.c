/*******************************************************************

IMPORTANTE:

Descargar dataset MNIST (formato CSV) y poner en la misma carpeta.

TRAIN: http://www.pjreddie.com/media/files/mnist_train.csv
TEST:  http://www.pjreddie.com/media/files/mnist_test.csv

Cada fila representa un numero con su correspondiente imagen en pixels
numero, pixel(1,1), pixel(1,2), pixel(1,3), ... , pixel(28,28)


*******************************************************************/


#include <stdio.h>
#include <stdlib.h> // For rand() system()
#include <string.h>
#include <math.h>

////////////////////////////////////  NEURONAL NETWORK CONSTANTS

// Number of data samples
const int NumExamplesInitial = 60000;

// Size of each layer
const int NumInput  = 784;// 7
const int NumHidden = 100; // 15
const int NumOutput = 10; // 10

// How fast I want to learn
// If I make more iterations, I should reduce the learning rate.
const int trainingIterations = 10; // More is better, but it takes longer
const double learnigRate = 0.5; // Range in [0,1]  (0.5 and 1 it's OK)

// Weights initialization
const double minWeight = -0.99;
const double maxWeight = 0.99;


////////////////////////////////////  NEURONAL NETWORK VARIABLES

// Dataset
int    NumExamples; //max is 60000 (see NumExamplesInitial)
double dataInput[NumExamplesInitial][NumInput];
double dataOutput[NumExamplesInitial][NumOutput];

// Input layer
double* input; // dataInput pointer to one sample

// Second layer (neurons)
double weightIH[NumInput+1][NumHidden]; // threhold=1 + input
double hidden[NumHidden];

// Third layer (neurons)
double weightHO[NumHidden+1][NumOutput]; // threhold=1 + hidden neurons
double output[NumOutput];
double* target; // dataOutput pointer to one sample


// pointer to functions
double (*funcError)(double output, double target);
double (*derivadaFuncError)(double output, double target);

double (*funcAcivacion)(double);
double (*derivadaFuncAcivacion)(double);

////////////////////////////////////////////  METHODS DECLARATION

void   test();
void   train();
void   forwardExample(int);
void   backPropagation(int);
void   backPropagationANTIGUO(int);
void   initializeWeights(double, double);
void   readCSV(char*, int);
void   readMNIST(char*, int);
void   printData();
void   printWeights();
int    outputNumber(double output[]);

double sigmoid(double);
double sigmoidPrime(double);
double relu(double);
double reluPrime(double);

double squaredError(double, double);
double squaredErrorPrime(double, double);

double randomInRange(double, double);


////////////////////////////////////////////////////////  METHODS

int main()
{
	funcError         = squaredError;
	derivadaFuncError = squaredErrorPrime;

	funcAcivacion         = sigmoid;
	derivadaFuncAcivacion = sigmoidPrime;


	system("clear");

	printf("Reading data...\n");
	readMNIST("mnist_train.csv", 60000); //readCSV("datos.csv", 5000);

	printf("Training...\n");
	train();

	printf("Reading test data...\n");
	readMNIST("mnist_test.csv", 10000);

	printf("Testing...\n");
	test();
	
	return 0;
}

void train()
{
	int correctClassified;
	double percentage;

	initializeWeights(minWeight, maxWeight);

	for(int i=0; i<trainingIterations; i++)
	{
		correctClassified = 0;
		percentage = 0;
		//printWeights();

		// Iterate examples
		for(int e=0; e<NumExamples; e++)
		{
			// Do the the back-propagation training
			backPropagation(e);

			if(outputNumber(output) == outputNumber(target))
				correctClassified++;

			int progress = 100*e/NumExamples;
			float accuracy = 100*correctClassified/NumExamples;
			printf("\rEpoch %d:\tProgress: %d%%\tAccuracy %.1f%%",i,progress,accuracy);
		}
		percentage = (double)correctClassified/NumExamples*100;
		printf("\rEpoch %d:\tCorrect classified %d of %d\t(%.1f%%).\n",
				i, correctClassified, NumExamples, percentage);
	}
}

void test()
{
	int correctClassified = 0;
	double percentage = 0;

	// Iterate examples
	for(int e=0; e<NumExamples; e++)
	{
		// Run the network
		forwardExample(e);

		// Save number of corrects digits classified
		if(outputNumber(output) == outputNumber(target))
			correctClassified++;		    
	}

	percentage = (double)correctClassified/NumExamples*100;
	printf("Correct classified %d of %d", correctClassified, NumExamples);
	printf(" (%.1f%%).\n", percentage);
}



void forwardExample(int sample)
{
	double net;

	input  = dataInput[sample];
	target = dataOutput[sample];

	// Hidden layer computation
	for(int j=0; j<NumHidden; j++)
	{	
		net = 0;	
		for(int i=0; i<NumInput; i++)
		{
			net += input[i] * weightIH[i][j];
		}
		net += weightIH[NumInput][j]; // Bias = 1 * another extra weight

		hidden[j] = funcAcivacion(net);
	}

	// Output layer computation
	for(int k=0; k<NumOutput; k++)
	{
		net = 0;
		for(int j=0; j<NumHidden; j++)
		{
			net += hidden[j] * weightHO[j][k];
		}
		net += weightHO[NumHidden][k]; // Bias = 1 * another extra weight

		output[k] = funcAcivacion(net);
	}
}

void backPropagationANTIGUO(int e)
{
	//double Error = 0.0;
	double DeltaH[NumHidden];
	double DeltaO[NumOutput];


	/////////////////////////////////////////  OBTAIN THE LAYERS OF THE NETWORK

	// Compute example and save its output
	forwardExample(e);

	////////////////////////////////////////////////////////  OBTAIN THE DELTAS

	// Calculate delta error of the output layer
	for(int k=0; k<NumOutput; k++)
	{
		//Error += 0.5 * pow(target[k]-output[k], 2);
		DeltaO[k] = (output[k] - target[k]) * // ANTES: target[k] - output[k]
					output[k] * (1.0 - output[k]);
	}

	// Calculate delta error of the hidden layer (back-propagate errors)
	for(int j=0; j<NumHidden; j++)
	{
		double Sum[NumHidden];
		Sum[j] = 0.0;

		for(int k=0; k<NumOutput; k++)
		{
			Sum[j] += weightHO[j][k] * DeltaO[k];
		}
		DeltaH[j] = Sum[j] * hidden[j] * (1.0 - hidden[j]);
	}

	///////////////////////////////////////////////////////  UPDATE THE WHEIGHS

	// Update weights WeightHO
	for(int k=0; k<NumOutput; k++)
	{
		for(int j=0; j<NumHidden; j++)
		{
			weightHO[j][k] -= learnigRate * hidden[j] * DeltaO[k]; // ANTES: +=
		}
		weightHO[NumHidden][k] -= learnigRate * DeltaO[k]; // Bias // ANTES: +=
	}

	// Update weights WeightIH
	for(int j=0; j<NumHidden; j++)
	{		
		for(int i=0; i<NumInput; i++)
		{
			weightIH[i][j] -= learnigRate * input[i] * DeltaH[j]; // ANTES: +=
		}
		weightIH[NumInput][j] -= learnigRate * DeltaH[j]; // Bias // ANTES: +=
	}
}


/////////////////////////////////////////////////////// NEW BACKPROPAGATION

void backPropagation(int e)
{
	double DeltaH[NumHidden];
	double DeltaO[NumOutput];


	// Compute example and save its output
	forwardExample(e);


	//////////// CHAIN RULE (computation graph)
	//
	//                capa oculta                        capa salida                  función de error
	//        ____________________________      ________________________________      __________________
	// x ---> z1=x*w1+b --->  h1=activ(z1) ---> z2=h1*w2+b ---> output=activ(z2) ---> loss(output,target)
	//   (5)            (4)                (3)             (2)                   (1)


	// (1) como cambia la funcion de error respecto al output
	//  => con la derivada de la función de error
	for(int i=0; i<NumOutput; ++i)
	{
		// CHAIN RULE (obtain gradients)
		double d1 = derivadaFuncError(output[i], target[i]);
		double d2 = derivadaFuncAcivacion(output[i]) * d1;
		DeltaO[i] = d2;

		// UPDATE WEIGHTS (GRADIENT DESCENT)
		for(int j=0; j<NumHidden; j++)
		{
			weightHO[j][i] -= learnigRate * hidden[j] * DeltaO[i];
		}
		weightHO[NumHidden][i] -= learnigRate * DeltaO[i]; // Bias
	}

	for(int i=0; i<NumHidden; i++)
	{
		// CHAIN RULE (obtain gradients)
		double d3 = 0.0;
		for(int j=0; j<NumOutput; j++)
		{
			d3 += weightHO[i][j] * DeltaO[j];
		}
		double d4 = derivadaFuncAcivacion(hidden[i]) * d3;
		DeltaH[i] = d4;

		// UPDATE WEIGHTS (GRADIENT DESCENT)
		for(int j=0; j<NumInput; j++)
		{
			weightIH[j][i] -= learnigRate * input[j] * DeltaH[i];
		}
		weightIH[NumInput][i] -= learnigRate * DeltaH[i]; // Bias
	}
}

///////////////////////////////////////////////////////  COST FUNCTION
/*
Función de coste
Mide lo bien que funciona nuestro modelo para todas las muestras
(es la media de la funcion de error para todas las muestras)

Hay que minimizar esta función,
por lo tanto tenemos que ir modificando los pesos para que el output sea difierente
y vaya siendo similar al target para que el error sea mínimo
*/

double J(output, target)
{
	double sum = 0;
	for (int i=0; i<NumExamples; ++i)
	{
		sum += funcError(output, target);
	}
	return sum/NumExamples;
}


///////////////////////////////////////////////////////  LOSS (ERROR) FUNCTIONS
/*
Función de error o pérdida:
Mide lo bien que funciona nuestro modelo para una muestra
*/


/*
Quadratic cost = mean squared error = maximum likelihood = sum squared error

El 0.5 creo que es para que la derivada sea fácil.

No se usa en regresión logística porque es una función de error no-convexa,
y GD se puede quedar atascado en un mínimo local
*/
double squaredError(double output, double target)
{
	return 0.5 * exp2(output - target);
}

double squaredErrorPrime(double output, double target)
{
	return output - target;
}

/*
Cross-entropy cost = Bernoulli negative log-likelihood = Binary Cross-Entropy
crossEntropy()


output entre [0, 1]
otra()
{
	-( target*log(output) + (1-target)*log(1-output) );
}


appropriate when working with logistic or softmax output layers
latter()
*/

///////////////////////////////////////////////////////  ACTIVATION FUNCTIONS

double sigmoid(double x)
{
	return 1.0/(1.0 + exp(-x));
}

double sigmoidPrime(double outputSigmoid)
{
	return outputSigmoid * (1.0 - outputSigmoid);
}

double relu(double x)
{
	return x>0 ? x : 0;
}

double reluPrime(double x)
{
	return x>0 ? 1 : 0;
}

/*TODO:
double softmax(double v*, int size)
{
	double output[size];

	for (int i = 0; i < size; ++i)
	{
	}
}*/


///////////////////////////////////////////////////////  LESS IMPORTANT METHODS

void initializeWeights(double minimum_number, double max_number)
{
	// Initialize weightIH
	for(int i=0; i<NumInput+1; i++)
	{		
		for(int j=0; j<NumHidden; j++)
		{
			weightIH[i][j] = randomInRange(minimum_number, max_number);
		}
	}

	// Initialize weightHO
	for(int j=0; j<NumHidden+1; j++)
	{
		for(int k=0; k<NumOutput; k++)
		{
			weightHO[j][k] = randomInRange(minimum_number, max_number);
		}
	}
}

double randomInRange(double minimum_number, double max_number)
{
	int precission = 10000;
	int minimum_number_int = minimum_number * precission;
	int max_number_int = max_number * precission;

	int myRand = (rand() % (max_number_int + 1 - minimum_number_int)) + minimum_number_int;

	return (double)myRand/(double)precission;
}

int outputNumber(double output[])
{
	double maxValue = 0;
	int index = -1;

	for(int k=0; k<NumOutput; k++)
	{
		if(output[k]>maxValue)
		{
			maxValue = output[k];
			index = k;
		}
	}
	return index;
}


////////////////////////////////////////////////////////////////  READ CSV DATA

void readCSV(char* fileName, int lines)
{
	NumExamples = lines;

	FILE* myfile = fopen(fileName, "r"); // Reading mode

	if (myfile == NULL) printf("Error opening file");

	for(int e=0; e<NumExamples; e++)
	{
		// READ INPUT
		for(int i=0; i<NumInput; i++)
		{
			fscanf(myfile,"%lf,",&dataInput[e][i]); // %lf = long float = double
		}

		// READ OUTPUT
		for(int i=0; i<NumOutput; i++)
		{
			fscanf(myfile,"%lf,",&dataOutput[e][i]); // %d = digit = int
		}
	}
	fclose(myfile);
}

void readMNIST(char* fileName, int lines)
{
	NumExamples = lines;

	FILE *myfile;
	double pixel;
	int realDigit;

	myfile=fopen(fileName, "r");

	for(int e=0; e<NumExamples; e++)
	{
		// READ TARGET OUTPUT
		fscanf(myfile,"%d,",&realDigit);
		for(int i=0; i<10; i++)
		{
			dataOutput[e][i] = 0.0;
		}
		dataOutput[e][realDigit] = 1.0;
		//printf("Number: %d \n",realDigit);

		// READ INPUT
		for(int i=0; i<NumInput; i++)
		{
			fscanf(myfile,"%lf,",&pixel);
			dataInput[e][i] = pixel/255;
			//printf("%.15f \n",pixel);
		}
	}
	fclose(myfile);
}

//////////////////////////////////////////////////////////  DEBUG PRINT METHODS

void printData()
{
	printf("Data:\n");
	
	for(int e=0; e<NumExamples; e++)
	{
		//  INPUT
		printf("[");
		for(int i=0; i<NumInput; i++)
		{
			printf("%f, ",dataInput[e][i]);
		}
		printf("]  [");

		// READ OUTPUT
		for(int i=0; i<NumOutput; i++)
		{
			printf("%f, ",dataOutput[e][i]);
		}
		printf("]\n");
	}

	printf("\n");
}

void printWeights()
{
	printf("double weightIH[%d][%d] = {\n", NumInput+1, NumHidden);
	for(int i=0; i<NumInput+1; i++)
	{
		printf("\t{");
		for(int j=0; j<NumHidden; j++)
		{
			printf("%f, ", weightIH[i][j]);
		}
		printf("},\n");
	}
	printf("};\n\n");

	printf("double weightHO[%d][%d] = {\n", NumHidden+1, NumOutput);
	for(int j=0; j<NumHidden+1; j++)
	{
		printf("\t{");
		for(int k=0; k<NumOutput; k++)
		{
			printf("%f, ", weightHO[j][k]);
		}
		printf("},\n");
	}
	printf("};\n\n");
}


/////////////////////////////////////////////////  SAMPLE OUTPUT OF THE PROGRAM
/*

Reading data...
Training...
Epoch 0:	Correct classified 52305 of 60000	(87.2%).
Epoch 1:	Correct classified 54571 of 60000	(91.0%).
Epoch 2:	Correct classified 55043 of 60000	(91.7%).
Epoch 3:	Correct classified 55173 of 60000	(92.0%).
Epoch 4:	Correct classified 55422 of 60000	(92.4%).
Epoch 5:	Correct classified 55552 of 60000	(92.6%).
Epoch 6:	Correct classified 55655 of 60000	(92.8%).
Epoch 7:	Correct classified 55723 of 60000	(92.9%).
Epoch 8:	Correct classified 55804 of 60000	(93.0%).
Epoch 9:	Correct classified 55892 of 60000	(93.2%).
Reading test data...
Testing...
Correct classified 9242 of 10000 (92.4%).

*/

