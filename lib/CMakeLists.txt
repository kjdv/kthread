file(GLOB public_headers include/*.hh include/*.h)
file(GLOB sources *.cc *.cpp *.c *.hh *.h)

add_library(kthread
    ${public_headers}
    ${sources}
)

set_target_properties(kthread PROPERTIES PUBLIC_HEADER "${public_headers}")
target_include_directories(kthread
    PUBLIC
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
      "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
      "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/include>"
)


target_link_libraries(kthread
    PUBLIC Results::results Threads::Threads
)

