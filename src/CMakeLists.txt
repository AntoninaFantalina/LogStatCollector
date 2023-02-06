project(LogStatCollector)

include("lib/rapidjson.cmake")
include_directories(
    ${RAPIDJSON_INCLUDE_DIR}
)

set(${PROJECT_NAME}_SRC
  ArgParser.cpp
  UserActionsStat.cpp
  CollectStat.cpp
  main.cpp)

add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC})

add_dependencies(${PROJECT_NAME} rapidjson)

target_link_libraries(${PROJECT_NAME})
