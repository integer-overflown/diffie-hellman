set(SOURCES
        main.cpp
        service.cpp
        crypto_config.cpp
)

set(HEADERS
        service.h
        crypto_config.h
)

add_executable(
        server_service
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(server_service PRIVATE Qt6::WebSockets Boost::headers message)
