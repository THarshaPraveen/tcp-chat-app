add_executable(socket_server
    main.c
)

target_link_libraries(socket_server
    PRIVATE socketutility
)

