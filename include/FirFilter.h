/* 
 * File:   FirFilter.h
 * Author: Ilya Pikin
 */

#ifndef FIRFILTER_H
#define FIRFILTER_H

#include <cstdio>

class FirFilter {
public:
    FirFilter(size_t tapsNum, float gain);
    virtual ~FirFilter();

    const size_t IO_SAMPLE_BUFF_SIZE = 4096;

    enum Status {
        OK = 0,
        ERROR = 1,
    };

    Status readImpulse(const char * path);
    Status process(const char * srcPath, const char * destPath);
private:
    float gain;
    size_t tapsNum;
    float * impulse;
    float * buffer;
};

#endif /* FIRFILTER_H */

