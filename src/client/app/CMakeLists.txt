set(HEADERS
        client.h
)

set(SOURCES
        main.cpp
        client.cpp
)

add_executable(client_app
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(client_app PRIVATE Qt6::Core Qt6::WebSockets message)
