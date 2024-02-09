set(HEADERS
        client.h
        crypto_config.h
)

set(SOURCES
        main.cpp
        client.cpp
        crypto_config.cpp
)

add_executable(client_app
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(client_app PRIVATE Qt6::Core Qt6::WebSockets message diffie_hellman)
