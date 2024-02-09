set(HEADERS
        include/Message/message.h
        include/Message/serialization.h
        include/Message/parsing.h
        include/Message/network.h
)

set(SOURCES
        src/message.cpp
        src/parsing.cpp
        src/logging.cpp
        src/network.cpp
)

set(PRIVATE_HEADERS
        src/logging.h
        src/parsing_utils.h
        src/registry.h
        src/registry.cpp
)

add_library(message
        ${HEADERS}
        ${SOURCES}
        ${PRIVATE_HEADERS}
)

target_include_directories(message PUBLIC include PRIVATE include/Message)

target_link_libraries(message PUBLIC Qt6::Core Qt6::WebSockets Boost::headers PRIVATE utils)
