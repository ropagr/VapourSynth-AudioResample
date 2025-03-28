// SPDX-License-Identifier: MIT

#ifndef _RESAMPLE_HPP
#define _RESAMPLE_HPP

#include <vector>

#include <samplerate.h>
#include "VapourSynth4.h"

#include "sampletype.hpp"


class Resample
{
public:
    Resample(VSNode* srcAudio, const VSAudioInfo* srcAi, int dstSampleRate, SampleType dstSampleType, int convType,
             VSCore* core, const VSAPI* vsapi);

    VSNode* getSrcAudio();

    const VSAudioInfo* getSrcAudioInfo();

    const VSAudioInfo* getDstAudioInfo();

    int getDstOverlapSamples();

    size_t getSrcBufLengthInSamples();

    void free(const VSAPI* vsapi);

    void writeFrame(VSFrame* dstFrame, int dstFrameTotal, int dstFrameSamples,
                    int64_t firstSrcSampleTotal, int64_t lastSrcSampleTotal, bool srcSamplesEnd,
                    std::vector<const VSFrame*> const &srcFrames, int firstSrcFrameTotal,
                    VSFrameContext* frameCtx, VSCore* core, const VSAPI* vsapi);

private:
    VSNode* srcAudio;
    const VSAudioInfo* srcAi;

    VSAudioInfo dstAi;

    const int dstSampleRate;

    SampleType srcSampleType;
    const SampleType dstSampleType;

    // libsamplerate conversion type
    const int convType;

    // number of channels for convenience; same for source and destination
    const int numChannels;

    // last processed destination frame
    // not using -1 as initial value because -1 would indicate that the actual first frame (0)
    // is a subsequent frame of an already processed frame (-1)
    int lastDstFrameTotal = -100;

    // libsamplerate resampler instance
    SRC_STATE* resState;

    // inclusive
    int64_t lastConsumedSrcSampleTotal = -1;

    // source buffer
    float* srcBuf = nullptr;
    // number of allocated interleaved samples, i.e. one sample of each channel
    size_t srcBufLenInSamples = 0;

    // destination buffer
    float* dstBuf = nullptr;
    // number of allocated interleaved samples, i.e. one sample of each channel
    size_t dstBufLenInSamples = 0;
    // number of interleaved samples currently stored in dstBuf
    int dstBufSamples = 0;


    template <typename src_sample_t, size_t SrcSampleIntBits, typename dst_sample_t, size_t DstSampleIntBits>
    void writeFrameNoResamplingImpl(VSFrame* dstFrame, const VSFrame* srcFrame, int samples, const VSAPI* vsapi);

    template <typename sample_t, size_t SampleIntBits>
    int fillInterleavedSamples(float* buf, size_t bufLenInSamples, int64_t firstSrcSampleTotal, int64_t lastSrcSampleTotal,
                               std::vector<const VSFrame*> const &srcFrames, int firstSrcFrameTotal, const VSAPI* vsapi);

    template <typename src_sample_t, size_t SrcSampleIntBits, typename dst_sample_t, size_t DstSampleIntBits>
    void writeFrameImpl(VSFrame* dstFrame, int dstFrameTotal, int dstFrameSamples,
                        int64_t firstSrcSampleTotal, int64_t lastSrcSampleTotal, bool srcSamplesEnd,
                        std::vector<const VSFrame*> const &srcFrames, int firstSrcFrameTotal,
                        VSFrameContext* frameCtx, VSCore* core, const VSAPI* vsapi);
};

void resampleInit(VSPlugin* plugin, const VSPLUGINAPI* vspapi);

#endif // _RESAMPLE_HPP
