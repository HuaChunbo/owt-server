#ifndef VideoMixEngineImp_h
#define VideoMixEngineImp_h

#include "VideoMixEngine.h"

#include <boost/thread/shared_mutex.hpp>
#include <map>
#include <msdk_xcoder.h>

struct InputInfo {
    VideoMixCodecType codec;
    VideoMixEngineInput* producer;
    void* decHandle;
    MemPool* mp;
};

struct OutputInfo {
    VideoMixCodecType codec;
    VideoMixEngineOutput* consumer;
    unsigned short bitrate;
    void* encHandle;
    Stream* stream;
};

struct VppInfo {
    BackgroundColor bgColor;
    unsigned int width;
    unsigned int height;
    void* vppHandle;
};

class VideoMixEngineImp {
    typedef enum {
        UN_INITIALIZED = 0,
        IDLE,
        WAITING_FOR_INPUT,
        WAITING_FOR_OUTPUT,
        IN_SERVICE
    } PipelineState;

public:
    VideoMixEngineImp();
    virtual ~VideoMixEngineImp();

    bool init(BackgroundColor bgColor, FrameSize frameSize);

    void setBackgroundColor(const BackgroundColor* bgColor);
    void setResolution(unsigned int width, unsigned int height);
    void setLayout(const CustomLayoutInfo* layout);

    InputIndex enableInput(VideoMixCodecType codec, VideoMixEngineInput* producer);
    void disableInput(InputIndex index);
    void pushInput(InputIndex index, unsigned char* data, int len);

    OutputIndex enableOutput(VideoMixCodecType codec, unsigned short bitrate, VideoMixEngineOutput* consumer);
    void disableOutput(OutputIndex index);
    void forceKeyFrame(OutputIndex index);
    void setBitrate(OutputIndex index, unsigned short bitrate);
    int pullOutput(OutputIndex index, unsigned char* buf);

private:
    InputIndex scheduleInput(VideoMixCodecType codec, VideoMixEngineInput* producer);
    void installInput(InputIndex index);
    void attachInput(InputIndex index, InputInfo* input);
    void detachInput(InputInfo* input);
    int uninstallInput(InputIndex index);
    void resetInput();
    int removeInput(InputIndex index);

    OutputIndex scheduleOutput(VideoMixCodecType codec, unsigned short bitrate, VideoMixEngineOutput* consumer);
    void installOutput(OutputIndex index);
    void attachOutput(OutputInfo* output);
    void detachOutput(OutputInfo* output);
    int uninstallOutput(OutputIndex index);
    void resetOutput();
    int removeOutput(OutputIndex index);

    void setupInputCfg(DecOptions* dec_cfg, InputInfo* input);
    void setupVppCfg(VppOptions* vpp_cfg);
    void setupOutputCfg(EncOptions* enc_cfg, OutputInfo* output);
    void setupPipeline();
    void demolishPipeline();

    bool isCodecAlreadyInUse(VideoMixCodecType codec);

private:
    PipelineState m_state;
    unsigned int m_inputIndex;
    unsigned int m_outputIndex;
    MsdkXcoder* m_xcoder;
    VppInfo* m_vpp;
    std::map<InputIndex, InputInfo> m_inputs;
    std::map<OutputIndex, OutputInfo> m_outputs;
    boost::shared_mutex m_inputMutex;
    boost::shared_mutex m_outputMutex;
    boost::shared_mutex m_stateMutex;
};

#endif
