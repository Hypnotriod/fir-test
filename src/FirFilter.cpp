
/* 
 * File:   FirFilter.cpp
 * Author: Ilya Pikin
 */

#include <cstdlib>
#include <cstring>

#include "FirFilter.h"
#include "WavFileReader.h"
#include "WavFileWriter.h"

FirFilter::FirFilter(size_t tapsNum, float gain) : tapsNum(tapsNum), gain(gain) {
    impulse = new float[tapsNum];
    buffer = new float[tapsNum];
}

FirFilter::~FirFilter() {
    delete[] impulse;
    delete[] buffer;
}

FirFilter::Status FirFilter::readImpulse(const char* path) {
    WavFileReader::Status status;
    WavFileReader reader;
    size_t samplesRead;
    if (reader.open(path) != WavFileReader::OK) {
        return ERROR;
    }
    status = reader.read(tapsNum, impulse, &samplesRead);
    if (status != WavFileReader::READ_ERROR) {
        tapsNum = samplesRead;
    }
    return status == WavFileReader::READ_ERROR ? ERROR : OK;
}

FirFilter::Status FirFilter::process(const char * srcPath, const char * destPath) {
    WavFileReader reader;
    WavFileReader::Status readerStatus;
    WavFileWriter writer;
    float ioSampleBuff[IO_SAMPLE_BUFF_SIZE];
    size_t samplesRead;
    size_t sampleIndex;
    float sample;

    if (reader.open(srcPath) != WavFileReader::OK || writer.open(destPath) != WavFileWriter::OK)
        return ERROR;

    writer.writeHeader(*reader.getHeader());

    memset(buffer, 0, sizeof (float) * tapsNum);

    while (true) {
        readerStatus = reader.read(IO_SAMPLE_BUFF_SIZE, ioSampleBuff, &samplesRead);
        if (readerStatus == WavFileReader::READ_ERROR)
            break;

        for (sampleIndex = 0; sampleIndex < samplesRead; sampleIndex++) {
            for (size_t i = tapsNum - 1; i > 0; i--) {
                buffer[i] = buffer[i - 1];
            }
            buffer[0] = ioSampleBuff[sampleIndex];

            sample = 0.f;
            for (size_t i = 0; i < tapsNum; i++) {
                sample += buffer[i] * impulse[i];
            }
            sample *= gain;
            ioSampleBuff[sampleIndex] = sample;
        }

        if (writer.write(samplesRead, ioSampleBuff) != WavFileWriter::OK)
            return ERROR;

        if (readerStatus == WavFileReader::END_OF_FILE)
            break;
    }

    return readerStatus == WavFileReader::READ_ERROR ? ERROR : OK;
}

