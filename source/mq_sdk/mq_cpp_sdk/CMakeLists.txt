set(SOURCES
	mq_cpp_sdk_common_def.cpp
	mq_cpp_sdk_common_def.h
	mq_cpp_sdk_export.h
	mq_cpp_sdk_interface.cpp
	mq_cpp_sdk_interface.h
	mq_cpp_sdk_interface.inl
	mq_msg_bind.h)

add_library(mq_cpp_sdk SHARED ${SOURCES})
set_target_properties(mq_cpp_sdk PROPERTIES DEFINE_SYMBOL "DECLARE_MQ_CPP_EXPORT")

if(MSVC)
	source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCES})
	set_target_properties(mq_cpp_sdk PROPERTIES FOLDER "mq/mq_sdk")
else()
	if(CMAKE_BUILD_TYPE STREQUAL "Release")
		target_compile_options(mq_cpp_sdk PRIVATE -fvisibility=hidden -fvisibility-inlines-hidden)
	endif()
endif()

target_include_directories(mq_cpp_sdk PRIVATE ${CMAKE_MQ_ROOT} ${CMAKE_MQ_SDK_ROOT})

target_link_libraries(mq_cpp_sdk PRIVATE sdp)
add_dependencies(mq_cpp_sdk sdp)

target_link_libraries(mq_cpp_sdk PRIVATE mq_c_sdk)
add_dependencies(mq_cpp_sdk mq_c_sdk)

install(TARGETS mq_cpp_sdk DESTINATION lib)

include(${CMAKE_WORK_ROOT}/mk_inc/common_functions.cmake)
COMPILE_OPTION_FUNCTION(mq_cpp_sdk)
