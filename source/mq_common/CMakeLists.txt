# 创建一个自定义目标 mqtt_comm，用于显示头文件
add_custom_target(mq_comm)

# 定义头文件列表
set(MQ_COMMON_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/mq_id_def.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mq_msg_def.h)

# 将头文件添加到 mqtt_comm 目标中
target_sources(mq_comm PRIVATE ${MQ_COMMON_HEADERS})

if(MSVC)
    # 使用 source_group 指令为头文件创建文件夹分组
    source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${MQ_COMMON_HEADERS})

    # 将目标文件夹属性设置为方便在 Visual Studio 中查看
    set_target_properties(mq_comm PROPERTIES FOLDER "mq")
endif()
