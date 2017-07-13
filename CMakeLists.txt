cmake_minimum_required(VERSION  3.1.0)
project( rubius_test )

# переменная для отключения тестов. По умолчанию тесты включены
set ( TEST_DISABLE FALSE CACHE BOOL FALSE )

set (CMAKE_CXX_STANDARD 11)

# Qt
find_package(Qt4 REQUIRED QtCore QtGui QtNetwork)

# Настраиваем unit-тестирование, если тестирование не отключено
if( NOT TEST_DISABLE )
	# Unit-тесты
	enable_testing()
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS})
	set( GTEST_RESULT_ROOT ${CMAKE_CURRENT_BINARY_DIR}/Testing )
endif()

# Если не задан установочный префикс - будем ставить в директорию install сборочной директории.
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
	set (CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/install" CACHE PATH "default install path" FORCE )
endif()

# под-директория для установки бинарников
set( INSTALL_BINARY_SUBDIR . )

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/include )

add_subdirectory( client )
add_subdirectory( server )
add_subdirectory( msg )
