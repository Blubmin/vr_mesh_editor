cmake_minimum_required(VERSION 3.0)
project(tori)

file(GLOB SOURCES ${CMAKE_SOURCE_DIR}/*.cpp)
file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/*.h)

add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${HEADERS})
include_directories(${CMAKE_PROJECT_NAME} ${CMAKE_SOURCE_DIR})

if (WIN32)
	set(GLEW_DIR "C:/Users/stfreed/Downloads/glew-2.0.0-win32/glew-2.0.0")
	set(GLFW_DIR "C:/Users/stfreed/Downloads/glfw-3.2.1.bin.WIN32/glfw-3.2.1.bin.WIN32")
	set(GLM_DIR "C:/Users/stfreed/Downloads/glm-0.9.8.4")
	set(OPENVR_DIR "C:/Users/stfreed/Downloads/openvr-master/openvr-master")

	include_directories(${CMAKE_PROJECT_NAME}
		${GLEW_DIR}/include
		${GLFW_DIR}/include
		${GLM_DIR}
		${OPENVR_DIR}/headers
	)

	target_link_libraries(${CMAKE_PROJECT_NAME}
		${GLEW_DIR}/lib/Release/Win32/glew32.lib
		${GLFW_DIR}/lib-vc2015/glfw3.lib
		${OPENVR_DIR}/lib/win32/openvr_api.lib
	)
endif()

