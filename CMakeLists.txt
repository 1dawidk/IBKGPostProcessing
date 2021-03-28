cmake_minimum_required(VERSION 3.17)
project(IBKGPostProcessing)

set(CMAKE_CXX_STANDARD 11)

find_package(OpenCV REQUIRED)


add_executable(IBKGPostProcessing main.cpp DSP.cpp DSP.h FrequencySpectrograph.cpp FrequencySpectrograph.h UI.cpp UI.h Plot.cpp Plot.h)
target_link_libraries(IBKGPostProcessing ${OpenCV_LIBS} fftw3)