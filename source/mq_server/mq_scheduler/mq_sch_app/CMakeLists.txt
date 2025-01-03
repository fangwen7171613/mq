set(SOURCES
	mq_scheduler_appmanager.cpp
	mq_scheduler_appmanager.h
	mq_scheduler_main.cpp)

set(INI_FILES mq_sch_app.yaml)

add_executable(mq_sch_app ${SOURCES} ${INI_FILES})

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES} ${INI_FILES})
	set_source_files_properties(${INI_FILES} PROPERTIES HEADER_FILE_ONLY TRUE)
	set_target_properties(mq_sch_app PROPERTIES FOLDER "mq/mq_server/mq_scheduler")
endif()

target_include_directories(mq_sch_app PRIVATE ${CMAKE_MQ_ROOT} ${CMAKE_MQ_SERVER_ROOT}/mq_scheduler)

target_link_libraries(mq_sch_app PRIVATE sdp)
add_dependencies(mq_sch_app sdp)

install(TARGETS mq_sch_app DESTINATION bin)

if(USE_DATABASE AND sqlite)
	install(FILES mq_sch_app.db  DESTINATION conf)
	USE_SQLITE_FUNCTION(mq_sch_app)
endif()

install(FILES mq_sch_app.yaml DESTINATION conf)

include(${CMAKE_WORK_ROOT}/mk_inc/common_functions.cmake)
COMPILE_OPTION_FUNCTION(mq_sch_app)

if(openssl)
	USE_OPENSSL_FUNCTION(mq_sch_app)
endif()
