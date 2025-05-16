include(FetchContent)

# ncurses
message(STATUS "Locating Curses ...")
find_package(Curses REQUIRED)

# cli11
FetchContent_Declare(
    cli11
    GIT_REPOSITORY https://github.com/CLIUtils/CLI11
    GIT_TAG v2.5.0 # 2.5.0
)
message(STATUS "Fetching CLIUtils/CLI11 ...")
set(CLI11_PRECOMPILED ON)
FetchContent_MakeAvailable(cli11)


message(STATUS "Linking ncurses ...")
target_link_libraries(${PROJECT_NAME} PRIVATE ${CURSES_NCURSES_LIBRARY} ${CURSES_CURSES_LIBRARY})
message(STATUS "Linking cli11 ...")
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)
