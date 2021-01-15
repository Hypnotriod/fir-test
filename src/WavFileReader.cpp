
/* 
 * File:   WavFileReader.cpp
 * Author: Ilya Pikin
 */

#include <cstdio>
#include <cstring>

#include "WavFileReader.h"

#define parseShort(_src, _index) (_src[_index] | (_src[_index+1] << 8))
#define parseLong(_src, _index) (_src[_index] | (_src[_index+1] << 8) \
               | (_src[_index+2] << 16) | (_src[_index+3] << 24))

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

WavFileReader::Status WavFileReader::read(size_t samplesNum, int16_t * buffer, size_t * samplesRead) {
    size_t samplesLeft = getSamplesLeft();
    size_t samplesToRead = samplesLeft > samplesNum ? samplesNum : samplesLeft;
    *samplesRead = fread(buffer, sizeof (int16_t), samplesToRead, file);
    if (samplesLeft == *samplesRead) {
        return END_OF_FILE;
    } else if (*samplesRead != samplesNum) {
        if (feof(file)) return END_OF_FILE;
        else return READ_ERROR;
    }
    return OK;
}

WavFileReader::Status WavFileReader::parseHeader() {
    uint8_t headerSource[WAV_FILE_HEADER_SIZE];
    size_t bytesRead = fread(headerSource, sizeof (uint8_t), WAV_FILE_HEADER_SIZE, file);
    if (bytesRead != WAV_FILE_HEADER_SIZE) return PARSE_ERROR;
    fseek(file, WAV_FILE_HEADER_SIZE, SEEK_SET);

    memcpy(header.chunkId, &headerSource[0], 4);
    header.chunkSize = parseLong(headerSource, 4);
    memcpy(header.format, &headerSource[8], 4);
    memcpy(header.subchunk1Id, &headerSource[12], 4);
    header.subchunk1Size = parseLong(headerSource, 16);
    header.audioFormat = parseShort(headerSource, 20);
    header.numChannels = parseShort(headerSource, 22);
    header.sampleRate = parseLong(headerSource, 24);
    header.byteRate = parseLong(headerSource, 28);
    header.blockAlign = parseShort(headerSource, 32);
    header.bitsPerSample = parseShort(headerSource, 34);
    memcpy(header.subchunk2Id, &headerSource[36], 4);
    header.subchunk2Size = parseLong(headerSource, 40);

    return OK;
}

size_t WavFileReader::getSamplesRead() {
    return (ftell(file) - WAV_FILE_HEADER_SIZE) / sizeof (int16_t);
}

size_t WavFileReader::getSamplesLeft() {
    return header.subchunk2Size / (header.bitsPerSample / 8) - getSamplesRead();
}
