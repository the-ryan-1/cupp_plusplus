cmake_minimum_required(VERSION 3.22)
project(cupp++)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -framework Security -framework CoreServices")


include(FetchContent)

FetchContent_Declare(
        Certify
        GIT_REPOSITORY https://github.com/djarek/certify.git
        GIT_TAG master
)

FetchContent_MakeAvailable(Certify)

# Include the project's headers and libraries
include_directories(${certify_SOURCE_DIR}/include)



find_package(Boost 1.82.0 REQUIRED COMPONENTS system date_time filesystem thread regex chrono atomic serialization context coroutine)
find_package(OpenSSL REQUIRED)

add_executable(cupp++ main.cpp)

target_include_directories(cupp++ PUBLIC
        ${Boost_INCLUDE_DIRS}
        ${OPENSSL_INCLUDE_DIR}
        ${CMAKE_BINARY_DIR}/external/certify/include
        )

target_link_libraries(cupp++
        ${CMAKE_DL_LIBS}
        ${OPENSSL_LIBRARIES}
        ${OPENSSL_SSL_LIBRARIES}
        Boost::system Boost::date_time Boost::filesystem Boost::thread Boost::regex Boost::chrono Boost::atomic Boost::serialization Boost::context Boost::coroutine
        )

message(STATUS "OpenSSL: Version ${OPENSSL_VERSION}")
message(STATUS "OpenSSL: include dir at ${OPENSSL_INCLUDE_DIR}")
message(STATUS "OpenSSL: libraries at ${OPENSSL_LIBRARIES} ${OPENSSL_SSL_LIBRARIES}")

message(STATUS "Boost: Version ${Boost_VERSION}")
message(STATUS "Boost: include dir at ${Boost_INCLUDE_DIRS}")
message(STATUS "Boost: libraries at ${Boost_LIBRARIES}")
