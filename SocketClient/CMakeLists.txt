add_executable(socket_client
    main.c
)

target_link_libraries(socket_client
    PRIVATE socketutility
)

