add_library(socketutility
    socketutil.c
)

target_include_directories(socketutility
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)

