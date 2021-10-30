cmake_minimum_required(VERSION 3.12)
project(OS)

if (UNIX)
  add_executable(hardlinks
    hardlinks.c
    vec.h
    )

  add_executable(myls
    ls.c
    vec.h
    )
elseif (MSVC)
  add_executable(mydir
    mydir.c
    )
  target_compile_definitions(mydir
    PRIVATE UNICODE
  )
  add_executable(lsdrives
  lsdrives.c
  )
  target_compile_definitions(lsdrives
    PRIVATE UNICODE
  )

endif()
