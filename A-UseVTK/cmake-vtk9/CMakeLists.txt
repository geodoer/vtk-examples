cmake_minimum_required(VERSION 3.3 FATAL_ERROR)
project(vtk-cmake)
set(TARGET_NAME ${PROJECT_NAME})

# Set "vtk-config.cmake" file directory, So that 'find_package' finds VTK
set(VTK_DIR "D:/install/vtk-9.0.1ds/lib/cmake/vtk-9.0")

#
# Find VTK
#
find_package(VTK 
COMPONENTS 
  CommonCore
  FiltersSources
  FiltersSources
  InteractionStyle
  RenderingOpenGL2
QUIET)
if (NOT VTK_FOUND)
  message("Fatal Find VTK: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")

#
# Collect Code Files
#
file (GLOB Headers  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)
file (GLOB Sources  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

#
# Create an executable program 
#
add_executable(${TARGET_NAME} ${Headers} ${Sources})

# 
# Add a reference to VTK for the executable
#
target_link_libraries(${TARGET_NAME}
	PRIVATE 
		${VTK_LIBRARIES}
)
vtk_module_autoinit(
    TARGETS ${TARGET_NAME}
    MODULES ${VTK_LIBRARIES}
)
