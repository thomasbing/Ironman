set(LIB_PATH ${CMAKE_CURRENT_SOURCE_DIR}/library)

# Add target for nvstitch
add_library(nvstitch INTERFACE)
target_include_directories(nvstitch INTERFACE include)
target_link_libraries(nvstitch INTERFACE ${LIB_PATH}/nvstitch.lib)

# Add target for nvss_video
add_library(nvss_video INTERFACE)
target_include_directories(nvss_video INTERFACE include)
target_link_libraries(nvss_video INTERFACE ${LIB_PATH}/nvss_video.lib)

# Add target for nvsf
add_library(nvsf_lowlevel INTERFACE)
target_include_directories(nvsf_lowlevel INTERFACE include)
target_link_libraries(nvsf_lowlevel INTERFACE ${LIB_PATH}/nvsf.lib)

# Copy binaries to build folder
file(COPY binary/nvstitch.dll DESTINATION ${CMAKE_BINARY_DIR}/Release)
file(COPY binary/nvss_video.dll DESTINATION ${CMAKE_BINARY_DIR}/Release)
file(COPY binary/nvsf.dll DESTINATION ${CMAKE_BINARY_DIR}/Release)
