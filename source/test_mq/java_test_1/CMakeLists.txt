# 设置Java编译器
find_package(Java REQUIRED)
include(UseJava)

# 设置源文件和目标目录
set(SOURCE_FILE ${CMAKE_CURRENT_LIST_DIR}/mq/MqClass.java)
set(DEST_DIR $ENV{WORK_ROOT}/bin/jt1)

# 确保目标目录存在
file(MAKE_DIRECTORY ${DEST_DIR})

# 添加一个自定义命令来直接在目标目录编译Java文件
add_custom_command(
    OUTPUT ${DEST_DIR}/MqClass.class
    COMMAND ${Java_JAVAC_EXECUTABLE} -d ${DEST_DIR} ${SOURCE_FILE}
    DEPENDS ${SOURCE_FILE}
    COMMENT "Compiling MqClass.java directly to the destination directory"
)

# 添加一个自定义目标来触发Java文件的编译
add_custom_target(
    test_java_mq_1 ALL
    DEPENDS ${DEST_DIR}/MqClass.class
)

if(MSVC)
	set_target_properties(test_java_mq_1 PROPERTIES FOLDER "mq/test_mq")
endif()
