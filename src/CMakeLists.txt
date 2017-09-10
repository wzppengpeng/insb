find_package( OpenCV REQUIRED )

include_directories(${INSB_HEADER_DIR} ../3rdparty)
aux_source_directory(./insb SRC_LIST)
aux_source_directory(./io SRC_LIST)
aux_source_directory(./task SRC_LIST)
add_library(insb SHARED ${SRC_LIST})
target_link_libraries(insb ${OpenCV_LIBS})