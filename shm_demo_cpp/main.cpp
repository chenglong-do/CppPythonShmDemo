#include <iostream>
#include <opencv2/opencv.hpp>
#include <memory>
#include <thread>

#include "shared_memory_tool.h"

#define RGB_IMG_WIDTH 1920
#define RGB_IMG_HEIGHT 1080

struct SharedNNImage {
  int is_processed;
  int width;
  int height;
  double scale;
  uint8_t data[RGB_IMG_WIDTH * RGB_IMG_HEIGHT * 3];
};

int main() {
  // 初始化共享内存工具
  const auto sharedData = std::make_shared<SharedMemoryTool<SharedNNImage>>("SharedDataDemo");

  // 读取图片
  const cv::Mat rgbCvFrame = cv::imread("../test.jpg");

  // 如果图片不是1080P，缩放至1080P
  cv::Mat resizedFrame;
  if (rgbCvFrame.cols != RGB_IMG_WIDTH || rgbCvFrame.rows != RGB_IMG_HEIGHT) {
    cv::resize(rgbCvFrame, resizedFrame, cv::Size(RGB_IMG_WIDTH, RGB_IMG_HEIGHT));
  } else {
    resizedFrame = rgbCvFrame;
  }

  bool completed = false;

  // 等待 Python 端处理完成
  while (!completed) {
    // 读取共享内存数据
    const auto sharedNNImage = sharedData->readData();

    if (sharedNNImage->is_processed == 0) {
      sharedNNImage->scale = 3.14;
      sharedNNImage->width = resizedFrame.cols;
      sharedNNImage->height = resizedFrame.rows;

      // 更改共享内存图片数据
      const auto imgSize = resizedFrame.total() * resizedFrame.elemSize();
      std::memcpy(sharedNNImage->data, resizedFrame.data, imgSize);

      // 将更改后的数据写入共享内存中
      sharedData->writeData(sharedNNImage.get());

      std::cout << "waiting for processing..." << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    } else {
      completed = true;
      std::cout << "Processing completed!" << std::endl;
    }
  }

  return 0;
}