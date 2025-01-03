set(SOURCES
	mq_forwarder_appmanager.cpp
	mq_forwarder_appmanager.h
	mq_forwarder_main.cpp)

set(INI_FILES mq_fwd_app.yaml)

add_executable(mq_fwd_app ${SOURCES} ${INI_FILES})

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INI_FILES})
	set_source_files_properties(${INI_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
	set_target_properties(mq_fwd_app PROPERTIES FOLDER "mq/mq_server/mq_forwarder")
endif()

target_include_directories(mq_fwd_app PRIVATE ${CMAKE_MQ_ROOT} ${CMAKE_MQ_SERVER_ROOT}/mq_forwarder)

target_link_libraries(mq_fwd_app PRIVATE sdp)
add_dependencies(mq_fwd_app sdp)

install(TARGETS mq_fwd_app DESTINATION bin)

if(USE_DATABASE AND sqlite)
	install(FILES mq_fwd_app.db  DESTINATION conf)
	USE_SQLITE_FUNCTION(mq_fwd_app)
endif()

install(FILES mq_fwd_app.yaml DESTINATION conf)

include(${CMAKE_WORK_ROOT}/mk_inc/common_functions.cmake)
COMPILE_OPTION_FUNCTION(mq_fwd_app)

if(openssl)
	USE_OPENSSL_FUNCTION(mq_fwd_app)
endif()
