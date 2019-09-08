file(GLOB examples *.cc *.cpp *.c)

foreach(example ${examples})
  get_filename_component(example_name ${example} NAME_WE)

  add_executable(${example_name} ${example})
  target_link_libraries(${example_name} PUBLIC kthread)
  
  add_test(example.${example_name} ${example_name})
  
endforeach()

target_link_libraries(benchmarks PUBLIC benchmark::benchmark benchmark::benchmark_main)
