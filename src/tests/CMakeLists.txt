set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
include_directories( ${PROJECT_SOURCE_DIR}/include/lsh )
add_executable( test_lsh test_lsh )
target_link_libraries( test_lsh sampledlsh l1lsh listdb array_lists mt19937-64 m)
