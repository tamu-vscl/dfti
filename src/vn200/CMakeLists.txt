project(dftivn200)
set(APP_NAME vn200_test)

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

add_executable(${APP_NAME}
  vn200_test.cc
)

target_link_libraries(${APP_NAME}
  Qt5::Core
  Qt5::SerialPort
  dftisensor
  dftisettings
  dftivn200
  dftiutil
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
