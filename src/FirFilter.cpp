
/* 
 * File:   FirFilter.cpp
 * Author: Ilya Pikin
 */

#include <cstdlib>
#include <cstring>

#include "FirFilter.h"
#include "WavFileReader.h"

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
    WavFileReader reader(path);
    size_t samplesRead;
    if (reader.open() != WavFileReader::OK) {
        return ERROR;
    }
    int16_t * samples = new int16_t[tapsNum];
    status = reader.read(tapsNum, samples, &samplesRead);
    if (status != WavFileReader::READ_ERROR) {
        for (size_t i = 0; i < samplesRead; i++) {
            impulse[i] = (float) samples[i] / (float) INT16_MAX;
        }
        tapsNum = samplesRead;
    }
    delete[] samples;
    return status == WavFileReader::READ_ERROR ? ERROR : OK;
}

FirFilter::Status FirFilter::process(const char * srcPath, const char * destPath) {
    WavFileReader reader(srcPath);
    WavFileReader::Status status;
    FILE * output = fopen(destPath, "wb");
    int16_t ioSampleBuff[IO_SAMPLE_BUFF_SIZE];
    size_t samplesRead;
    float sample;
    size_t sampleIndex;

    if (output == NULL || reader.open() != WavFileReader::OK) {
        fclose(output);
        delete output;
        return ERROR;
    }
    fwrite(reader.getHeaderSource(), sizeof (uint8_t), WavFileReader::HEADER_SIZE, output);

    memset(buffer, 0, sizeof (float) * tapsNum);

    while (true) {
        status = reader.read(IO_SAMPLE_BUFF_SIZE, ioSampleBuff, &samplesRead);
        if (status == WavFileReader::READ_ERROR) {
            break;

        for (sampleIndex = 0; sampleIndex < samplesRead; sampleIndex++) {
            for (size_t i = tapsNum - 1; i > 0; i--) {
                buffer[i] = buffer[i - 1];
            }
            buffer[0] = (float) ioSampleBuff[sampleIndex];

            sample = 0;
            for (size_t i = 0; i < tapsNum; i++) {
                sample += buffer[i] * impulse[i];
            }
            sample *= gain;
            ioSampleBuff[sampleIndex] = (int16_t) sample;
        }

        fwrite(ioSampleBuff, sizeof (int16_t), samplesRead, output);

        if (status == WavFileReader::END_OF_FILE)
            break;
    }

    fclose(output);
    delete output;

    return status == WavFileReader::READ_ERROR ? ERROR : OK;
}

