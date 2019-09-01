file(GLOB sources *.c *.cc *.cpp *.h *.hh)

add_executable(kthread_test ${sources})
target_link_libraries(kthread_test kthread ${GMOCK_LIBRARIES} GTest::GTest GTest::Main)

gtest_discover_tests(kthread_test)

add_test(soak_test kthread_test --gtest_repeat=100 --gtest_shuffle)
