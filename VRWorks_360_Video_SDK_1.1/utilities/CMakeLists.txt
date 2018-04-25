set(SOURCE_FILES
    math_util/math_utility.cpp

    xml_util/xml_utility_audio_rig.cpp
    xml_util/xml_utility_hl.cpp
    xml_util/xml_utility_camera_rig.cpp
    xml_util/xml_utility_common.cpp)

# Listing headers here only to show in VS; search paths must be added separately
set(HEADER_FILES
    math_util/math_utility.h

    xml_util/xml_utility_audio.h
    xml_util/xml_utility_hl.h
    xml_util/xml_utility_video.h
    xml_util/xml_utility_common.h)

add_library(common_util STATIC ${SOURCE_FILES} ${HEADER_FILES})
target_include_directories(common_util PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(common_util PUBLIC nvstitch nvcalib rapidxml)
