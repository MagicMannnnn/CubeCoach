cmake_minimum_required(VERSION 3.1)

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>") #we want to use the static runtime
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")

project(CubeCoach)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
#add_subdirectory(thirdparty/glfw-3.3.2)			#window oppener
set(GLFW_BUILD_SHARED OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw-3.3.2)
add_subdirectory(thirdparty/glad)				#opengl loader
add_subdirectory(thirdparty/stb_image)			#loading imma
add_subdirectory(thirdparty/stb_truetype)	
add_subdirectory(thirdparty/glm)				#math
#add_subdirectory(thirdparty/assimp)	


# Define MY_SOURCES to be a list of all the source files for my game 
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")


if(WIN32)
    add_executable("${CMAKE_PROJECT_NAME}" ${MY_SOURCES} "${CMAKE_SOURCE_DIR}/app_icon.rc")
else()
    add_executable("${CMAKE_PROJECT_NAME}" ${MY_SOURCES})
endif()




set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY CXX_STANDARD 17)

#target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources/") # This is useful to get an ASSETS_PATH in your IDE during development but you should comment this if you compile a release version and uncomment the next line
target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="./resources/") # Uncomment this line to setup the ASSETS_PATH macro to the final assets directory when you share the game

#set(DOCUMENTS_PATH "$ENV{USERPROFILE}/Documents")
#target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC RESOURCES_PATH="/${DOCUMENTS_PATH}/CubeCoach/")


target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${MY_SOURCES} )


if(MSVC) # If using the VS compiler...

	target_compile_definitions("${CMAKE_PROJECT_NAME}" PUBLIC _CRT_SECURE_NO_WARNINGS)

	#remove console
	set_target_properties("${CMAKE_PROJECT_NAME}" PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
	
	set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedDebug<$<CONFIG:Debug>:Debug>")
	set_property(TARGET "${CMAKE_PROJECT_NAME}" PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreadedRelease$<$<CONFIG:Release>:Release>")

endif()


target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/")


target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE glm glfw 
	glad stb_image stb_truetype)

# Add threading support
#set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE Threads::Threads)






# Package config
set(CPACK_PACKAGE_NAME "CubeCoach")
set(CPACK_PACKAGE_VERSION "0.1.0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CubeCoach")             # <<<< REMOVE version from path
set(CPACK_NSIS_INSTALL_ROOT "$PROGRAMFILES64/")              # <<<< Add trailing slash
set(CPACK_OUTPUT_FILE_PREFIX "${CMAKE_BINARY_DIR}/release")
set(CPACK_GENERATOR "NSIS")

# Optional: NSIS UI and custom script
set(CPACK_NSIS_DISPLAY_NAME "CubeCoach Installer")
set(CPACK_NSIS_PACKAGE_NAME "CubeCoach")
set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
set(CPACK_NSIS_SCRIPT "${CMAKE_SOURCE_DIR}/CubeCoachInstaller.nsi")

# Install rules
install(TARGETS "${CMAKE_PROJECT_NAME}" RUNTIME DESTINATION bin)
install(DIRECTORY "${CMAKE_SOURCE_DIR}/resources/" DESTINATION resources)
install(FILES "${CMAKE_SOURCE_DIR}/LICENSE.txt" DESTINATION resources)

include(CPack)



