#include <SDL/SDL.h>
#include <emscripten.h>
#include <emscripten/bind.h>
#include <opencv2/opencv.hpp>

namespace {

constexpr int WIDTH = 640;
constexpr int HEIGHT = 480;
SDL_Surface *screen = nullptr;

} // namespace

extern "C" int main(int argc, char **argv) {

  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(WIDTH, HEIGHT, 32, SDL_SWSURFACE);

  return 0;
}

void doOpenCvTask(size_t addr, int width, int height, int cnt) {
  auto data = reinterpret_cast<void *>(addr);
  cv::Mat rgbaMat(height, width, CV_8UC4, data);
  cv::Mat rgbMat;
  cv::Mat rgbOutMat;
  cv::Mat outMat;
  cv::cvtColor(rgbaMat, rgbMat, cv::COLOR_RGBA2RGB);
  rgbMat.convertTo(rgbOutMat, -1, 1.0, cnt - 100.0);
  cv::cvtColor(rgbOutMat, outMat, cv::COLOR_RGB2RGBA);

  if (SDL_MUSTLOCK(screen))
    SDL_LockSurface(screen);
  cv::Mat dstRGBAImage(height, width, CV_8UC4, screen->pixels);
  outMat.copyTo(dstRGBAImage);
  if (SDL_MUSTLOCK(screen))
    SDL_UnlockSurface(screen);
  SDL_Flip(screen);
}

EMSCRIPTEN_BINDINGS(my_module) {
  emscripten::function("doOpenCvTask", &doOpenCvTask);
}