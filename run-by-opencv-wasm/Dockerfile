FROM emscripten/emsdk:1.39.16 AS builder
# https://github.com/opencv/opencv/issues/18632

WORKDIR /build
RUN git clone https://github.com/opencv/opencv.git -b 4.5.0 --depth 1
# RUN git clone https://github.com/opencv/opencv_contrib.git -b 4.5.0 --depth 1
WORKDIR /build/opencv
COPY opencv_js.config.py ./platform/js/
RUN python3 ./platforms/js/build_js.py build_wasm --build_wasm --emscripten_dir=/emsdk/upstream/emscripten

FROM python:3.8-slim

WORKDIR /app
COPY index.html ./
COPY --from=builder /build/opencv/build_wasm/bin/opencv.js ./


CMD ["python3", "-m", "http.server", "8002"]
