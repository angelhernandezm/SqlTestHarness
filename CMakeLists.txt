cmake_minimum_required(VERSION 3.5)
project(SqlTestHarness)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

include(FindPkgConfig)

pkg_check_modules(GTK gtk+-3.0 REQUIRED)
pkg_check_modules(libglade-2.0 REQUIRED)

include_directories(${GTK_INCLUDE_DIRS})

set(SOURCE_FILES src/MainWindow.cpp src/MainWindowController.h src/MainWindowController.cpp src/BaseController.cpp src/BaseController.h src/ActionToolbar.h src/ActionToolbar.cpp src/CommonHeaders.h src/GenericCallback.h src/GenericCallback.h src/ContactModel.h src/DataContext.cpp src/DataContext.h src/Common.h src/SqlDal.cpp src/SqlDal.h src/SqlCommands.h)
add_executable(SqlTestHarness ${SOURCE_FILES} src/MainWindowController.h src/MainWindowController.cpp src/BaseController.cpp src/BaseController.h src/ActionToolbar.h src/ActionToolbar.cpp src/CommonHeaders.h src/GenericCallback.h src/GenericCallback.h src/ContactModel.h src/DataContext.cpp src/DataContext.h src/Common.h src/SqlDal.cpp src/SqlDal.h src/SqlCommands.h)

target_link_libraries(${PROJECT_NAME} ${GTK_LIBRARIES} odbc)
