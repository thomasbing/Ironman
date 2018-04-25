# Sample apps

set (NVSF_SAMPLE_INCLUDE
      fp32pcm.hpp)

set(NVSF_SAMPLE_SOURCE
    nvsf_sample.cpp)

source_group("Headers" FILES ${NVSF_SAMPLE_INCLUDE})
source_group("Source Files" FILES ${NVSF_SAMPLE_SOURCE})

add_executable(nvsf_sample ${NVSF_SAMPLE_SOURCE} ${NVSF_SAMPLE_INCLUDE})
target_link_libraries(nvsf_sample PRIVATE nvsf_lowlevel common_sample)
target_include_directories(nvsf_sample PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(nvsf_sample PROPERTIES FOLDER SampleApps)

# Copy executables into package folder
if(INSTALL_SDK)
    install(TARGETS nvsf_sample RUNTIME DESTINATION ./samples/nvsf_sample)

elseif(INSTALL_FLAT)
    install(TARGETS nvsf_sample RUNTIME DESTINATION .)
    install(FILES $<TARGET_PDB_FILE:nvsf_sample> DESTINATION . CONFIGURATIONS Debug)
endif()
