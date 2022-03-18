cmake_minimum_required(VERSION 3.21)
project(WebServer C)

set(CMAKE_C_STANDARD 23)

include_directories(include)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(WebServer src/main.c src/cone_string.c include/cone_string.h include/def.h
        include/metadata.h include/tcp.h include/urlencoding.h include/http/event.h include/http/eventid.h
        include/http/eventloop.h include/http/header.h src/event.c src/eventloop.c src/header.c src/metadata.c src/tcp.c src/urlencoding.c)

target_link_libraries(WebServer PRIVATE Threads::Threads)