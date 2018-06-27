#include <cuda.h>
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <device_functions.h>
#include <stdio.h>
#include <cstdint>
#include <mpir.h>
#include "string.h"

// TODO: Repair mode, select an area to re-render at a higher sample rate

#define cudaCheckError() {                                          \
 cudaError_t e=cudaGetLastError();                                 \
 if(e!=cudaSuccess) {                                              \
   printf("Cuda failure %d %s:%d: '%s'\n",e,__FILE__,__LINE__,cudaGetErrorString(e));           \
   /*exit(0);*/ \
 }                                                                 \
}
typedef struct mpfloat {
	uint8_t *digits;
	uint64_t decimal;
	bool negative;
} mpfloat;

__device__ uint32_t mpfloat_precision;


// Device Functions
__device__ void mpfloat_init(mpfloat *multifloat);
__device__ void mpfloat_free(mpfloat multifloat);
__device__ void mpfloat_clear(mpfloat *multifloat);

__device__ void mpfloat_set_uint32(uint32_t a, mpfloat *result);
__device__ void mpfloat_set_mpfloat(mpfloat *a, mpfloat b);

__device__ int8_t mpfloat_cmp(mpfloat a, mpfloat b);
__device__ int8_t mpfloat_cmp_ui(mpfloat a, uint32_t b);

__device__ void mpfloat_pack_left(mpfloat *multifloat);
__device__ void mpfloat_shift_right_insert(mpfloat *multifloat, uint8_t digit);

__device__ void mpfloat_get_extent(mpfloat a, uint64_t *extent);
__device__ uint32_t mpfloat_get_num_digits(mpfloat multifloat);
__device__ uint32_t mpfloat_get_num_decimals(mpfloat multifloat);

__device__ void mpfloat_add(mpfloat a, mpfloat b, mpfloat *result);
__device__ void mpfloat_sub(mpfloat a, mpfloat b, mpfloat *result);

__device__ void mpfloat_mul(mpfloat a, mpfloat b, mpfloat *result);
__device__ void mpfloat_mul_ui(mpfloat a, uint32_t b, mpfloat *result);

__device__ void mpfloat_to_str(mpfloat multifloat, char *buffer, uint32_t length);
__device__ void mpfloat_to_double(mpfloat multifloat, double *result);

// Host Functions
void mpfloat_sub_host(mpfloat a, mpfloat b, mpfloat *result);
void mpfloat_add_host(mpfloat a, mpfloat b, mpfloat *result);
void mpfloat_mul_host(mpfloat a, mpfloat b, mpfloat *result);
void mpfloat_mul_ui_host(mpfloat a, uint32_t b, mpfloat *result);
int8_t mpfloat_cmp_host(mpfloat a, mpfloat b);

void mpfloat_set_mpfloat_host(mpfloat *a, mpfloat b);
void mpfloat_set_mpir(mpfloat *multifloat, mpf_t mpirfloat);
void mpfloat_set_str_host(mpfloat *multifloat, char *floatString);
void mpfloat_set_double_host(double a, mpfloat *result);

void mpfloat_set_precision(uint32_t precision);


void mpfloat_get_extent_host(mpfloat a, uint64_t *extent);
uint32_t mpfloat_get_num_decimals_host(mpfloat multifloat);
uint32_t mpfloat_get_num_digits_host(mpfloat multifloat);

void mpfloat_init_host(mpfloat *multifloat);
void mpfloat_free_host(mpfloat multifloat);
void mpfloat_clear_host(mpfloat *multifloat);
void mpfloat_to_str_host(mpfloat multifloat, char *buffer, uint32_t length);

void mpfloat_test_add_host();

struct test_data
{
	char *A;
	char *B;
	char *Solution;
};

#define NUM_ADD_TEST_INPUTS 10
struct test_data add_test_input[NUM_ADD_TEST_INPUTS] = {
	{ "9.99999999","0.00000001","10" },
	{ "0","1.234","1.234" },
	{ "9.8634","0.9463","10.8097" },
	{ "99.8634","0.9463","100.8097" },
	{ "0","0.9463","0.9463" },
	{ "1","2","3" },
	{ "1","0.0000002","1.0000002" },
	{ "1","0","1" },
	{ "0.99999999","0.00000001","1" },
	{ "0","0","0" }
};

void mpfloat_test_add_host()
{
	char ABuff[100];
	char BBuff[100];
	char ResultBuff[100];
	char SolutionBuff[100];
	mpfloat A;
	mpfloat_init_host(&A);
	mpfloat B;
	mpfloat_init_host(&B);
	mpfloat Result;
	mpfloat_init_host(&Result);
	mpfloat Solution;
	mpfloat_init_host(&Solution);
	bool Correct;
	bool GotFailure = false;
	int testsPassed = 0;

	fprintf(stdout, "\nADD TEST\n----------------\n");

	for (uint32_t TestInputIter = 0; TestInputIter < NUM_ADD_TEST_INPUTS; TestInputIter++)
	{
		GotFailure = false;
		mpfloat_clear_host(&Result);
		mpfloat_set_str_host(&A, add_test_input[TestInputIter].A);
		mpfloat_set_str_host(&B, add_test_input[TestInputIter].B);
		mpfloat_set_str_host(&Solution, add_test_input[TestInputIter].Solution);
		mpfloat_add_host(A, B, &Result);
		Correct = (mpfloat_cmp_host(Result, Solution) == 0);
		if (!Correct)
		{
			GotFailure = true;
		}
		mpfloat_to_str_host(A, ABuff, 98);
		mpfloat_to_str_host(B, BBuff, 98);
		mpfloat_to_str_host(Result, ResultBuff, 98);
		mpfloat_to_str_host(Solution, SolutionBuff, 98);
		fprintf(stdout, "%s + %s\nExpected: %s\nActual:   %s \n%s\n", ABuff, BBuff, SolutionBuff, ResultBuff, Correct ? "PASS":"FAIL");
		mpfloat_clear_host(&Result);
		mpfloat_add_host(B, A, &Result);
		Correct = (mpfloat_cmp_host(Result, Solution) == 0);
		if (!Correct)
		{
			GotFailure = true;
		}
		mpfloat_to_str_host(Result, ResultBuff, 98);
		fprintf(stdout, "%s + %s\nExpected: %s\nActual:   %s \n%s\n", BBuff, ABuff, SolutionBuff, ResultBuff, Correct ? "PASS" : "FAIL");
		fprintf(stdout, "Test Case %u : %s\n", TestInputIter, GotFailure ? "FAIL" : "PASS");
		fprintf(stdout, "---------------------------\n\n");
		if (!GotFailure)
		{
			testsPassed++;
		}
	}
	fprintf(stdout, "Passed %d of %d Tests. Score: %f\n\n", testsPassed, NUM_ADD_TEST_INPUTS,100*((double)testsPassed/(double)NUM_ADD_TEST_INPUTS));

}

#define NUM_SUB_TEST_INPUTS 10
struct test_data sub_test_input[NUM_ADD_TEST_INPUTS] = {
	{"10","9.99999999","0.00000001"},
	{ "0","1.234","-1.234" },
	{ "9.8634","0.9463","10.8097" },
	{ "99.8634","0.9463","100.8097" },
	{ "0","0.9463","0.9463" },
	{ "1","2","3" },
	{ "1","0.0000002","1.0000002" },
	{ "1","0","1" },
	{ "0.99999999","0.00000001","1" },
	{ "0","0","0" }
};

