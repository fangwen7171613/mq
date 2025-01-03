set(CMAKE_MQ_ROOT ${CMAKE_CURRENT_LIST_DIR})

if(MSVC)
add_subdirectory(mq_common)
endif()

add_subdirectory(mq_server)
add_subdirectory(mq_sdk)
add_subdirectory(test_mq)

install(FILES readme.txt DESTINATION ${CMAKE_INSTALL_PREFIX})

