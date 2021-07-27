
set(ROCKIT_FILE_CONFIGS ${CMAKE_CURRENT_LIST_DIR}/conf)
set(ROCKIT_FILE_HEADERS ${CMAKE_CURRENT_LIST_DIR}/headers)

option(ENABLE_UAC  "enable uac" ON)
if (${ENABLE_UAC})
    set(ROCKIT_FILE_LIBS ${CMAKE_CURRENT_LIST_DIR}/lib32/librockit.so)
else()
    set(ROCKIT_FILE_LIBS ${CMAKE_CURRENT_LIST_DIR}/lib32/uaccut/librockit.so)
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/configs")
    file(GLOB ROCKIT_FILE_CONFIGS "${CMAKE_CURRENT_LIST_DIR}/configs/*")
endif()