void mpfloat_test_sub_host()
{
	char ABuff[100];
	char BBuff[100];
	char ResultBuff[100];
	char SolutionBuff[100];
	mpfloat A;
	mpfloat_init_host(&A);
	mpfloat B;
	mpfloat_init_host(&B);
	mpfloat Result;
	mpfloat_init_host(&Result);
	mpfloat Solution;
	mpfloat_init_host(&Solution);
	bool Correct;
	bool GotFailure = false;
	int testsPassed = 0;

	fprintf(stdout, "\nSUB TEST\n----------------\n");

	for (uint32_t TestInputIter = 0; TestInputIter < NUM_SUB_TEST_INPUTS; TestInputIter++)
	{
		GotFailure = false;
		mpfloat_clear_host(&Result);
		mpfloat_set_str_host(&A, sub_test_input[TestInputIter].A);
		mpfloat_set_str_host(&B, sub_test_input[TestInputIter].B);
		mpfloat_set_str_host(&Solution, sub_test_input[TestInputIter].Solution);
		mpfloat_sub_host(A, B, &Result);
		Correct = (mpfloat_cmp_host(Result, Solution) == 0);
		if (!Correct)
		{
			GotFailure = true;
		}
		mpfloat_to_str_host(A, ABuff, 98);
		mpfloat_to_str_host(B, BBuff, 98);
		mpfloat_to_str_host(Result, ResultBuff, 98);
		mpfloat_to_str_host(Solution, SolutionBuff, 98);
		fprintf(stdout, "%s - %s\nExpected: %s\nActual:   %s \n%s\n", ABuff, BBuff, SolutionBuff, ResultBuff, Correct ? "PASS" : "FAIL");
		fprintf(stdout, "Test Case %u : %s\n", TestInputIter, GotFailure ? "FAIL" : "PASS");
		fprintf(stdout, "---------------------------\n\n");
		if (!GotFailure)
		{
			testsPassed++;
		}
	}
	fprintf(stdout, "Passed %d of %d Tests. Score: %f\n\n", testsPassed, NUM_SUB_TEST_INPUTS, 100 * ((double)testsPassed / (double)NUM_SUB_TEST_INPUTS));

}

#define NUM_MUL_TEST_INPUTS 9
struct test_data mul_test_input[NUM_MUL_TEST_INPUTS] = {
	{ "-10","1","-10" },
	{ "-1","1","-1" },
	{ "-10.001","-0.1","1.0001" },
	{ "2","3","6" },
	{ "-0.022","-3.998","0.087956" },
	{ "6.983","7.423","51.834809" },
	{ "0.55487","28.513873","15.82149271151" },
	{"1","1","1"},
	{"1","0","0"}
};

void mpfloat_test_mul_host()
{
	char ABuff[100];
	char BBuff[100];
	char ResultBuff[100];
	char SolutionBuff[100];
	mpfloat A;
	mpfloat_init_host(&A);
	mpfloat B;
	mpfloat_init_host(&B);
	mpfloat Result;
	mpfloat_init_host(&Result);
	mpfloat Solution;
	mpfloat_init_host(&Solution);
	bool Correct;
	bool GotFailure = false;
	int testsPassed = 0;

	fprintf(stdout, "\nMUL TEST\n----------------\n");

	for (uint32_t TestInputIter = 0; TestInputIter < NUM_MUL_TEST_INPUTS; TestInputIter++)
	{
		GotFailure = false;
		mpfloat_clear_host(&Result);
		mpfloat_set_str_host(&A, mul_test_input[TestInputIter].A);
		mpfloat_set_str_host(&B, mul_test_input[TestInputIter].B);
		mpfloat_set_str_host(&Solution, mul_test_input[TestInputIter].Solution);
		mpfloat_mul_host(A, B, &Result);
		Correct = (mpfloat_cmp_host(Result, Solution) == 0);
		if (!Correct)
		{
			GotFailure = true;
		}
		mpfloat_to_str_host(A, ABuff, 98);
		mpfloat_to_str_host(B, BBuff, 98);
		mpfloat_to_str_host(Result, ResultBuff, 98);
		mpfloat_to_str_host(Solution, SolutionBuff, 98);
		fprintf(stdout, "%s * %s\nExpected: %s\nActual:   %s \n%s\n", ABuff, BBuff, SolutionBuff, ResultBuff, Correct ? "PASS" : "FAIL");
		fprintf(stdout, "Test Case %u : %s\n", TestInputIter, GotFailure ? "FAIL" : "PASS");
		fprintf(stdout, "---------------------------\n\n");
		if (!GotFailure)
		{
			testsPassed++;
		}
	}
	fprintf(stdout, "Passed %d of %d Tests. Score: %f\n\n", testsPassed, NUM_MUL_TEST_INPUTS, 100 * ((double)testsPassed / (double)NUM_MUL_TEST_INPUTS));

}

void mpfloat_set_precision(uint32_t precision)
{
	mpfloat_precision = precision;
	cudaMemcpyToSymbol("mpfloat_precision", &precision, 0, 0, cudaMemcpyHostToDevice);
}

__device__ void mpfloat_free(mpfloat multifloat)
{
	free(multifloat.digits);
}

void mpfloat_free_host(mpfloat multifloat)
{
	cudaFree(multifloat.digits);
}

void mpfloat_clear_host(mpfloat *multifloat)
{
	for (uint32_t digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		multifloat->digits[digitIter] = 0;
	}
	multifloat->decimal = 0;
	multifloat->negative = false;
}

void mpfloat_init_host(mpfloat *multifloat)
{
	//multifloat->digits = new uint8_t[mpfloat_precision]();
	cudaError_t status = cudaMallocManaged(&multifloat->digits, mpfloat_precision * sizeof(uint8_t));
	if (status == 0)
	{
		mpfloat_clear_host(multifloat);
	}
	else
	{
		fprintf(stdout, "fuck\n");
	}
}

void mpfloat_set_str_host(mpfloat *multifloat, char *floatString)
{
	uint32_t mpfloatDigitIter = 0;
	uint32_t charDigitIter;
	uint32_t stringLength = strlen(floatString);
	bool FoundDecimal = false;
	bool NonZeroLeadingDigit = false;

	mpfloat_clear_host(multifloat);
	if (stringLength == 1 && floatString[0] == '0')
	{
		return;
	}

	for (charDigitIter = 0; charDigitIter < stringLength; charDigitIter++)
	{
		if (charDigitIter == 0 && floatString[charDigitIter] == '-')
		{
			multifloat->negative = true;
		}
		else if (floatString[charDigitIter] == '.')
		{
			multifloat->decimal = mpfloatDigitIter;
			FoundDecimal = true;
			NonZeroLeadingDigit = true;
		}
		else
		{
			uint8_t DigitToAdd = floatString[charDigitIter] - 48;
			if (!NonZeroLeadingDigit)
			{
				if (DigitToAdd)
				{
					NonZeroLeadingDigit = true;
				}
				else
				{
					continue;
				}
			}
			multifloat->digits[mpfloatDigitIter++] = DigitToAdd;
		}
	}

	if (multifloat->decimal == 0 && !FoundDecimal)
	{
		multifloat->decimal = charDigitIter - (int)(multifloat->negative);
	}
}

__device__ void mpfloat_clear(mpfloat *multifloat)
{
	if (multifloat->digits != nullptr)
	{
		for (int digitIter = 0; digitIter < mpfloat_precision; digitIter++)
		{
			multifloat->digits[digitIter] = 0;
		}
	}
	multifloat->decimal = 0;
	multifloat->negative = false;
}

__device__ void mpfloat_init(mpfloat *multifloat)
{
	multifloat->digits = new uint8_t[mpfloat_precision]();
	//cudaMalloc(&multifloat->digits, mpfloat_precision * sizeof(uint8_t));
	mpfloat_clear(multifloat);
}

__device__ void mpfloat_shift_right_insert(mpfloat *multifloat, uint8_t digit)
{
	uint8_t ReplaceDigit = digit;
	uint8_t HolderDigit;

	for (uint32_t digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		HolderDigit = multifloat->digits[digitIter];
		multifloat->digits[digitIter] = ReplaceDigit;
		ReplaceDigit = HolderDigit;
	}

	if (digit)
	{
		multifloat->decimal++;
	}
}

