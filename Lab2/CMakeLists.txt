cmake_minimum_required(VERSION 3.12)
project(OS)

if (UNIX)
  add_executable(hardlinks
    hardlinks.c
    vec.h
    )
elseif (MSVC)

endif()
