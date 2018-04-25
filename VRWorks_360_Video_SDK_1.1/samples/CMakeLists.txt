# Sample apps

if(ENABLE_SAMPLES)
    add_library(common_sample STATIC common/filesys_util.cpp)
    target_include_directories(common_sample PUBLIC common)

    add_subdirectory(nvcalib_sample)
    add_subdirectory(nvss_sample)
    add_subdirectory(nvstitch_sample)
    add_subdirectory(nvsf_sample)
endif()