void mpfloat_shift_right_insert_host(mpfloat *multifloat, uint8_t digit, bool IncrementDecimal)
{
	uint8_t ReplaceDigit = digit;
	uint8_t HolderDigit;

	for (uint32_t digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		HolderDigit = multifloat->digits[digitIter];
		multifloat->digits[digitIter] = ReplaceDigit;
		ReplaceDigit = HolderDigit;
	}

	if (IncrementDecimal)
	{
		multifloat->decimal++;
	}
}

__device__  void mpfloat_pack_left(mpfloat *multifloat)
{
	uint32_t NumLeadingZeroes = 0;
	for (int digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		if (multifloat->digits[digitIter])
		{
			break;
		}
		NumLeadingZeroes++;
	}
	multifloat->decimal -= NumLeadingZeroes;

	for (int digitIter = 0; digitIter < (mpfloat_precision - NumLeadingZeroes); digitIter++)
	{
		multifloat->digits[digitIter] = multifloat->digits[digitIter + NumLeadingZeroes];
	}
}

void mpfloat_pack_left_host(mpfloat *multifloat)
{
	uint32_t NumLeadingZeroes = 0;
	for (int digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		if (multifloat->digits[digitIter])
		{
			break;
		}
		NumLeadingZeroes++;
	}
	multifloat->decimal -= NumLeadingZeroes;

	for (int digitIter = 0; digitIter < (mpfloat_precision - NumLeadingZeroes); digitIter++)
	{
		multifloat->digits[digitIter] = multifloat->digits[digitIter + NumLeadingZeroes];
	}
}

__device__ int8_t mpfloat_cmp(mpfloat a, mpfloat b)
{
	// a > b  -->  1
	// a == b -->  0
	// a < b  --> -1

	if (a.negative && !b.negative)
	{
		return -1;
	}
	else if (b.negative && !a.negative)
	{
		return 1;
	}

	int8_t result = 0;

	if (a.decimal > b.decimal)
	{
		result = 1;
	}
	else if (a.decimal < b.decimal)
	{
		result = -1;
	}
	else // a.decimal == b.decimal
	{
		for (uint32_t digitIter = 0; digitIter < mpfloat_precision; digitIter++)
		{
			if (a.digits[digitIter] > b.digits[digitIter])
			{
				result = 1;
				break;
			}
			else if (a.digits[digitIter] < b.digits[digitIter])
			{
				result = -1;
				break;
			}
		}
	}

	if (b.negative && a.negative)
	{
		result *= -1;
	}

	return result;
}

int8_t mpfloat_cmp_host(mpfloat a, mpfloat b)
{
	// a > b  -->  1
	// a == b -->  0
	// a < b  --> -1

	if (a.negative && !b.negative)
	{
		return -1;
	}
	else if (b.negative && !a.negative)
	{
		return 1;
	}

	int8_t result = 0;

	if (a.decimal > b.decimal)
	{
		result = 1;
	}
	else if (a.decimal < b.decimal)
	{
		result = -1;
	}
	else // a.decimal == b.decimal
	{
		for (uint32_t digitIter = 0; digitIter < mpfloat_precision; digitIter++)
		{
			if (a.digits[digitIter] > b.digits[digitIter])
			{
				result = 1;
				break;
			}
			else if (a.digits[digitIter] < b.digits[digitIter])
			{
				result = -1;
				break;
			}
		}
	}

	if (b.negative && a.negative)
	{
		result *= -1;
	}

	return result;
}

__device__ int8_t mpfloat_cmp_ui(mpfloat a, uint32_t b)
{
	int8_t result;
	mpfloat tempfloat;
	mpfloat_init(&tempfloat);
	mpfloat_set_uint32(b, &tempfloat);
	result = mpfloat_cmp(a, tempfloat);
	mpfloat_free(tempfloat);
	return result;
}

uint32_t mpfloat_get_num_decimals_host(mpfloat multifloat)
{
	return mpfloat_get_num_digits_host(multifloat) - multifloat.decimal;
}

__device__  uint32_t mpfloat_get_num_decimals(mpfloat multifloat)
{
	return mpfloat_get_num_digits(multifloat) - multifloat.decimal;
}

uint32_t mpfloat_get_num_digits_host(mpfloat multifloat)
{
	uint32_t digitIter;

	//for (digitIter = (mpfloat_precision - 1); !multifloat.digits[digitIter]; digitIter--){}
	digitIter = mpfloat_precision - 1;
	while (true)
	{
		if (multifloat.digits[digitIter])
		{
			break;
		}

		if (digitIter == (multifloat.decimal - 1))
		{
			break;
		}
		
		if (digitIter == 0)
		{
			break;
		}
		digitIter--;
	}
	//for (digitIter = (mpfloat_precision - 1); !multifloat.digits[digitIter]; digitIter--) {}

	return digitIter + 1;
}

__device__  uint32_t mpfloat_get_num_digits(mpfloat multifloat)
{
	int32_t digitIter;

	//for (digitIter = (mpfloat_precision - 1); !multifloat.digits[digitIter]; digitIter--){}
	digitIter = mpfloat_precision - 1;
	while (digitIter >= 0)
	{
		if (multifloat.digits[digitIter])
		{
			break;
		}
		digitIter--;
	}
	//for (digitIter = (mpfloat_precision - 1); !multifloat.digits[digitIter]; digitIter--) {}

	return digitIter + 1;
}

__device__ void mpfloat_mul_ui(mpfloat a, uint32_t b, mpfloat *result)
{
	mpfloat tempfloat;
	mpfloat_init(&tempfloat);
	mpfloat_set_uint32(b, &tempfloat);

	mpfloat_mul(a, tempfloat, result);

	mpfloat_free(tempfloat);
}

__device__ void mpfloat_mul(mpfloat a, mpfloat b, mpfloat *result)
{
	char buff[100];
	mpfloat tempfloat;
	mpfloat resultCopy;
	mpfloat_init(&tempfloat);
	mpfloat_init(&resultCopy);



	int32_t DecimalOffset = b.decimal - a.decimal;
	uint8_t Carry = 0;

	uint32_t aDecimalCount = mpfloat_get_num_decimals(a);
	uint32_t aDigitCount = mpfloat_get_num_digits(a);

	uint32_t bDecimalCount = mpfloat_get_num_decimals(b);
	uint32_t bDigitCount = mpfloat_get_num_digits(b);

	if (bDigitCount == 0 || aDigitCount == 0)
	{
		// multiply by zero is zero
		mpfloat_clear(result);
		mpfloat_free(tempfloat);
		mpfloat_free(resultCopy);
		return;
	}

	uint32_t productDecimalCount = aDecimalCount + bDecimalCount;
	uint32_t tempfloatDecimalsAdded;
	int32_t aDigitIter;
	int32_t bDigitIter;
	uint8_t product = 0;


	for (aDigitIter = aDigitCount - 1; aDigitIter >= 0; aDigitIter--)
	{
		if (a.digits[aDigitIter])
		{
			mpfloat_clear(&tempfloat);
			tempfloatDecimalsAdded = 0;
			for (bDigitIter = bDigitCount - 1; bDigitIter >= 0; bDigitIter--)
			{
				product = a.digits[aDigitIter] * b.digits[bDigitIter] + Carry;
				tempfloat.digits[bDigitIter] = product % 10;
				Carry = (product - tempfloat.digits[bDigitIter]) / 10;
				tempfloatDecimalsAdded++;
			}
			if (Carry)
			{
				tempfloatDecimalsAdded++;
				mpfloat_shift_right_insert(&tempfloat, Carry);
				Carry = 0;
			}
			for (; tempfloatDecimalsAdded < productDecimalCount; tempfloatDecimalsAdded++)
			{
				mpfloat_shift_right_insert(&tempfloat, 0);
			}
			tempfloat.decimal = tempfloatDecimalsAdded - productDecimalCount;
			mpfloat_set_mpfloat(&resultCopy, *result);
			mpfloat_add(tempfloat, resultCopy, result);

			mpfloat_to_str(tempfloat, buff, 99);
			printf("tempfloat: %s \n", buff);
			mpfloat_to_str(resultCopy, buff, 99);
			printf("resultCopy: %s \n", buff);
			mpfloat_to_str(*result, buff, 99);
			printf("result: %s \n------------\n", buff);
		}
		productDecimalCount--;
	}

	result->negative = (a.negative != b.negative);

	mpfloat_free(tempfloat);
	mpfloat_free(resultCopy);
}

