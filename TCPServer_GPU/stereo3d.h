#ifndef STEREO3D_H
#define STEREO3D_H
#include <opencv2/opencv.hpp>
#include <NvInfer.h>
#include <cuda_runtime_api.h>
#include <string>

using namespace nvinfer1;

// TensorRT模型输出图像的尺寸是BxCxHxW
class Stereo
{
public:
    Stereo();
    ~Stereo();
    int Initialize(const std::string &model_path);
    int RunStereo(const cv::Mat &img_l, const cv::Mat &img_r, cv::Mat &disp);
    int Release();
private:
    IRuntime *runtime;
    ICudaEngine *engine;
    IExecutionContext *context;

    float *buffers[3];
    int inputIndex1;
    int inputIndex2;
    int outputIndex;
    float *flow_up;

    cudaStream_t stream;
    float *img_left_host;
    float *img_right_host;
};
#endif //