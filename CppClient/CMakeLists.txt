cmake_minimum_required(VERSION 3.8)
project(CppClient)
set(CMAKE_BUILD_TYPE "Release")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

include_directories(include)
# link_libraries(${CMAKE_CURRENT_SOURCE_DIR}/ViconDataStreamSDK_1.12_145507h/20230413_145507h/Release/Linux64/)
find_library(
  VICON_LIBS 
  NAMES ViconDataStreamSDK_CPP 
  PATHS ${CMAKE_CURRENT_SOURCE_DIR}/lib)
message(STATUS "!!!!! ViconDataStreamSDK_CPP found !!!!! \n${VICON_LIBS}")

add_executable(client src/client.cpp)

# target_link_libraries(client PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/ViconDataStreamSDK_1.12_145507h/20230413_145507h/Release/Linux64/libViconDataStreamSDK_CPP.so) 
target_link_libraries(client ${VICON_LIBS})

install(TARGETS client DESTINATION lib/${PROJECT_NAME})