void mpfloat_mul_ui_host(mpfloat a, uint32_t b, mpfloat *result)
{

}

void mpfloat_mul_host(mpfloat a, mpfloat b, mpfloat *result)
{
	mpfloat tempfloat;
	mpfloat resultCopy;
	mpfloat_init_host(&tempfloat);
	mpfloat_init_host(&resultCopy);

	int32_t DecimalOffset = b.decimal - a.decimal;
	uint8_t Carry = 0;

	uint32_t aDecimalCount = mpfloat_get_num_decimals_host(a);
	uint32_t aDigitCount = mpfloat_get_num_digits_host(a);

	uint32_t bDecimalCount = mpfloat_get_num_decimals_host(b);
	uint32_t bDigitCount = mpfloat_get_num_digits_host(b);

	if (bDigitCount == 0 || aDigitCount == 0)
	{
		// multiply by zero is zero
		mpfloat_clear_host(result);
		mpfloat_free_host(tempfloat);
		mpfloat_free_host(resultCopy);
		return;
	}

	int32_t productDecimalCount = aDecimalCount + bDecimalCount;
	int32_t tempfloatDecimalsAdded;
	int32_t aDigitIter;
	int32_t bDigitIter;
	uint8_t product = 0;


	for (aDigitIter = aDigitCount - 1; aDigitIter >= 0; aDigitIter--)
	{
		if (a.digits[aDigitIter])
		{
			mpfloat_clear_host(&tempfloat);
			tempfloatDecimalsAdded = 0;
			for (bDigitIter = bDigitCount - 1; bDigitIter >= 0; bDigitIter--)
			{
				product = a.digits[aDigitIter] * b.digits[bDigitIter] + Carry;
				tempfloat.digits[bDigitIter] = product % 10;
				Carry = (product - tempfloat.digits[bDigitIter]) / 10;
				tempfloatDecimalsAdded++;
			}
			if (Carry)
			{
				tempfloatDecimalsAdded++;
				mpfloat_shift_right_insert_host(&tempfloat, Carry, true);
				Carry = 0;
			}
			for (; tempfloatDecimalsAdded < productDecimalCount; tempfloatDecimalsAdded++)
			{
				mpfloat_shift_right_insert_host(&tempfloat, 0, true);
			}
			tempfloat.decimal = tempfloatDecimalsAdded - productDecimalCount;
			mpfloat_set_mpfloat_host(&resultCopy, *result);
			mpfloat_add_host(tempfloat, resultCopy, result);
		}
		productDecimalCount--;
	}

	result->negative = (a.negative != b.negative);

	mpfloat_free_host(tempfloat);
	mpfloat_free_host(resultCopy);
}


__device__  void mpfloat_sub(mpfloat a, mpfloat b, mpfloat *result)
{
	bool bIsGreater = (mpfloat_cmp(a, b) == -1);
	if (a.negative && !b.negative)
	{
		// -a - b == -(a + b)
		b.negative = true;
		mpfloat_add(a, b, result);
		return;
	}
	else if (b.negative && !a.negative)
	{
		// a - (-b) == a + b
		mpfloat_add(a, b, result);
		return;
	}
	else if ((a.negative && b.negative) || bIsGreater)
	{
		// -a - (-b) == -a + b == b - a
		mpfloat temp;
		temp.decimal = a.decimal;
		temp.digits = a.digits;
		temp.negative = a.negative;

		a.decimal = b.decimal;
		a.digits = b.digits;
		a.negative = b.negative;

		b.decimal = temp.decimal;
		b.digits = temp.digits;
		b.negative = temp.negative;
	}



	int32_t DecimalOffset = b.decimal - a.decimal;
	uint32_t DigitIndex;
	int32_t Difference;
	if (a.decimal > b.decimal)
	{
		result->decimal = a.decimal;
	}
	else
	{
		result->decimal = b.decimal;
	}

	for (uint32_t digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		DigitIndex = mpfloat_precision - (digitIter + 1);
		if ((DigitIndex + DecimalOffset) >= mpfloat_precision)
		{
			continue;
		}

		Difference = a.digits[DigitIndex] - b.digits[DigitIndex + DecimalOffset];
		if (Difference < 0)
		{
			if (result->negative)
			{
				Difference *= -1;
			}
			else
			{
				for (int32_t BorrowIndex = (DigitIndex - 1); BorrowIndex >= 0; BorrowIndex--)
				{
					if (a.digits[BorrowIndex])
					{
						a.digits[BorrowIndex]--;
						for (uint32_t BorrowedDigitIter = 1; BorrowedDigitIter < (DigitIndex - BorrowIndex); BorrowedDigitIter++)
						{
							a.digits[BorrowIndex + BorrowedDigitIter] = 9;
						}
						Difference += 10;
						break;
					}
				}
				if (Difference < 0)
				{
					result->negative = true;
					Difference *= -1;
				}
			}
		}

		result->digits[DigitIndex] = Difference;

	}

	if (DecimalOffset < 0)
	{
		uint32_t AbsOffset = DecimalOffset * -1;
		for (uint32_t LeftOverDigitIter = 0; LeftOverDigitIter < AbsOffset; LeftOverDigitIter++)
		{
			result->digits[LeftOverDigitIter] = a.digits[LeftOverDigitIter];
		}
	}

	if (bIsGreater)
	{
		result->negative = true;
	}

	if (!result->digits[0])
	{
		mpfloat_pack_left(result);
	}

}

void mpfloat_sub_host(mpfloat a, mpfloat b, mpfloat *result)
{
	bool bIsGreater = (mpfloat_cmp_host(a, b) == -1);
	if (a.negative && !b.negative)
	{
		// -a - b == -(a + b)
		b.negative = true;
		mpfloat_add_host(a, b, result);
		return;
	}
	else if (b.negative && !a.negative)
	{
		// a - (-b) == a + b
		mpfloat_add_host(a, b, result);
		return;
	}
	else if ((a.negative && b.negative) || bIsGreater)
	{
		// -a - (-b) == -a + b == b - a
		mpfloat temp;
		temp.decimal = a.decimal;
		temp.digits = a.digits;
		temp.negative = a.negative;

		a.decimal = b.decimal;
		a.digits = b.digits;
		a.negative = b.negative;

		b.decimal = temp.decimal;
		b.digits = temp.digits;
		b.negative = temp.negative;
	}



	int32_t DecimalOffset = b.decimal - a.decimal;
	uint32_t DigitIndex;
	int32_t Difference;
	if (a.decimal > b.decimal)
	{
		result->decimal = a.decimal;
	}
	else
	{
		result->decimal = b.decimal;
	}

	for (uint32_t digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		DigitIndex = mpfloat_precision - (digitIter + 1);
		if ((DigitIndex + DecimalOffset) >= mpfloat_precision)
		{
			continue;
		}

		Difference = a.digits[DigitIndex] - b.digits[DigitIndex + DecimalOffset];
		if (Difference < 0)
		{
			if (result->negative)
			{
				Difference *= -1;
			}
			else
			{
				for (int32_t BorrowIndex = (DigitIndex - 1); BorrowIndex >= 0; BorrowIndex--)
				{
					if (a.digits[BorrowIndex])
					{
						a.digits[BorrowIndex]--;
						for (uint32_t BorrowedDigitIter = 1; BorrowedDigitIter < (DigitIndex - BorrowIndex); BorrowedDigitIter++)
						{
							a.digits[BorrowIndex + BorrowedDigitIter] = 9;
						}
						Difference += 10;
						break;
					}
				}
				if (Difference < 0)
				{
					result->negative = true;
					Difference *= -1;
				}
			}
		}

		result->digits[DigitIndex] = Difference;

	}

	if (DecimalOffset < 0)
	{
		uint32_t AbsOffset = DecimalOffset * -1;
		for (uint32_t LeftOverDigitIter = 0; LeftOverDigitIter < AbsOffset; LeftOverDigitIter++)
		{
			result->digits[LeftOverDigitIter] = a.digits[LeftOverDigitIter];
		}
	}

	if (bIsGreater)
	{
		result->negative = true;
	}

	if (!result->digits[0])
	{
		mpfloat_pack_left_host(result);
	}

}

