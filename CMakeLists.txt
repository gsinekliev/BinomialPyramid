project(BinomialPyramid)
cmake_minimum_required(VERSION 2.8)
aux_source_directory(. SRC_LIST)
INCLUDE_DIRECTORIES (. SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

