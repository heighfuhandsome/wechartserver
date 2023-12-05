include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/service ${CMAKE_CURRENT_SOURCE_DIR}/model)

include_directories(${PROJECT_SOURCE_DIR}/dbinc)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/model/orm)

file(GLOB_RECURSE SRC_LIST ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

link_directories(${PROJECT_SOURCE_DIR}/lib)

link_libraries(mysqlclient muduo_net muduo_base pthread jsoncpp mysqlconnpoll)

add_executable(${PROJECT_NAME} ${SRC_LIST})