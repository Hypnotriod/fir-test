
/* 
 * File:   ImpulseReader.h
 * Author: Ilya Pikin
 */

#ifndef IMPULSEREADER_H
#define IMPULSEREADER_H

#include <cstdint>
#include <cstdlib>

struct WavFileHeader {
    char chunkId[4]; // "RIFF"
    uint32_t chunkSize;
    char format[4]; // "WAVE"
    char subchunk1Id[4]; // "fmt "
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    char subchunk2Id[4]; // "data"
    uint32_t subchunk2Size;
};

class WavFileReader {
public:
    WavFileReader(const char * path);
    virtual ~WavFileReader();

    static const size_t HEADER_SIZE = 44;

    enum Status {
        OK = 0,
        READ_ERROR = 1,
        PARSE_ERROR = 2,
        END_OF_FILE = 3,
    };

    Status open();
    Status read(size_t samplesNum, int16_t * pBuff, size_t * samplesRead);
    size_t getSamplesRead();
    size_t getSamplesLeft();
    uint8_t * getHeaderSource();

private:
    WavFileHeader header;
    uint8_t headerSource[HEADER_SIZE];
    FILE * file;
    size_t dataSize;
    const char * path;

    Status parseHeader();
};

#endif /* IMPULSEREADER_H */

