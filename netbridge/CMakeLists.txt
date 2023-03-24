file(GLOB SRC
${CMAKE_CURRENT_SOURCE_DIR}/*.cc
)

add_library(netbridge SHARED ${SRC})