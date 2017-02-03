project(dftivn200)

set(SOURCES
  vn200.cc
)

set(HEADERS
  vn200.hh
)

add_library(${PROJECT_NAME} SHARED
   ${SOURCES}
   ${MOC_SRC}
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Core
  Qt5::SerialPort
  dftisensor
  dftisettings
  dftiutil
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${dfti_TARGET_LIB_DIRECTORY})
