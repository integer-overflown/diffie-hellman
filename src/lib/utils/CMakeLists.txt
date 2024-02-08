set(HEADERS
        include/Utils/functional.h
)

add_library(
        utils
        INTERFACE
        ${HEADERS}
)

target_include_directories(utils INTERFACE include)
