cmake_minimum_required(VERSION 3.26)
project(breakout)

set(CMAKE_CXX_STANDARD 20)
set(SFML_SRC_DIR libs/sfml)
set(SFML_BUILD_DIR sfml_build)
add_subdirectory(${SFML_SRC_DIR} ${SFML_BUILD_DIR})

add_executable(breakout
    main.cpp
    constants.h
    ball.h
    player.h
    block.h
)
add_executable(levelcreator
    lvl/levelcreator.cpp
    constants.h
    lvl/cursor.h
)

target_link_libraries(breakout
    PRIVATE
    sfml-window
    sfml-graphics
    sfml-system
)
target_link_libraries(levelcreator
    PRIVATE
    sfml-window
    sfml-graphics
    sfml-system
)