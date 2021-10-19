cmake_minimum_required(VERSION 3.0)
project(OpenGLPrj)

SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/Debug)
SET( CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_BINARY_DIR}/bin/Release)
option(GLFW_BUILD_DOCS OFF)
option(GLFW_BUILD_EXAMPLES OFF)
option(GLFW_BUILD_TESTS OFF)

add_subdirectory(vendor/glfw)
add_subdirectory(vendor/freetype vendor/freetype/build)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
    if(NOT WIN32)
        set(GLAD_LIBRARIES dl)
    endif()
endif()

include_directories(include/
                    vendor/glad/include/
                    vendor/glfw/include/
                    vendor/glm/
                    vendor/stb/
		    vendor/freetype/include/
		    vendor/irrKlang/include)

file(GLOB VENDORS_SOURCES vendor/glad/src/glad.c)
file(GLOB PROJECT_HEADERS include/*.hpp
			  include/*.h)

file(GLOB PROJECT_SOURCES src/*.cpp)
file(GLOB PROJECT_SHADERS shaders/*.comp
                          shaders/*.frag
                          shaders/*.geom
                          shaders/*.vert
                          shaders/*.vs
                          shaders/*.fs
                          )
file(GLOB PROJECT_CONFIGS CMakeLists.txt
                          Readme.md
                         .gitattributes
                         .gitignore
                         .gitmodules)

source_group("include" FILES ${PROJECT_HEADERS})
source_group("shaders" FILES ${PROJECT_SHADERS})
source_group("src" FILES ${PROJECT_SOURCES})
source_group("vendors" FILES ${VENDORS_SOURCES})

add_definitions(-DGLFW_INCLUDE_NONE
                -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES} ${PROJECT_HEADERS}
                               ${PROJECT_SHADERS} ${PROJECT_CONFIGS}
                               ${VENDORS_SOURCES})

target_link_libraries(${PROJECT_NAME} glfw ${GLFW_LIBRARIES} ${GLAD_LIBRARIES} freetype ${CMAKE_SOURCE_DIR}/vendor/irrKlang/lib/Winx64-visualStudio/irrKlang.lib)
file(COPY ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/winx64-visualStudio/ikpFlac.dll
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/Debug)

file(COPY ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/winx64-visualStudio/ikpMP3.dll
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/Debug)

file(COPY ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/winx64-visualStudio/irrKlang.dll
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/Debug)

file(COPY ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/winx64-visualStudio/ikpFlac.dll
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/Release)

file(COPY ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/winx64-visualStudio/ikpMP3.dll
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/Release)

file(COPY ${CMAKE_SOURCE_DIR}/vendor/irrKlang/bin/winx64-visualStudio/irrKlang.dll
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/Release)

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})
