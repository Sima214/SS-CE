cmake_minimum_required(VERSION 3.8)

## Use the variable PROJECT_NAME for changing the target name
set( PROJECT_NAME "dummy" )

## Set our project name
project(${PROJECT_NAME})

## Use all the *.cpp files we found under this folder for the project
FILE(GLOB SRCS "*.c")

## Define the executable
add_executable(${PROJECT_NAME} ${SRCS})

target_link_libraries( ${PROJECT_NAME} )