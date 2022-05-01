cmake_minimum_required(VERSION 2.8.12)
project(vtk-DataStructure)

# Set "vtk-config.cmake" file directory, So that 'find_package' finds VTK
set(VTK_DIR "D:/install/vtk-9.1.0.rc4-vc16-static-debug/lib/cmake/vtk-9.1")

#
# Find VTK
#
find_package(VTK
COMPONENTS
  vtkCommonColor
  vtkCommonCore
  vtkCommonDataModel
  vtkCommonTransforms
  vtkIOXML
	#渲染
  vtkInteractionStyle
  vtkRenderingContextOpenGL2
  vtkRenderingCore
  vtkRenderingFreeType
  vtkRenderingGL2PSOpenGL2
  vtkRenderingOpenGL2
 	#image
   vtkImagingCore
   vtkImagingSources
   vtkImagingMath
   vtkInteractionImage
 QUIET)
if (NOT VTK_FOUND)
  message("Fatal Find VTK: ${VTK_NOT_FOUND_MESSAGE}")
  return ()
endif()
message (STATUS "VTK_VERSION: ${VTK_VERSION}")

#
# 项目配置
# 

#工程公共头文件
set(PROJECT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)  #工程头文件目录
include_directories(${PROJECT_INCLUDE_DIR})  #添加include目录（全局）
file(GLOB_RECURSE Project_Headers_Files ${PROJECT_INCLUDE_DIR}/*.h ${PROJECT_INCLUDE_DIR}/*.hpp)

#
# 添加
# 

set(Project_Src_Dir ${CMAKE_SOURCE_DIR}/src)
file(GLOB_RECURSE files "${Project_Src_Dir}/*.cpp" "${Project_Src_Dir}/*.cxx")
foreach(file ${files})
  #工程名
  get_filename_component(name ${file} NAME_WE)
  #工程文件夹
  STRING(REGEX REPLACE "${Project_Src_Dir}(.+)/.+" "\\1" DIR ${file})

  #创建工程 
  add_executable(${name} MACOSX_BUNDLE ${file} ${Project_Headers_Files})

  # Add a reference to VTK for the executable
  if (VTK_VERSION VERSION_LESS "8.90.0")
    # old system
    include(${VTK_USE_FILE})
    target_link_libraries(${name} PRIVATE ${VTK_LIBRARIES})
  else()
    # Prevent a "command line is too long" failure in Windows.
    set(CMAKE_NINJA_FORCE_RESPONSE_FILE "ON" CACHE BOOL "Force Ninja to use response files.")
    target_link_libraries(${name} PRIVATE ${VTK_LIBRARIES})
    # vtk_module_autoinit is needed
    vtk_module_autoinit(
      TARGETS ${name}
      MODULES ${VTK_LIBRARIES}
    )
  endif()

  # 子工程的文件夹
  set_property(TARGET ${name} PROPERTY FOLDER ${DIR})
  # 代码分组
  source_group(TREE ${CMAKE_SOURCE_DIR} FILES ${Project_Headers_Files})
endforeach()

#
# 其他配置
#

# 开启工程分组
set_property(GLOBAL PROPERTY USE_FOLDERS ON)