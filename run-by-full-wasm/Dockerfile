FROM emscripten/emsdk:1.39.16
# https://github.com/opencv/opencv/issues/18632

WORKDIR /build
RUN git clone https://github.com/opencv/opencv.git -b 4.5.0 --depth 1
# RUN git clone https://github.com/opencv/opencv_contrib.git -b 4.5.0 --depth 1
WORKDIR /build/opencv
RUN python3 ./platforms/js/build_js.py build_wasm --build_wasm --emscripten_dir=/emsdk/upstream/emscripten --config_only
ENV OPENCV_JS_WHITELIST /build/opencv/platforms/js/opencv_js.config.py
RUN cd build_wasm && emmake make -j && emmake make install

WORKDIR /app
COPY CMakeLists.txt ./
COPY src ./src
WORKDIR /app/dist/build
RUN emcmake cmake ../..
RUN emmake make

WORKDIR /app
COPY index.html ./

CMD ["python3", "-m", "http.server", "8001"]
