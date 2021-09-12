cmake_minimum_required(VERSION 3.12)
project(OS)

if (UNIX)
  find_package(Threads REQUIRED)

  add_executable(philosophers
    philosophers.c
    )

  target_link_libraries(philosophers PRIVATE Threads::Threads)

  add_executable(readerswriters
    readerswriters.c
    )

  target_link_libraries(readerswriters PRIVATE Threads::Threads)
elseif (MSVC)

endif()
