include_directories(${INSB_HEADER_DIR} ../3rdparty)
aux_source_directory(. SRC_LIST)
LINK_DIRECTORIES(${LIBRARY_OUTPUT_PATH})
add_executable(ins_bagger ${SRC_LIST})
TARGET_LINK_LIBRARIES(ins_bagger insb)