cmake_minimum_required(VERSION 2.8)
#Déclaration du projet

project(ht-caen)

set (O_FLAG "-O2")
set (CFLAGS "${O_FLAG} -Wall -D LINUX") 
#-D HAVE_PF_RING_ZC")
set (CMAKE_CXX_FLAGS "-std=c++11 ${CFLAGS} -Wextra -Wno-unused-parameter -ggdb -g -pthread")
#set (EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)
#set (Libraries "pfring pthread")


find_package(Boost 1.56 REQUIRED COMPONENTS system iostreams)
include_directories(${Boost_INCLUDE_DIRS})


set (USERLIB tinyxml2)

include_directories(./include/ ../include  ${LIB_SHM_PATH}/include  ${LIB_DECODEFRAME_PATH}/include /usr/local/include)


file (
		GLOB_RECURSE

	source_files
	src/*.c
	src/*.cpp
        
	include/*.h     
	../include/*.h
)

#Déclaration de l'exécutable

add_executable(
        ht-caen
        ${source_files}

)

target_link_libraries(ht-caen PUBLIC pthread CAENVME rt  ${USERLIB} Boost::system Boost::iostreams)



