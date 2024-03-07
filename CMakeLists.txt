cmake_minimum_required(VERSION 3.1)
project(azs_local_server)
set(MAIN_SOURCES src/controller.cpp
    src/model.cpp
    src/view.cpp
    src/core.cpp
    src/local_data.cpp
    src/azs.cpp
    src/web_tehnology.cpp
    src/users_controller.cpp
    library/sqlite/sqlite3.c
    )

IF (WIN32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/azs_site)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/azs_site)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/azs_site/dist/ DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE}/azs_site)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/windows/mysqlconn/${CMAKE_BUILD_TYPE}/lib64/mysqlcppconn-9-vs14.dll DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/windows/mysqlconn/${CMAKE_BUILD_TYPE}/lib64/libssl-1_1-x64.dll DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/windows/mysqlconn/${CMAKE_BUILD_TYPE}/lib64/libcrypto-1_1-x64.dll DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/windows/openssl/libcrypto-3-x64.dll DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/windows/openssl/libssl-3-x64.dll DESTINATION ${CMAKE_BINARY_DIR}/bin/${CMAKE_BUILD_TYPE})
    include_directories(library/windows/mysqlconn/include/jdbc)
    include_directories(library/windows/openssl/include)
    link_directories(library/windows)
    link_directories(library/windows/mysqlconn/${CMAKE_BUILD_TYPE}/lib64)
    include_directories(library/sqlite)
    set(SOURCES ${MAIN_SOURCES})
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
    
ELSE()
    file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/bin/azs_site)
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin/azs_site)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/azs_site/dist/ DESTINATION ${CMAKE_BINARY_DIR}/bin/azs_site)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/linux/mysqlconn/lib64/private/libssl.so.3 DESTINATION ${CMAKE_BINARY_DIR}/bin)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/linux/mysqlconn/lib64/private/libcrypto.so.3 DESTINATION ${CMAKE_BINARY_DIR}/bin)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/linux/mysqlconn/lib64/libmysqlcppconn.so.9 DESTINATION ${CMAKE_BINARY_DIR}/bin)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/library/linux/mysqlconn/lib64/libmysqlcppconn.so.9.8.3.0 DESTINATION ${CMAKE_BINARY_DIR}/bin)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/Dockerfile DESTINATION ${CMAKE_BINARY_DIR}/bin)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.dockerignore DESTINATION ${CMAKE_BINARY_DIR}/bin)
    include_directories(library/linux/openssl/inlcude)
    include_directories(library/linux/mysqlconn/include/jdbc)
    link_directories(library/linux/mysqlconn/lib64)
    link_directories(library/linux/mysqlconn/lib64/private)
    link_directories(library/linux/openssl)
    include_directories(library/sqlite)
    set(SOURCES ${MAIN_SOURCES})
   # Включаємо RPATH
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

    # Вказуємо RPATH для знаходження бібліотеки в тій самій папці
    set(CMAKE_INSTALL_RPATH "$ORIGIN")
ENDIF()

add_subdirectory(library/json-develop)
include_directories(library/json-develop/include)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG_BUILD)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    add_definitions(-DRELEASE_BUILD)
endif()

include_directories(library/jwt)
include_directories(include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_subdirectory(library/Crow)
link_directories(library/Crow/include)

add_executable(${PROJECT_NAME} main.cpp ${SOURCES})
IF (WIN32)
target_link_libraries(${PROJECT_NAME} Crow vs14/libssl vs14/libcrypto vs14/mysqlcppconn openssl/libssl openssl/libcrypto )
ELSE()
target_link_libraries(${PROJECT_NAME} Crow libssl.so.3 libcrypto.so.3 libmysqlcppconn.so )

ENDIF()
target_link_libraries(${PROJECT_NAME} nlohmann_json::nlohmann_json)