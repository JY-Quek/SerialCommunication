add_library(SerialCommunication)

target_sources(SerialCommunication
  PRIVATE
    src/SerialCommunication.cpp
  PUBLIC
    src/SerialCommunication.h
  )


target_include_directories(SerialCommunication
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/src
  )
