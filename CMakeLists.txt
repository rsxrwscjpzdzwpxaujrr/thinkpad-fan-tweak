cmake_minimum_required(VERSION 3.0)

project(thinkpad-fan-tweak LANGUAGES C)

include(GNUInstallDirs)

add_executable(${PROJECT_NAME}
               src/main.c
               src/config.h
               src/config.c)

configure_file(${PROJECT_NAME}.service.in
               ${PROJECT_NAME}.service)

install(TARGETS ${PROJECT_NAME} RUNTIME
        DESTINATION ${CMAKE_INSTALL_BINDIR})

install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.service
        DESTINATION /lib/systemd/system/)
