add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC "${PROJECT_SOURCE_DIR}/lib/include")

target_link_libraries(${PROJECT_NAME} task_scheduler)