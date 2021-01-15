
/* 
 * File:   WavFileWriter.h
 * Author: Ilya Pikin
 */

#include <cstdlib>
#include <cstring>
#include <cstdio>

#include "WavFileWriter.h"

#define toShort(_src, _buff, _index) _buff[_index] = _src & 0xFF; \
                                     _buff[_index+1] = ((_src >> 8) & 0xFF)
#define toLong(_src, _buff, _index) _buff[_index] = _src & 0xFF; \
                                    _buff[_index+1] = ((_src >> 8) & 0xFF); \
                                    _buff[_index+2] = ((_src >> 16) & 0xFF); \
                                    _buff[_index+3] = ((_src >> 24) & 0xFF)

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

WavFileWriter::Status WavFileWriter::writeHeader(WavFileHeader* header) {
    uint8_t headerSource[WAV_FILE_HEADER_SIZE];

    size_t chunkSize = header->subchunk2Size + WAV_FILE_HEADER_SIZE - 8;

    memcpy(&headerSource[0], header->chunkId, 4);
    toLong(chunkSize, headerSource, 4);
    memcpy(&headerSource[8], header->format, 4);
    memcpy(&headerSource[12], header->subchunk1Id, 4);
    toLong(header->subchunk1Size, headerSource, 16);
    toShort(header->audioFormat, headerSource, 20);
    toShort(header->numChannels, headerSource, 22);
    toLong(header->sampleRate, headerSource, 24);
    toLong(header->byteRate, headerSource, 28);
    toShort(header->blockAlign, headerSource, 32);
    toShort(header->bitsPerSample, headerSource, 34);
    memcpy(&headerSource[36], header->subchunk2Id, 4);
    toLong(header->subchunk2Size, headerSource, 40);

    fwrite(headerSource, sizeof (uint8_t), WAV_FILE_HEADER_SIZE, file);

    return OK;
}

WavFileWriter::Status WavFileWriter::write(size_t samplesNum, int16_t* buffer) {
    fwrite(buffer, sizeof (int16_t), samplesNum, file);
    return OK;
}