__device__  void mpfloat_add(mpfloat a, mpfloat b, mpfloat *result)
{
	if (a.negative && !b.negative)
	{
		// -a + b == b - a
		a.negative = false;
		mpfloat_sub(b, a, result);
		return;
	}
	else if (b.negative && !a.negative)
	{
		// a + (-b) == a - b
		b.negative = false;
		mpfloat_sub(a, b, result);
		return;
	}
	else if (a.negative && b.negative)
	{
		// -a + (-b) == -(a + b)

		result->negative = true;
	}
	char *msg = new char[500];
	int32_t DecimalOffset = b.decimal - a.decimal;
	uint8_t Sum;
	uint8_t Carry = 0;
	uint32_t DigitIndex;

	result->decimal = a.decimal;

	for (uint32_t digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		DigitIndex = mpfloat_precision - (digitIter + 1);
		if ((DigitIndex + DecimalOffset) >= mpfloat_precision)
		{
			continue;
		}
		else if ((DigitIndex + DecimalOffset) < 0)
		{
			break;
		}
		Sum = a.digits[DigitIndex] + b.digits[DigitIndex + DecimalOffset] + (int)Carry;
		if (Sum >= 10)
		{
			Carry = 1;
			Sum -= 10;
		}
		else
		{
			Carry = 0;
		}
		result->digits[DigitIndex] = Sum;
	}

	if (DecimalOffset > 0)
	{
		for (uint32_t LeftOverDigitIter = 0; LeftOverDigitIter < DecimalOffset; LeftOverDigitIter++)
		{
			mpfloat_shift_right_insert(result, b.digits[DecimalOffset - (LeftOverDigitIter + 1)]);
		}
	}
	else if (DecimalOffset < 0)
	{
		uint32_t AbsOffset = DecimalOffset * -1;
		for (uint32_t LeftOverDigitIter = 0; LeftOverDigitIter < AbsOffset; LeftOverDigitIter++)
		{
			result->digits[LeftOverDigitIter] = a.digits[LeftOverDigitIter];
		}
	}
}

void mpfloat_add_host(mpfloat a, mpfloat b, mpfloat *result)
{
	if (a.negative && !b.negative)
	{
		// -a + b == b - a
		a.negative = false;
		//mpfloat_sub(b, a, result);
		return;
	}
	else if (b.negative && !a.negative)
	{
		// a + (-b) == a - b
		b.negative = false;
		//mpfloat_sub(a, b, result);
		return;
	}
	else if (a.negative && b.negative)
	{
		// -a + (-b) == -(a + b)

		result->negative = true;
	}

	int32_t DecimalOffset = b.decimal - a.decimal;
	uint8_t Sum;
	uint8_t Carry = 0;
	int32_t DigitIndex;
	uint32_t CarryDigitsAdded = 0;
	int32_t digitIter = 0;

	result->decimal = a.decimal;

	for (digitIter = mpfloat_precision-1; digitIter >= 0; digitIter--)
	{
		if ((digitIter + DecimalOffset) < 0)
		{
			break;
		}
		else if ((digitIter + DecimalOffset) >= (mpfloat_precision-1))
		{
			continue;
		}

		Sum = a.digits[digitIter] + b.digits[digitIter + DecimalOffset] + (int)Carry;
		if (Sum >= 10)
		{
			Carry = 1;
			Sum -= 10;
		}
		else
		{
			Carry = 0;
		}
		result->digits[digitIter] = Sum;
	}

	while (Carry)
	{
		if ((digitIter + DecimalOffset) >= 0)
		{
			if (DecimalOffset > 0)
			{
				Sum = b.digits[digitIter + DecimalOffset] + Carry;
			}
			else if (DecimalOffset < 0)
			{
				Sum = a.digits[digitIter + DecimalOffset] + Carry;
			}
			else
			{
				Sum = Carry;
			}
			
		}
		else if (DecimalOffset < 0 && digitIter >= 0)
		{
			Sum = a.digits[digitIter] + Carry;
		}
		else
		{
			Sum = Carry;
		}
		
		if (Sum >= 10)
		{
			Carry = 1;
			Sum -= 10;
		}
		else
		{
			Carry = 0;
		}

		if (digitIter >= 0)
		{
			result->digits[digitIter] = Sum;	
		}
		else
		{
			mpfloat_shift_right_insert_host(result, Sum, true);
		}

		digitIter--;
		CarryDigitsAdded++;
	}

	if (DecimalOffset > 0)
	{
		for (uint32_t LeftOverDigitIter = CarryDigitsAdded; LeftOverDigitIter < DecimalOffset; LeftOverDigitIter++)
		{
			mpfloat_shift_right_insert_host(result, b.digits[DecimalOffset - (LeftOverDigitIter + 1)], true);
		}
	}
	else if (DecimalOffset < 0)
	{
		uint32_t AbsOffset = DecimalOffset * -1;
		for (uint32_t LeftOverDigitIter = CarryDigitsAdded; LeftOverDigitIter < AbsOffset; LeftOverDigitIter++)
		{
			result->digits[LeftOverDigitIter] = a.digits[LeftOverDigitIter];
		}
	}
}

__device__ void mpfloat_set_mpfloat(mpfloat *a, mpfloat b)
{
	a->decimal = b.decimal;
	a->negative = b.negative;
	memcpy(a->digits, b.digits, mpfloat_precision * sizeof(uint8_t));
	//cudaMemcpy(a->digits, b.digits, mpfloat_precision * sizeof(uint8_t), cudaMemcpyHostToHost);
}

void mpfloat_set_mpfloat_host(mpfloat *a, mpfloat b)
{
	a->decimal = b.decimal;
	a->negative = b.negative;
	memcpy(a->digits, b.digits, mpfloat_precision * sizeof(uint8_t));
	//cudaMemcpy(a->digits, b.digits, mpfloat_precision * sizeof(uint8_t), cudaMemcpyHostToHost);
}

__device__ void mpfloat_to_double(mpfloat multifloat, double *result)
{
	*result = 0;
	uint64_t NumDigits;
	mpfloat_get_extent(multifloat, &NumDigits);
	double digitMagnitude = powf(10.0, multifloat.decimal - 1);
	int test = 0;
	for (uint32_t DigitIter = 0; DigitIter < NumDigits; DigitIter++)
	{
		*result += digitMagnitude * (double)(multifloat.digits[DigitIter]);
		digitMagnitude /= 10;
	}
	if (multifloat.negative)
	{
		*result *= -1;
	}
}

