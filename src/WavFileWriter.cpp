
/* 
 * File:   WavFileWriter.h
 * Author: Ilya Pikin
 */

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "WavFileWriter.h"

#define write16(_src, _buff, _index) _buff[(_index)] = (_src & 0xFF); \
                                     _buff[(_index)+1] = ((_src >> 8) & 0xFF)
#define write24(_src, _buff, _index) _buff[(_index)] = (_src & 0xFF); \
                                     _buff[(_index)+1] = ((_src >> 8) & 0xFF); \
                                     _buff[(_index)+2] = ((_src >> 16) & 0xFF)
#define write32(_src, _buff, _index) _buff[(_index)] = (_src & 0xFF); \
                                    _buff[(_index)+1] = ((_src >> 8) & 0xFF); \
                                    _buff[(_index)+2] = ((_src >> 16) & 0xFF); \
                                    _buff[(_index)+3] = ((_src >> 24) & 0xFF)

#define INT24_MAX 8388607

WavFileWriter::WavFileWriter() {
}

WavFileWriter::~WavFileWriter() {
    close();
}

WavFileWriter::Status WavFileWriter::open(const char * path) {
    file = fopen(path, "wb");
    if (file == NULL)
        return OPEN_ERROR;
    return OK;
}

void WavFileWriter::close() {
    if (file != NULL) {
        fclose(file);
        delete file;
    }
}

WavFileWriter::Status WavFileWriter::writeHeader(WavFileHeader & header) {
    uint8_t headerSource[WAV_FILE_HEADER_SIZE];

    bitsPerSample = header.bitsPerSample;

    size_t chunkSize = header.subchunk2Size + WAV_FILE_HEADER_SIZE - 8;

    memcpy(&headerSource[0], header.chunkId, 4);
    write32(chunkSize, headerSource, 4);
    memcpy(&headerSource[8], header.format, 4);
    memcpy(&headerSource[12], header.subchunk1Id, 4);
    write32(header.subchunk1Size, headerSource, 16);
    write16(header.audioFormat, headerSource, 20);
    write16(header.numChannels, headerSource, 22);
    write32(header.sampleRate, headerSource, 24);
    write32(header.byteRate, headerSource, 28);
    write16(header.blockAlign, headerSource, 32);
    write16(header.bitsPerSample, headerSource, 34);
    memcpy(&headerSource[36], header.subchunk2Id, 4);
    write32(header.subchunk2Size, headerSource, 40);

    fwrite(headerSource, sizeof (uint8_t), WAV_FILE_HEADER_SIZE, file);

    return OK;
}

WavFileWriter::Status WavFileWriter::write(size_t samplesNum, float* buffer) {
    uint8_t * ioBuffer = new uint8_t[(bitsPerSample / 8) * samplesNum];
    int32_t sample;

    if (bitsPerSample == 16) {
        for (size_t i = 0; i < samplesNum; i++) {
            sample = (buffer[i] * (float) INT16_MAX);
            write16(sample, ioBuffer, i * 2);
        }
    } else if (bitsPerSample == 24) {
        for (size_t i = 0; i < samplesNum; i++) {
            sample = (buffer[i] * (float) INT24_MAX);
            write24(sample, ioBuffer, i * 3);
        }
    }

    fwrite(ioBuffer, (bitsPerSample / 8), samplesNum, file);
    delete[] ioBuffer;
    return OK;
}
