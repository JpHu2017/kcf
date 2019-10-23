cmake_minimum_required(VERSION 3.10)
project(kcf_test)
find_package(OpenCV 3.2 REQUIRED)
set(SRC ffttools.hpp fhog.cpp fhog.hpp kcftracker.cpp kcftracker.hpp 
        labdata.hpp recttools.hpp tracker.h)
add_executable(kcf_test kcf_test.cpp ${SRC})
target_link_libraries(kcf_test ${OpenCV_LIBS}) 