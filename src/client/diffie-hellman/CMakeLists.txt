add_library(diffie_hellman
        include/DiffieHellman/diffie_hellman.h
        src/diffie_hellman.cpp
)

target_include_directories(diffie_hellman PUBLIC include PRIVATE include/DiffieHellman)

target_link_libraries(diffie_hellman PUBLIC Boost::boost)
