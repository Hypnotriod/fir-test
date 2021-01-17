
/* 
 * File:   WavFileReader.cpp
 * Author: Ilya Pikin
 */

#include <cstdio>
#include <cstring>

#include "WavFileReader.h"

#define read16(_src, _index) (_src[(_index)] | (_src[(_index)+1] << 8))
#define read24(_src, _index) (_src[(_index)] | (_src[(_index)+1] << 8) \
               | (_src[(_index)+2] << 16))
#define read32(_src, _index) (_src[(_index)] | (_src[(_index)+1] << 8) \
               | (_src[(_index)+2] << 16) | (_src[(_index)+3] << 24))

#define INT24_MAX 8388607

WavFileReader::WavFileReader() {
}

WavFileReader::~WavFileReader() {
    close();
}

WavFileHeader * WavFileReader::getHeader() {
    return &header;
}

WavFileReader::Status WavFileReader::open(const char * path) {
    file = fopen(path, "rb");
    if (file == NULL)
        return OPEN_ERROR;
    return parseHeader();
}

void WavFileReader::close() {
    if (file != NULL) {
        fclose(file);
        delete file;
    }
}

WavFileReader::Status WavFileReader::read(size_t samplesNum, float * buffer, size_t * samplesRead) {
    size_t samplesLeft = getSamplesLeft();
    size_t samplesToRead = samplesLeft > samplesNum ? samplesNum : samplesLeft;
    uint8_t * ioBuffer = new uint8_t[(header.bitsPerSample / 8) * samplesNum];
    int32_t sample;
    Status status = OK;

    *samplesRead = fread(ioBuffer, (header.bitsPerSample / 8), samplesToRead, file);

    if (*samplesRead == samplesLeft) {
        status = END_OF_FILE;
    } else if (*samplesRead != samplesNum) {
        if (feof(file)) status = END_OF_FILE;
        else status = READ_ERROR;
    }

    if (status != READ_ERROR) {
        if (header.bitsPerSample == 16) {
            for (size_t i = 0; i < *samplesRead; i++) {
                sample = read16(ioBuffer, i * 2);
                if (sample & 0x8000)
                    sample = sample | 0xFFFF0000;
                buffer[i] = (float) sample / (float) INT16_MAX;
            }
        } else if (header.bitsPerSample == 24) {
            for (size_t i = 0; i < *samplesRead; i++) {
                sample = read24(ioBuffer, i * 3);
                if (sample & 0x800000)
                    sample = sample | 0xFF000000;
                buffer[i] = (float) sample / (float) INT24_MAX;
            }
        }
    }

    delete[] ioBuffer;
    return status;
}

WavFileReader::Status WavFileReader::parseHeader() {
    uint8_t headerSource[WAV_FILE_HEADER_SIZE];
    size_t bytesRead = fread(headerSource, sizeof (uint8_t), WAV_FILE_HEADER_SIZE, file);
    if (bytesRead != WAV_FILE_HEADER_SIZE) return PARSE_ERROR;
    fseek(file, WAV_FILE_HEADER_SIZE, SEEK_SET);

    memcpy(header.chunkId, &headerSource[0], 4);
    header.chunkSize = read32(headerSource, 4);
    memcpy(header.format, &headerSource[8], 4);
    memcpy(header.subchunk1Id, &headerSource[12], 4);
    header.subchunk1Size = read32(headerSource, 16);
    header.audioFormat = read16(headerSource, 20);
    header.numChannels = read16(headerSource, 22);
    header.sampleRate = read32(headerSource, 24);
    header.byteRate = read32(headerSource, 28);
    header.blockAlign = read16(headerSource, 32);
    header.bitsPerSample = read16(headerSource, 34);
    memcpy(header.subchunk2Id, &headerSource[36], 4);
    header.subchunk2Size = read32(headerSource, 40);

    return OK;
}

size_t WavFileReader::getSamplesRead() {
    return (ftell(file) - WAV_FILE_HEADER_SIZE) / sizeof (int16_t);
}

size_t WavFileReader::getSamplesLeft() {
    return header.subchunk2Size / (header.bitsPerSample / 8) - getSamplesRead();
}
