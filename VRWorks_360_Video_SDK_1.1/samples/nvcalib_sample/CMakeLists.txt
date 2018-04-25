set(SOURCE_FILES nvcalib_sample_app.cpp)
set(HEADER_FILES )

# Set Visual Studio source filters
source_group("Source Files" FILES ${SOURCE_FILES})
source_group("Header Files" FILES ${HEADER_FILES})

add_executable(nvcalib_sample ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(nvcalib_sample PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(nvcalib_sample PRIVATE nvcalib common_sample common_util OpenCV)

set_target_properties(nvcalib_sample PROPERTIES FOLDER SampleApps)

# Copy executables into package folder
if(INSTALL_SDK)
    install(TARGETS nvcalib_sample RUNTIME DESTINATION ./samples/nvcalib_sample)

elseif(INSTALL_FLAT)
    install(TARGETS nvcalib_sample RUNTIME DESTINATION .)
    install(FILES $<TARGET_PDB_FILE:nvcalib_sample> DESTINATION . CONFIGURATIONS Debug)
endif()
