/* 
 * File:   main.cpp
 * Author: Ilya Pikin
 */

#include <cstdlib>
#include <iostream>
#include <iomanip>

#include "WavFileReader.h"
#include "FirFilter.h"

#define TAPS_NUM 22000L
#define GAIN 0.35F

using namespace std;

int main(int argc, char** argv) {
    FirFilter filter(TAPS_NUM, GAIN);
    FirFilter::Status status;
    clock_t start, end;

    cout << "Computation started." << endl << flush;

    status = filter.readImpulse("data/impulse.wav");
    if (status != FirFilter::OK) {
        cout << "Read Impulse Error! " << endl;
        return -1;
    }

    start = clock();
    status = filter.process("data/input.wav", "data/output.wav");
    if (status != FirFilter::OK) {
        cout << "Filter Process Error!" << endl;
        return -1;
    }
    end = clock();

    double timeTaken = double(end - start) / double(CLOCKS_PER_SEC);

    cout << "Done in " << fixed << setprecision(3) << timeTaken << " sec." << endl;
    return 0;
}

