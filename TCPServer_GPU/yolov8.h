#ifndef YOLOV8_H
#define YOLOV8_H

#include <string>
#include <vector>
#include <NvInfer.h>
#include <cuda_runtime_api.h>
#include <opencv2/opencv.hpp>

using namespace nvinfer1;

struct OutputSeg
{
    int id;           // 结果类别id
    float confidence; // 结果置信度
    cv::Rect box;     // 矩形框
    cv::Mat boxMask;  // 矩形框内mask，节省内存空间和加快速度
};

// YOLO实例分割类，实现了模型加载与初始化，模型推理的功能
class YOLO
{
public:
    YOLO();
    ~YOLO();
    int Init(const std::string &engine_path);
    void Segment(const cv::Mat &img, std::vector<OutputSeg> &output);
private:
    void Inference(IExecutionContext &context, float *input, float *output0, float *output1, int batchSize);
    void PreProcess(const cv::Mat &img, float *data);

    ICudaEngine* engine_;
    IRuntime* runtime_;
    IExecutionContext* context_;
    float* prob0_;
    float* prob1_;
};
#endif // YOLOV8_H