set(SOURCES "test_mq_1_main.c")

add_executable(test_mq_1 ${SOURCES})

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
	set_target_properties(test_mq_1 PROPERTIES FOLDER "mq/test_mq")
endif()

target_include_directories(test_mq_1 PRIVATE ${CMAKE_MQ_ROOT}/mq_sdk ${CMAKE_MQ_ROOT}/test_mq)

target_link_libraries(test_mq_1 PRIVATE sdp)
add_dependencies(test_mq_1 sdp)

target_link_libraries(test_mq_1 PRIVATE mq_c_sdk)
add_dependencies(test_mq_1 mq_c_sdk)

install(TARGETS test_mq_1 DESTINATION bin)

include(${CMAKE_WORK_ROOT}/mk_inc/common_functions.cmake)
COMPILE_OPTION_FUNCTION_BASE(test_mq_1)

if(openssl)
	USE_OPENSSL_FUNCTION(test_mq_1)
endif()
