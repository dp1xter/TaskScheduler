set(INCLUDE_FILES
    include/task_scheduler.h include/any_type.h include/task_types.h
)

include_directories(include)

add_library(task_scheduler INTERFACE ${INCLUDE_FILES})