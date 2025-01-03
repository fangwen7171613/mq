set(JAVA_SOURCES
	mq_MqClass.cpp
	mq_MqClass.h)

add_library(jmq_sdk SHARED ${JAVA_SOURCES} ${MQ_COMMON_SOURCES})
set_target_properties(jmq_sdk PROPERTIES DEFINE_SYMBOL "DECLARE_MQ_EXPORT")

if(MSVC)
	source_group(TREE ${CMAKE_MQ_ROOT} FILES ${SOURCES})
	set_target_properties(jmq_sdk PROPERTIES FOLDER "mq/mq_sdk")
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(jmq_sdk PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
	endif()
endif(MSVC)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${JAVA_SOURCES})
	set_target_properties(jmq_sdk PROPERTIES FOLDER "mq/mq_sdk")
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(jmq_sdk PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
	endif()
endif()

target_include_directories(jmq_sdk PRIVATE ${CMAKE_MQ_ROOT}/mq_sdk ${CMAKE_MQ_SDK_ROOT})

find_package(Java REQUIRED)
find_package(JNI REQUIRED)

include_directories(${JNI_INCLUDE_DIRS})
target_link_libraries(jmq_sdk PRIVATE ${JNI_LIBRARIES})

target_link_libraries(jmq_sdk PRIVATE sdp)
add_dependencies(jmq_sdk sdp)

target_link_libraries(jmq_sdk PRIVATE mq_c_sdk)
add_dependencies(jmq_sdk mq_c_sdk)

install(TARGETS jmq_sdk DESTINATION lib)
install(FILES mq_MqClass.h  DESTINATION include)

include(${CMAKE_WORK_ROOT}/mk_inc/common_functions.cmake)
COMPILE_OPTION_FUNCTION(jmq_sdk)
