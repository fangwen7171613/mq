set(SOURCES
	mcs_forwarder_agent.cpp
	mcs_forwarder_agent.h
	mcs_forwarder_agent.inl
	mcs_common_def.h
	mcs_export.h
	mcs_interface.cpp
	mcs_interface.h
	mcs_forwarder_scheduler.cpp
	mcs_forwarder_scheduler.h
	mcs_forwarder_scheduler.inl)

set(CONFIG_FILES mq_sdk.yaml)

add_library(mq_c_sdk SHARED ${SOURCES} ${CONFIG_FILES})

set_target_properties(mq_c_sdk PROPERTIES DEFINE_SYMBOL "DECLARE_MQ_C_EXPORT")
set_source_files_properties(${CONFIG_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${CONFIG_FILES})
	set_target_properties(mq_c_sdk PROPERTIES FOLDER "mq/mq_sdk")
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(mq_c_sdk PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
	endif()
endif()

target_include_directories(mq_c_sdk PRIVATE ${CMAKE_MQ_ROOT} ${CMAKE_MQ_SDK_ROOT})
target_link_libraries(mq_c_sdk PRIVATE sdp)
add_dependencies(mq_c_sdk sdp)

install(FILES mcs_export.h  DESTINATION include)
install(FILES mcs_common_def.h  DESTINATION include)
install(FILES mcs_interface.h  DESTINATION include)
install(TARGETS mq_c_sdk DESTINATION lib)

if(USE_DATABASE AND sqlite)
	install(FILES mq_sdk.db  DESTINATION conf)
	USE_SQLITE_FUNCTION(mq_c_sdk)
endif()

install(FILES mq_sdk.yaml DESTINATION conf)

include(${CMAKE_WORK_ROOT}/mk_inc/common_functions.cmake)
COMPILE_OPTION_FUNCTION(mq_c_sdk)
