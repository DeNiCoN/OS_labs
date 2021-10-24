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
  add_executable(prodcons
    prodcons.c
    )

  add_executable(barber
    barber.c 
    )
endif()
