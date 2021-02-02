/* 
 * File:   FirFilter.h
 * Author: Ilya Pikin
 */

#ifndef FIRFILTER_H
#define FIRFILTER_H

#include <cstdio>

class FirFilter {
public:
    FirFilter(size_t maxTapsNum);
    virtual ~FirFilter();

    const size_t IO_SAMPLE_BUFF_SIZE = 4096;
    const float GAIN_ADJUST_FREQUENCY = 300.f;

    enum Status {
        OK = 0,
        ERROR = 1,
    };

    Status readImpulse(const char * path);
    void adjustGain();
    Status process(const char * srcPath, const char * destPath);
private:
    uint32_t sampleRate;
    const size_t maxTapsNum;
    size_t tapsNum;
    float gain = 1.f;
    float * impulse;
    float * buffer;
};

#endif /* FIRFILTER_H */

