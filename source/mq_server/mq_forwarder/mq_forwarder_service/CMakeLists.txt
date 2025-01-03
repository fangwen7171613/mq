set(SOURCES
	mq_forwarder_scheduler.cpp
	mq_forwarder_scheduler.h
	mq_forwarder_service.cpp
	mq_forwarder_service.h
	mq_forwarder_service.inl
	mq_forwarder_servicee_plugin.cpp
	mq_forwarder_servicee_plugin.h)

add_library(mq_forwarder_service SHARED ${SOURCES})

set_target_properties(mq_forwarder_service PROPERTIES DEFINE_SYMBOL "DECLARE_PLUGIN_EXPORT")

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
	set_target_properties(mq_forwarder_service PROPERTIES FOLDER "mq/mq_server/mq_forwarder")
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(mq_forwarder_service PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
	endif()
endif()

target_include_directories(mq_forwarder_service PRIVATE	${CMAKE_MQ_ROOT} ${CMAKE_MQ_SERVER_ROOT}/mq_forwarder)
target_link_libraries(mq_forwarder_service PRIVATE sdp)
add_dependencies(mq_forwarder_service sdp)

install(TARGETS mq_forwarder_service DESTINATION plugin)

include(${CMAKE_WORK_ROOT}/mk_inc/common_functions.cmake)
COMPILE_OPTION_FUNCTION(mq_forwarder_service)
