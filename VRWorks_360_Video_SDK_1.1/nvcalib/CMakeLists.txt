set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/library)

# Add target for nvcalib
add_library(nvcalib INTERFACE)
target_include_directories(nvcalib INTERFACE include)
target_link_libraries(nvcalib INTERFACE ${LIB_PATH}/nvcalib.lib)

# Copy binaries to build folder
file(COPY binary/nvcalib.dll DESTINATION ${CMAKE_BINARY_DIR}/Release)
