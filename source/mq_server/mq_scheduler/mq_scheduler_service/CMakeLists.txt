set(SOURCES
	 mq_scheduler_service.cpp
	 mq_scheduler_service.h
	 mq_scheduler_service_plugin.cpp
	 mq_scheduler_service_plugin.h)

add_library(mq_scheduler_service SHARED ${SOURCES})

set_target_properties(mq_scheduler_service PROPERTIES DEFINE_SYMBOL "DECLARE_PLUGIN_EXPORT")

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
	set_target_properties(mq_scheduler_service PROPERTIES FOLDER "mq/mq_server/mq_scheduler")
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(mq_scheduler_service PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
	endif()
endif()

target_include_directories(mq_scheduler_service PRIVATE	${CMAKE_MQ_ROOT} ${CMAKE_MQ_SERVER_ROOT}/mq_scheduler)
target_link_libraries(mq_scheduler_service PRIVATE sdp)
add_dependencies(mq_scheduler_service sdp)

install(TARGETS mq_scheduler_service DESTINATION plugin)

include(${CMAKE_WORK_ROOT}/mk_inc/common_functions.cmake)
COMPILE_OPTION_FUNCTION(mq_scheduler_service)
