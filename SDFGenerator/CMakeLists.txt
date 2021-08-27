cmake_minimum_required(VERSION 3.10)

set( TARGET_NAME SDFGenerator )

if(WIN32)
	add_compile_definitions(_UNICODE UNICODE)
endif()

set(
	SOURCE_FILES
	main.cpp
	stb_image.h
	stb_image_write.h
	sdf.h
	original.h
	original.cpp
)

add_executable(
	${TARGET_NAME} 
	${SOURCE_FILES}
)

target_include_directories( 
	${TARGET_NAME}
    PUBLIC ../ThirdParty/SDL2-2.0.14/include 
)

target_link_libraries( 
	${TARGET_NAME}
	../../ThirdParty/SDL2-2.0.14/lib/x64/SDL2
	../../ThirdParty/SDL2-2.0.14/lib/x64/SDL2main
)

set_target_properties(
	${TARGET_NAME}
	PROPERTIES
	PROJECT_LABEL "SDFGenerator"
)

# Set the startup project.
set_directory_properties( PROPERTIES 
	VS_STARTUP_PROJECT SDFGenerator
)


file(COPY ../ThirdParty/SDL2-2.0.14/lib/x64/SDL2.dll DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG})
file(COPY ../ThirdParty/SDL2-2.0.14/lib/x64/SDL2.dll DESTINATION ${CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE})








