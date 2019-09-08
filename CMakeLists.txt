cmake_minimum_required(VERSION 3.10)

project(kthread)
set(CMAKE_CXX_STANDARD 17)

include(GoogleTest)
find_package(GTest MODULE REQUIRED)
find_library(GMOCK_LIBRARIES gmock)
enable_testing()


# dependencies:
find_package(benchmark CONFIG REQUIRED)
find_package(Results CONFIG REQUIRED)


add_subdirectory(lib)
add_subdirectory(test)
add_subdirectory(examples)

# install rules
install(TARGETS kthread
        EXPORT KthreadTargets
        ARCHIVE DESTINATION lib COMPONENT lib
        PUBLIC_HEADER DESTINATION include/kthread COMPONENT dev
)
install(FILES KthreadConfig.cmake DESTINATION share/kthread)

install(EXPORT KthreadTargets DESTINATION share/kthread NAMESPACE Kthread::)
export(TARGETS kthread NAMESPACE Kthread:: FILE ${CMAKE_CURRENT_BINARY_DIR}/KthreadTargers.cmake)
