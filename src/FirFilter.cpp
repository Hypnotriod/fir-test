
/* 
 * File:   FirFilter.cpp
 * Author: Ilya Pikin
 */

#include <cstring>
#include <math.h>

#include "FirFilter.h"
#include "WavFileReader.h"
#include "WavFileWriter.h"

FirFilter::FirFilter(size_t maxTapsNum) : maxTapsNum(maxTapsNum), tapsNum(maxTapsNum) {
    impulse = new float[maxTapsNum];
    buffer = new float[maxTapsNum];
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
    status = reader.read(maxTapsNum, impulse, &samplesRead);
    if (status != WavFileReader::READ_ERROR) {
        tapsNum = samplesRead;
        sampleRate = reader.getHeader()->sampleRate;
    }
    return status == WavFileReader::READ_ERROR ? ERROR : OK;
}

void FirFilter::adjustGain() {
    float summ;
    float maxSumm = 0.f;

    for (size_t i = 0; i < tapsNum; i++) {
        summ = 0.f;
        for (size_t j = 0; j < tapsNum; j++) {
            summ += sinf(M_2_PI / ((float) sampleRate / ADJUST_FREQUENCY) * j)
                    * impulse[(i + j) % tapsNum];
        }
        impulse[i];
        if (maxSumm < summ) maxSumm = summ;
    }
    gain = 1.f / maxSumm;
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

