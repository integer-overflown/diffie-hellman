set(HEADERS
        include/Message/message.h
        include/Message/serialization.h
        include/Message/parsing.h
)

set(SOURCES
        src/message.cpp
        src/parsing.cpp
)

set(PRIVATE_HEADERS
        src/logging.h
        src/parsing_utils.h
)

add_library(message
        ${HEADERS}
        ${SOURCES}
        ${PRIVATE_HEADERS}
)

target_include_directories(message PUBLIC include PRIVATE include/Message)

target_link_libraries(message PUBLIC Qt6::Core boost::boost PRIVATE utils)
