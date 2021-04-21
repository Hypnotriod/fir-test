
/* 
 * File:   WavFileWriter.h
 * Author: Ilya Pikin
 */

#ifndef WAVFILEWRITER_H
#define WAVFILEWRITER_H

#include <stdio.h>
#include <cstdint>
#include <cstdlib>

#include "WavFileHeader.h"

class WavFileWriter {
public:
    WavFileWriter();
    virtual ~WavFileWriter();

    enum Status {
        OK = 0,
        OPEN_ERROR,
        WRITE_ERROR,
    };

    Status open(const char * path);
    void close();
    Status writeHeader(WavFileHeader & header);
    Status write(size_t samplesNum, float * buffer);
private:
    FILE * file;
    uint16_t audioFormat = 0;
    uint16_t bitsPerSample = 0;
};

#endif /* WAVFILEWRITER_H */

