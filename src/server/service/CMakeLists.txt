set(SOURCES
        main.cpp
        service.cpp
)

set(HEADERS
        service.h
)

add_executable(
        server_service
        ${SOURCES}
        ${HEADERS}
)

target_link_libraries(server_service PRIVATE Qt6::WebSockets boost::boost message)
