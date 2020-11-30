#include <SDL/SDL.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <vector>
#include <string>
#include <sstream>

#define FACE_DETECTION_CONFIGURATION "/deploy.prototxt"
#define FACE_DETECTION_WEIGHTS "/res10_300x300_ssd_iter_140000_fp16.caffemodel"

// ref. https://bewagner.net/programming/2020/04/12/building-a-face-detector-with-opencv-in-cpp/
class FaceDetector
{
public:
  explicit FaceDetector();
  std::vector<cv::Rect> detect_face_rectangles(const cv::Mat &frame);

private:
  cv::dnn::Net network_;
  const int input_image_width_;
  const int input_image_height_;
  const double scale_factor_;
  const cv::Scalar mean_values_;
  const float confidence_threshold_;
};

FaceDetector::FaceDetector() : confidence_threshold_(0.5), input_image_height_(300), input_image_width_(300),
                               scale_factor_(1.0), mean_values_({104., 177.0, 123.0})
{

  // Note: The varibles MODEL_CONFIGURATION_FILE and MODEL_WEIGHTS_FILE are passed in via cmake
  network_ = cv::dnn::readNetFromCaffe(FACE_DETECTION_CONFIGURATION, FACE_DETECTION_WEIGHTS);

  if (network_.empty())
  {
    std::ostringstream ss;
    ss << "Failed to load network with the following settings:\n"
       << "Configuration: " + std::string(FACE_DETECTION_CONFIGURATION) + "\n"
       << "Binary: " + std::string(FACE_DETECTION_WEIGHTS) + "\n";
    throw std::invalid_argument(ss.str());
  }
}

std::vector<cv::Rect> FaceDetector::detect_face_rectangles(const cv::Mat &frame)
{
  cv::Mat input_blob = cv::dnn::blobFromImage(frame, scale_factor_, cv::Size(input_image_width_, input_image_height_),
                                              mean_values_, false, false);
  network_.setInput(input_blob, "data");
  cv::Mat detection = network_.forward("detection_out");
  cv::Mat detection_matrix(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

  std::vector<cv::Rect> faces;

  for (int i = 0; i < detection_matrix.rows; i++)
  {
    float confidence = detection_matrix.at<float>(i, 2);

    if (confidence < confidence_threshold_)
    {
      continue;
    }
    int x_left_bottom = static_cast<int>(detection_matrix.at<float>(i, 3) * frame.cols);
    int y_left_bottom = static_cast<int>(detection_matrix.at<float>(i, 4) * frame.rows);
    int x_right_top = static_cast<int>(detection_matrix.at<float>(i, 5) * frame.cols);
    int y_right_top = static_cast<int>(detection_matrix.at<float>(i, 6) * frame.rows);

    faces.emplace_back(x_left_bottom, y_left_bottom, (x_right_top - x_left_bottom), (y_right_top - y_left_bottom));
  }

  return faces;
}

namespace
{
  constexpr int WIDTH = 640;
  constexpr int HEIGHT = 480;
  SDL_Surface *screen = nullptr;
  FaceDetector face_detector;

} // namespace

extern "C" int main(int argc, char **argv)
{

  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);

  return 0;
}

void doOpenCvTask(size_t addr, int width, int height, int cnt)
{
  auto data = reinterpret_cast<void *>(addr);
  cv::Mat rgbaMat(height, width, CV_8UC4, data);
  cv::Mat bgrMat;
  cv::Mat outMat;
  cv::cvtColor(rgbaMat, bgrMat, cv::COLOR_RGBA2BGR);
  auto rectangles = face_detector.detect_face_rectangles(bgrMat);
  cv::Scalar color(0, 105, 205);
  for (const auto &r : rectangles)
  {
    cv::rectangle(bgrMat, r, color, 4);
  }
  cv::cvtColor(bgrMat, outMat, cv::COLOR_BGR2RGBA);

  if (SDL_MUSTLOCK(screen))
    SDL_LockSurface(screen);
  cv::Mat dstRGBAImage(height, width, CV_8UC4, screen->pixels);
  outMat.copyTo(dstRGBAImage);
  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);
  SDL_Flip(screen);
}

EMSCRIPTEN_BINDINGS(my_module)
{
  emscripten::function("doOpenCvTask", &doOpenCvTask);
}