void mpfloat_set_mpir(mpfloat *multifloat, mpf_t mpirfloat)
{
	mp_exp_t exponent;
	char temp_buffer[200];
	mpf_get_str(temp_buffer, &exponent, 10, 90, mpirfloat);

	for (int charDigitIter = 0; charDigitIter < strlen(temp_buffer); charDigitIter++)
	{
		if (charDigitIter == 0 && temp_buffer[charDigitIter] == '-')
		{
			multifloat->negative = true;
		}
		else
		{
			multifloat->digits[charDigitIter - multifloat->negative] = temp_buffer[charDigitIter] - 48;
		}
	}

	multifloat->decimal = exponent;
}

void mpfloat_set_double_host(double a, mpfloat *result)
{
	char doubleString[128];
	mpfloat_clear_host(result);
	sprintf(doubleString, "%f", a);
	size_t stringLength = strlen(doubleString);
	uint32_t charIter;
	uint32_t digitIter = 0;
	bool gotDecimalPoint = false;
	bool gotNegative = false;

	for (charIter = 0; charIter < stringLength; charIter++)
	{
		if (doubleString[charIter] == '.')
		{
			if (a >= 1 || (a <= -1))
			{
				result->decimal = (charIter - gotNegative);
			}
			gotDecimalPoint = true;
		}
		else if (doubleString[charIter] == '-')
		{
			result->negative = true;
			gotNegative = true;
		}
		else if (digitIter == 0 && doubleString[charIter] == '0')
		{
			continue;
		}
		else
		{
			result->digits[digitIter++] = ((uint8_t)doubleString[charIter]) - 48;
		}

	}
}

__device__ void mpfloat_set_uint32(uint32_t a, mpfloat *result)
{
	mpfloat_clear(result);
	if (a == 0)
	{
		return;
	}

	uint32_t Reference = a;
	int InsertIndex = 0;
	uint32_t DivVal = 1000000000;
	bool FoundDigit = false;
	uint32_t digitValue;

	while (DivVal != 0)
	{
		digitValue = Reference / DivVal;
		if (digitValue != 0)
		{
			FoundDigit = true;
		}

		if (FoundDigit)
		{
			result->digits[InsertIndex++] = digitValue;
		}

		Reference %= DivVal;
		DivVal /= 10;
	}

	result->decimal = InsertIndex;
}

__device__ void mpfloat_get_extent(mpfloat a, uint64_t *extent)
{
	int decimalIter = 0;
	for (decimalIter = 0; decimalIter < mpfloat_precision; decimalIter++)
	{
		if (a.digits[(mpfloat_precision - 1) - decimalIter] != 0)
		{
			break;
		}

		if ((mpfloat_precision - 1) - decimalIter < a.decimal)
		{
			*extent = 0;
			return;
		}
	}

	*extent = (mpfloat_precision - decimalIter);
}

void mpfloat_get_extent_host(mpfloat a, uint64_t *extent)
{
	uint32_t decimalIter = 0;
	for (decimalIter = 0; decimalIter < mpfloat_precision; decimalIter++)
	{
		if (a.digits[(mpfloat_precision - 1) - decimalIter] != 0)
		{
			break;
		}

		if ((mpfloat_precision - 1) - decimalIter < a.decimal)
		{
			*extent = 0;
			return;
		}
	}

	*extent = (mpfloat_precision - decimalIter);
}

__device__ void mpfloat_to_str(mpfloat multifloat, char *buffer, uint32_t length)
{
	uint32_t CharIndexIter = 0;
	uint64_t decimalPlaces;
	mpfloat_get_extent(multifloat, &decimalPlaces);
	uint64_t printedDecimals = 0;
	bool GotDecimalPoint = false;

	if (multifloat.negative)
	{
		buffer[CharIndexIter++] = '-';
	}

	if (multifloat.decimal == 0)
	{
		buffer[CharIndexIter++] = '0';
	}

	for (int digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		if (digitIter == multifloat.decimal)
		{
			buffer[CharIndexIter++] = '.';
			GotDecimalPoint = true;
			if (CharIndexIter >= (length - 2))
			{
				break;
			}
		}

		buffer[CharIndexIter++] = multifloat.digits[digitIter] + 48;
		
		if (GotDecimalPoint)
		{
			printedDecimals++;
			if (printedDecimals >= decimalPlaces)
			{
				break;
			}
		}

		if (CharIndexIter >= (length - 2))
		{
			break;
		}
	}

	buffer[CharIndexIter] = '\0';
}

void mpfloat_to_str_host(mpfloat multifloat, char *buffer, uint32_t length)
{
	uint32_t CharIndexIter = 0;
	uint32_t decimalPlaces = mpfloat_get_num_decimals_host(multifloat);
	uint64_t printedDecimals = 0;
	bool GotDecimalPoint = false;

	if (multifloat.negative)
	{
		buffer[CharIndexIter++] = '-';
	}

	if (multifloat.decimal == 0)
	{
		buffer[CharIndexIter++] = '0';

		if (decimalPlaces == 0)
		{
			buffer[CharIndexIter] = '\0';
			return;
		}
	}

	for (int digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		if (digitIter == multifloat.decimal)
		{
			buffer[CharIndexIter++] = '.';
			GotDecimalPoint = true;
			if (CharIndexIter >= (length - 2))
			{
				break;
			}
		}

		buffer[CharIndexIter++] = multifloat.digits[digitIter] + 48;

		if (GotDecimalPoint)
		{
			printedDecimals++;
			if (printedDecimals >= decimalPlaces)
			{
				break;
			}
		}

		if (CharIndexIter >= (length - 2))
		{
			break;
		}
	}

	buffer[CharIndexIter] = '\0';
}

void print_mpfloat(mpfloat multifloat)
{
	uint64_t decimalPlaces;
	mpfloat_get_extent_host(multifloat, &decimalPlaces);
	uint64_t printedDecimals = 0;

	if (multifloat.negative)
	{
		fprintf(stdout, "-");
	}

	for (int digitIter = 0; digitIter < mpfloat_precision; digitIter++)
	{
		if (digitIter == multifloat.decimal)
		{
			fprintf(stdout, ".%d", multifloat.digits[digitIter]);
			printedDecimals++;
		}
		else
		{
			fprintf(stdout, "%d", multifloat.digits[digitIter]);
		}

		if (printedDecimals)
		{
			printedDecimals++;
			if (printedDecimals >= decimalPlaces)
			{
				break;
			}
		}

	}
}



#define DEBUG_RETURN return;

#define MEAN 0
#define MODE 1
#define MEDIAN 2

__device__ 	void getBestIteration(uint32_t *bestIteration, double *bestMagnitude, uint32_t *IterationSamples, double *MagnitudeSamples, uint32_t NumSamples, uint32_t statistic)
{

	switch (statistic)
	{
	case MODE:

		break;
	}
}

