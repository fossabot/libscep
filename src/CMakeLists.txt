set (CMAKE_C_FLAGS "-Wall")

add_library (scep SHARED scep.c util.c configuration.c message.c)
target_link_libraries (scep crypto m pthread rt)
if (BUILD_CLI)
add_subdirectory (clients/cli)
endif()

install(TARGETS scep LIBRARY DESTINATION ${INSTALL_LIB_DIR})
install(FILES "${PROJECT_BINARY_DIR}/scep.h" DESTINATION include)
