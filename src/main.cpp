/**
 * @file      main.cpp
 * @brief     Stream compaction test program
 * @authors   Kai Ninomiya
 * @date      2015
 * @copyright University of Pennsylvania
 */

#include <cstdio>
#include <stream_compaction/cpu.h>
#include <stream_compaction/naive.h>
#include <stream_compaction/efficient.h>
#include <stream_compaction/thrust.h>
#include <stream_compaction/common.h>
#include <stream_compaction/radixsort.h>
#include "testing_helpers.hpp"
#include <algorithm>


static StreamCompaction::Common::Timer timer;
const int CPU_RUNTIMES = 5;

int main(int argc, char* argv[]) {
    const int SIZE = 1 << 20;
    const int NPOT = SIZE - 3;
    //int a[SIZE], b[SIZE], c[SIZE];
	int* a = new int[SIZE];
	int* b = new int[SIZE];
	int* c = new int[SIZE];


	// print parameters setting
	printf("\n");
	printf("**********************\n");
	printf("**  TEST PARAMETERS **\n");
	printf("**********************\n");
	printf("	Array size = %d \n", SIZE);
	printf("	BlockSize  = %d \n", StreamCompaction::Common::BlockSize);
	printf("\n");

    // Scan tests
	
    printf("\n");
    printf("****************\n");
    printf("** SCAN TESTS **\n");
    printf("****************\n");

    genArray(SIZE - 1, a, 50);  // Leave a 0 at the end to test that edge case
    a[SIZE - 1] = 0;
    printArray(SIZE, a, true);

    zeroArray(SIZE, b);
    printDesc("cpu scan, power-of-two");
	timer.startCpuTimer();
	for (int i = 0; i < CPU_RUNTIMES;++i)
	    StreamCompaction::CPU::scan(SIZE, b, a);
	timer.stopCpuTimer();
	timer.printTimerInfo("Scan::CPU = ", timer.getCpuElapsedTime() / float(CPU_RUNTIMES));
    printArray(SIZE, b, true);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("cpu scan, non-power-of-two");
	timer.startCpuTimer();
	for (int i = 0; i < CPU_RUNTIMES; ++i)
		StreamCompaction::CPU::scan(NPOT, c, a);
	timer.stopCpuTimer();
	timer.printTimerInfo("Scan::CPU = ", timer.getCpuElapsedTime() / float(CPU_RUNTIMES));
    printArray(NPOT, b, true);
    printCmpResult(NPOT, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("naive scan, power-of-two");
    StreamCompaction::Naive::scan(SIZE, c, a);
    //printArray(SIZE, c, true);
    printCmpResult(SIZE, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("naive scan, non-power-of-two");
    StreamCompaction::Naive::scan(NPOT, c, a);
    //printArray(SIZE, c, true);
    printCmpResult(NPOT, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("work-efficient scan, power-of-two");
    StreamCompaction::Efficient::scan(SIZE, c, a);
    //printArray(SIZE, c, true);
    printCmpResult(SIZE, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("work-efficient scan, non-power-of-two");
    StreamCompaction::Efficient::scan(NPOT, c, a);
    //printArray(NPOT, c, true);
    printCmpResult(NPOT, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("thrust scan, power-of-two");
    StreamCompaction::Thrust::scan(SIZE, c, a);
    //printArray(SIZE, c, true);
    printCmpResult(SIZE, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("thrust scan, non-power-of-two");
    StreamCompaction::Thrust::scan(NPOT, c, a);
    //printArray(NPOT, c, true);
    printCmpResult(NPOT, b, c);
	printf("\n");

	
    printf("\n");
    printf("*****************************\n");
    printf("** STREAM COMPACTION TESTS **\n");
    printf("*****************************\n");

    // Compaction tests

    genArray(SIZE - 1, a, 4);  // Leave a 0 at the end to test that edge case
    a[SIZE - 1] = 0;
    printArray(SIZE, a, true);

    int count, expectedCount, expectedNPOT;

    zeroArray(SIZE, b);
    printDesc("cpu compact without scan, power-of-two");
	timer.startCpuTimer();
	for (int i = 0; i < CPU_RUNTIMES; ++i)
		count = StreamCompaction::CPU::compactWithoutScan(SIZE, b, a);
	timer.stopCpuTimer();
	timer.printTimerInfo("Compact::CPU::WithoutScan = ", timer.getCpuElapsedTime() / float(CPU_RUNTIMES));
    expectedCount = count;
    printArray(count, b, true);
    printCmpLenResult(count, expectedCount, b, b);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("cpu compact without scan, non-power-of-two");
	timer.startCpuTimer();
	for (int i = 0; i < CPU_RUNTIMES; ++i)
		count = StreamCompaction::CPU::compactWithoutScan(NPOT, c, a);
	timer.stopCpuTimer();
	timer.printTimerInfo("Compact::CPU::WithoutScan = ", timer.getCpuElapsedTime() / float(CPU_RUNTIMES));
    expectedNPOT = count;
    printArray(count, c, true);
    printCmpLenResult(count, expectedNPOT, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("cpu compact with scan");
	timer.startCpuTimer();
	for (int i = 0; i < CPU_RUNTIMES; ++i)
		count = StreamCompaction::CPU::compactWithScan(SIZE, c, a);
	timer.stopCpuTimer();
	timer.printTimerInfo("Compact::CPU::WithScan = ", timer.getCpuElapsedTime() / float(CPU_RUNTIMES));
    printArray(count, c, true);
    printCmpLenResult(count, expectedCount, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("work-efficient compact, power-of-two");
    count = StreamCompaction::Efficient::compact(SIZE, c, a);
    //printArray(count, c, true);
    printCmpLenResult(count, expectedCount, b, c);
	printf("\n");


    zeroArray(SIZE, c);
    printDesc("work-efficient compact, non-power-of-two");
    count = StreamCompaction::Efficient::compact(NPOT, c, a);
    //printArray(count, c, true);
    printCmpLenResult(count, expectedNPOT, b, c);
	printf("\n");
	

	printf("\n");
	printf("*****************************\n");
	printf("****** RADIX SORT TESTS *****\n");
	printf("*****************************\n");

	genArray(SIZE, a, SIZE);  
	printArray(SIZE, a, true);
	memcpy(b, a, SIZE*sizeof(int));

	printDesc("std::sort");
	timer.startCpuTimer();
	std::sort(a, a + SIZE);
	timer.stopCpuTimer();
	timer.printTimerInfo("CPU::std::sort = ", timer.getCpuElapsedTime());
	printArray(SIZE, a, true);
	printf("\n");

	printDesc("GPU::RadixSort");
	StreamCompaction::RadixSort::RadixSort(SIZE, b, SIZE);
	printCmpResult(SIZE, b, a);
	printf("\n");

	//free a,b,c
	delete[] a;
	delete[] b;
	delete[] c;
}