__global__ void doFractal(mpfloat leftX, mpfloat topI, mpfloat pixelCoordinateDelta, mpfloat superSampleCoordinateDelta, int frameWidth, int MaxIterations, int SampleRate, uint32_t *iterations, double *magnitudes, uint32_t Size, int precision)
{
	char *msg = new char[500];
	if (mpfloat_precision == 0)
	{
		mpfloat_precision = precision;
	}

	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if (idx >= Size)
	{
		return;
	}

	{

		mpfloat result;
		mpfloat_init(&result);
		mpfloat_to_str(leftX, msg, 498);
		printf("A: %s\n", msg);
		printf("A.decimal: %d\n", leftX.decimal);
		mpfloat_to_str(topI, msg, 498);
		printf("B: %s\n", msg);
		printf("B[0]: %u\n", topI.digits[0]);
		printf("B.decimal: %d\n", topI.decimal);
		mpfloat_add(topI, leftX, &result);
		mpfloat_to_str(result, msg, 498);
		printf("result: %s\n", msg);
	}
	DEBUG_RETURN

	mpfloat CurrentX;
	mpfloat_init(&CurrentX);
	mpfloat CurrentI;
	mpfloat_init(&CurrentI);
	mpfloat SuperI;
	mpfloat_init(&SuperI);
	mpfloat SuperX;
	mpfloat_init(&SuperX);
	mpfloat cx;
	mpfloat_init(&cx);
	mpfloat ci;
	mpfloat_init(&ci);
	mpfloat zx;
	mpfloat_init(&zx);
	mpfloat zi;
	mpfloat_init(&zi);
	mpfloat Magnitude;
	mpfloat_init(&Magnitude);
	mpfloat zx_squared;
	mpfloat_init(&zx_squared);
	mpfloat zi_squared;
	mpfloat_init(&zi_squared);
	mpfloat tempzx;
	mpfloat_init(&tempzx);
	mpfloat tempzi;
	mpfloat_init(&tempzi);
	mpfloat RowPixelOffset;
	mpfloat_init(&RowPixelOffset);
	mpfloat tempfloat;
	mpfloat_init(&tempfloat);

	double MinIterationMagnitude = 0.0;
	int CellsCompleted = 0;
	int MinIteration = 0;
	int CurrentIterations = 0;
	bool PreviouslyPaused = false;
	bool DoFullCalculation = true;

	uint32_t *IterationSamples = new uint32_t[SampleRate*SampleRate];
	double *MagnitudeSamples = new double[SampleRate*SampleRate];

	uint32_t Row = idx / frameWidth;
	// Current I = topI - (rows * pixelcoordinatedelta)
	mpfloat_mul_ui(pixelCoordinateDelta, Row, &CurrentI);
	mpfloat_sub(topI, CurrentI, &CurrentI);

	

	uint32_t superSamplesCalculated = 0;
	double CurrentMagnitude;

	mpfloat_set_mpfloat(&SuperI, CurrentI);


	mpfloat_set_mpfloat(&CurrentX, leftX);

	for (int SuperRow = 0; SuperRow < SampleRate; SuperRow++)
	{
		mpfloat_set_mpfloat(&SuperX, CurrentX);

		for (int SuperCol = 0; SuperCol < SampleRate; SuperCol++)
		{
			mpfloat_set_mpfloat(&cx, SuperX);
			mpfloat_set_mpfloat(&ci, SuperI);
			mpfloat_set_mpfloat(&zx, SuperX);
			mpfloat_set_mpfloat(&zi, SuperI);
			mpfloat_clear(&Magnitude);

			for (CurrentIterations = 0; CurrentIterations < MaxIterations; CurrentIterations++)
			{
				mpfloat_mul(zx, zx, &zx_squared);
				mpfloat_to_str(zx, msg, 499);
				printf("zx is: %s\n", msg);
				mpfloat_to_str(zx_squared, msg, 499);
				printf("zx_squared is: %s\n", msg);
				DEBUG_RETURN
				mpfloat_mul(zi, zi, &zi_squared);
				mpfloat_sub(zx_squared, zi_squared, &tempzx);
				mpfloat_sub(zx, zi, &tempzi);
				mpfloat_mul_ui(tempzi, 2, &tempfloat);

				mpfloat_add(tempzx, cx, &zx);
				mpfloat_add(tempfloat, ci, &zi);

				mpfloat_mul(zx, zx, &zx_squared);
				mpfloat_mul(zi, zi, &zi_squared);

				mpfloat_add(zx_squared, zi_squared, &Magnitude);

				if (Magnitude.decimal > 0)
				{
					if (Magnitude.decimal > 1 || Magnitude.digits[0] >= 4)
					{
						break;
					}
				}
			}
			mpfloat_to_double(Magnitude, &CurrentMagnitude);

			IterationSamples[superSamplesCalculated] = CurrentIterations;
			MagnitudeSamples[superSamplesCalculated] = CurrentMagnitude;

			mpfloat_add(SuperX, superSampleCoordinateDelta, &tempfloat);
			mpfloat_set_mpfloat(&SuperX, tempfloat);
		}
		mpfloat_sub(SuperI, superSampleCoordinateDelta, &tempfloat);
		mpfloat_set_mpfloat(&SuperI, tempfloat);

	}


	iterations[idx] = CurrentIterations;
	magnitudes[idx] = CurrentMagnitude;

	//getBestIteration(&iterations[idx], &magnitudes[idx], IterationSamples, MagnitudeSamples, (SampleRate*SampleRate), MODE);
	delete IterationSamples;
	delete MagnitudeSamples;
	mpfloat_free(CurrentX);
	mpfloat_free(SuperI);
	mpfloat_free(SuperX);
	mpfloat_free(CurrentI);
	mpfloat_free(cx);
	mpfloat_free(ci);
	mpfloat_free(zx);
	mpfloat_free(zi);
	mpfloat_free(Magnitude);
	mpfloat_free(zx_squared);
	mpfloat_free(zi_squared);
	mpfloat_free(tempzx);
	mpfloat_free(RowPixelOffset);
	mpfloat_free(tempfloat);
}
#if 0
void doFractal_host(mpfloat leftX, mpfloat topI, mpfloat pixelCoordinateDelta, mpfloat superSampleCoordinateDelta, int frameWidth, int MaxIterations, int SampleRate, uint32_t *iterations, double *magnitudes, uint32_t Size, int precision)
{
	if (mpfloat_precision == 0)
	{
		mpfloat_precision = precision;
	}

	int idx = threadIdx.x + blockIdx.x * blockDim.x;
	if (idx >= Size)
	{
		return;
	}

	mpfloat CurrentX;
	mpfloat_init(&CurrentX);
	mpfloat CurrentI;
	mpfloat_init(&CurrentI);
	mpfloat SuperI;
	mpfloat_init(&SuperI);
	mpfloat SuperX;
	mpfloat_init(&SuperX);
	mpfloat cx;
	mpfloat_init(&cx);
	mpfloat ci;
	mpfloat_init(&ci);
	mpfloat zx;
	mpfloat_init(&zx);
	mpfloat zi;
	mpfloat_init(&zi);
	mpfloat Magnitude;
	mpfloat_init(&Magnitude);
	mpfloat zx_squared;
	mpfloat_init(&zx_squared);
	mpfloat zi_squared;
	mpfloat_init(&zi_squared);
	mpfloat tempzx;
	mpfloat_init(&tempzx);
	mpfloat tempzi;
	mpfloat_init(&tempzi);
	mpfloat RowPixelOffset;
	mpfloat_init(&RowPixelOffset);
	mpfloat tempfloat;
	mpfloat_init(&tempfloat);

	double MinIterationMagnitude = 0.0;
	int CellsCompleted = 0;
	int MinIteration = 0;
	int CurrentIterations = 0;
	bool PreviouslyPaused = false;
	bool DoFullCalculation = true;

	uint32_t *IterationSamples = new uint32_t[SampleRate*SampleRate];
	double *MagnitudeSamples = new double[SampleRate*SampleRate];

	uint32_t Row = idx / frameWidth;
	// Current I = topI - (rows * pixelcoordinatedelta)
	mpfloat_mul_ui(pixelCoordinateDelta, Row, &CurrentI);
	mpfloat_sub(topI, CurrentI, &CurrentI);



	uint32_t superSamplesCalculated = 0;
	double CurrentMagnitude;

	mpfloat_set_mpfloat(&SuperI, CurrentI);


	mpfloat_set_mpfloat(&CurrentX, leftX);

	for (int SuperRow = 0; SuperRow < SampleRate; SuperRow++)
	{
		mpfloat_set_mpfloat(&SuperX, CurrentX);

		for (int SuperCol = 0; SuperCol < SampleRate; SuperCol++)
		{
			mpfloat_set_mpfloat(&cx, SuperX);
			mpfloat_set_mpfloat(&ci, SuperI);
			mpfloat_set_mpfloat(&zx, SuperX);
			mpfloat_set_mpfloat(&zi, SuperI);
			mpfloat_clear(&Magnitude);

			for (CurrentIterations = 0; CurrentIterations < MaxIterations; CurrentIterations++)
			{
				mpfloat_mul(zx, zx, &zx_squared);

				DEBUG_RETURN
				mpfloat_mul(zi, zi, &zi_squared);
				mpfloat_sub(zx_squared, zi_squared, &tempzx);
				mpfloat_sub(zx, zi, &tempzi);
				mpfloat_mul_ui(tempzi, 2, &tempfloat);

				mpfloat_add(tempzx, cx, &zx);
				mpfloat_add(tempfloat, ci, &zi);

				mpfloat_mul(zx, zx, &zx_squared);
				mpfloat_mul(zi, zi, &zi_squared);

				mpfloat_add(zx_squared, zi_squared, &Magnitude);

				if (Magnitude.decimal > 0)
				{
					if (Magnitude.decimal > 1 || Magnitude.digits[0] >= 4)
					{
						break;
					}
				}
			}
			mpfloat_to_double(Magnitude, &CurrentMagnitude);

			IterationSamples[superSamplesCalculated] = CurrentIterations;
			MagnitudeSamples[superSamplesCalculated] = CurrentMagnitude;

			mpfloat_add(SuperX, superSampleCoordinateDelta, &tempfloat);
			mpfloat_set_mpfloat(&SuperX, tempfloat);
		}
		mpfloat_sub(SuperI, superSampleCoordinateDelta, &tempfloat);
		mpfloat_set_mpfloat(&SuperI, tempfloat);

	}


	iterations[idx] = CurrentIterations;
	magnitudes[idx] = CurrentMagnitude;

	//getBestIteration(&iterations[idx], &magnitudes[idx], IterationSamples, MagnitudeSamples, (SampleRate*SampleRate), MODE);
	delete IterationSamples;
	delete MagnitudeSamples;
	mpfloat_free(CurrentX);
	mpfloat_free(SuperI);
	mpfloat_free(SuperX);
	mpfloat_free(CurrentI);
	mpfloat_free(cx);
	mpfloat_free(ci);
	mpfloat_free(zx);
	mpfloat_free(zi);
	mpfloat_free(Magnitude);
	mpfloat_free(zx_squared);
	mpfloat_free(zi_squared);
	mpfloat_free(tempzx);
	mpfloat_free(RowPixelOffset);
	mpfloat_free(tempfloat);
}
#endif

