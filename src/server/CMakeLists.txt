project(dftiserver)

set(SOURCES
  server.cc
)

set(HEADERS
  server.hh
)

add_library(${PROJECT_NAME} SHARED
   ${SOURCES}
   ${MOC_SRC}
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Core
  Qt5::Network
  Qt5::SerialPort
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${dfti_TARGET_LIB_DIRECTORY})
