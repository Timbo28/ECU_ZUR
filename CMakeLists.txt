cmake_minimum_required(VERSION 3.16.3)
project(ECU_ZUR)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXE_LINKER_FLAGS "-static-libstdc++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")



# Add additional include directories
include_directories(
    include
    path/include
)


add_definitions(-DJSON_FOLDER_PATH=\"${CMAKE_SOURCE_DIR}/.config/\")
add_definitions(-DSPDLOG_COMPILED_LIB)

file(GLOB_RECURSE SOURCE_FILES
    "source/*.cpp"
    "source/data/*.cpp"
    "source/system/*.cpp"
)
# Remove the specific files you want to exclude
list(REMOVE_ITEM SOURCE_FILES "${CMAKE_CURRENT_SOURCE_DIR}/source/main.cpp")

add_executable(ECU_ZUR source/main.cpp ${SOURCE_FILES} )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Libraries/json/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Libraries/pfr/include/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Libraries/spdlog/include/)
add_subdirectory(Libraries/pfr)  # Pfad zum PFR-Verzeichnis
add_subdirectory(Libraries/spdlog)  # Pfad zum SPDLog-Verzeichnis
add_subdirectory(Libraries/json)  # Pfad zum JSON-Verzeichnis
target_link_libraries(ECU_ZUR PRIVATE boost_pfr spdlog::spdlog nlohmann_json::nlohmann_json )

add_subdirectory(Tests)

