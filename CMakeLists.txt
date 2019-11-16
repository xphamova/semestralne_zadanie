cmake_minimum_required(VERSION 3.15)
project(semestral C)

set(CMAKE_C_STANDARD 99)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(seme client.c)
target_link_libraries(seme Threads::Threads)