__device__ unsigned int dev_gx;
__device__ unsigned int dev_bx;


__global__ void cudaTesting()
{
	if (threadIdx.x == 0)
	{
		dev_gx = gridDim.x;
		dev_bx = blockDim.x;
	}
}
void CUDAFractalWorker(mpf_t leftX, mpf_t topI, mpf_t pixelCoordinateDelta, mpf_t SuperSampleCoordinateDelta, int frameWidth, int frameHeight, int MaxIterations, int SampleRate, int Precision, uint32_t *HostIterations, double *HostMagnitudes)
{
	cudaError_t cudaStatus;

	cudaStatus = cudaSetDevice(0);
	if (cudaStatus != cudaSuccess) {
		fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
		return;
	}

	mpfloat_set_precision(Precision);

#if 0
	char buff[100];
	mpfloat multifloattest;
	mpfloat_init_host(&multifloattest);
	cudaCheckError();
	mpfloat_set_double(1.325, &multifloattest);
	double test;
	mpfloat_to_double(multifloattest, &test);
	fprintf(stdout, "%f\n\n", test);
	//mpfloat_to_str(multifloattest, buff, 100);
	mpfloat_free_host(multifloattest);
	return;
#endif
#if 1
	int blockSize;   // The launch configurator returned block size 
	int minGridSize; // The minimum grid size needed to achieve the maximum occupancy for a full device launch 
	int gridSize;    // The actual grid size needed, based on input size 
	int arrayCount = frameWidth*frameHeight;
	cudaOccupancyMaxPotentialBlockSize(&minGridSize, &blockSize, doFractal, 0, 0);
	// Round up according to array size 
	gridSize = (arrayCount + blockSize - 1) / blockSize;
	cudaCheckError();
	if (cudaStatus != cudaSuccess)
	{
		fprintf(stdout, "Cuda Error: %d --> %s\n", cudaGetLastError(), cudaGetErrorString(cudaStatus));
		return;
	}

	fprintf(stdout, "GridSize: %d BlockSize: %d\n", gridSize, blockSize);




	mpfloat multifloat_leftX;
	mpfloat multifloat_topI;
	mpfloat multifloat_pixelDelta;
	mpfloat multifloat_superPixelDelta;
	mpfloat_init_host(&multifloat_leftX);
	mpfloat_set_mpir(&multifloat_leftX, leftX);
	mpfloat_init_host(&multifloat_topI);
	mpfloat_set_mpir(&multifloat_topI, topI);
	mpfloat_init_host(&multifloat_pixelDelta);
	mpfloat_set_mpir(&multifloat_pixelDelta, pixelCoordinateDelta);
	mpfloat_init_host(&multifloat_superPixelDelta);
	mpfloat_set_mpir(&multifloat_superPixelDelta, SuperSampleCoordinateDelta);
	cudaCheckError();

	uint32_t *DeviceIterations;
	double *DeviceMagnitudes;
	cudaStatus = cudaMallocManaged(&DeviceIterations, arrayCount * sizeof(uint32_t));
	cudaCheckError();
	if (cudaStatus == cudaSuccess)
	{
		cudaStatus = cudaMallocManaged(&DeviceMagnitudes, arrayCount * sizeof(double));
		cudaCheckError();
	}

	//__global__ void doFractal(mpfloat leftX, mpfloat topI, mpfloat pixelCoordinateDelta, mpfloat superSampleCoordinateDelta, uint32_t *iterations, uint32_t Size)

	if (cudaStatus == cudaSuccess)
	{
		//mpfloat_test_mul_host();
		unsigned int *gx;
		unsigned int *bx;
		cudaStatus = cudaMallocManaged(gx, sizeof(unsigned int));
		cudaTesting << <gridSize, blockSize >> > ();

		

		//doFractal << <gridSize, blockSize >> > (multifloat_leftX, multifloat_topI, multifloat_pixelDelta, multifloat_superPixelDelta, frameWidth, MaxIterations, SampleRate, DeviceIterations, DeviceMagnitudes, arrayCount, Precision);

		//doFractal << <1, 1 >> > (multifloat_leftX, multifloat_topI, multifloat_pixelDelta, multifloat_superPixelDelta, frameWidth, MaxIterations, SampleRate, DeviceIterations, DeviceMagnitudes, arrayCount, Precision);

	}
	if (cudaStatus == cudaSuccess)
	{
		cudaStatus = cudaDeviceSynchronize();
		cudaCheckError();
	}

	if (cudaStatus == cudaSuccess)
	{
		unsigned int hst_gx;
		cudaMemcpyFromSymbol(&hst_gx, &dev_gx, sizeof(hst_gx), 0, cudaMemcpyDeviceToHost);
		fprintf(stdout, "Gridx: %d\n", hst_gx);

		cudaStatus = cudaMemcpy(HostIterations, DeviceIterations, arrayCount * sizeof(uint32_t), cudaMemcpyDeviceToHost);
		cudaCheckError();
		fprintf(stdout, "iter: %d\n", HostIterations[0]);
	}
	if (cudaStatus == cudaSuccess)
	{
		cudaStatus = cudaMemcpy(HostMagnitudes, DeviceMagnitudes, arrayCount * sizeof(double), cudaMemcpyDeviceToHost);
		cudaCheckError();
	}

	cudaFree(DeviceIterations);
	cudaFree(DeviceMagnitudes);
	mpfloat_free_host(multifloat_leftX);
	mpfloat_free_host(multifloat_topI);
	mpfloat_free_host(multifloat_superPixelDelta);
	mpfloat_free_host(multifloat_pixelDelta);
#endif
	return;
}
