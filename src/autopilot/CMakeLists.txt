project(dftiap)
set(APP_NAME ap_test)

set(SOURCES
  autopilot.cc
)

set(HEADERS
  autopilot.hh
)

add_library(${PROJECT_NAME} SHARED
   ${SOURCES}
   ${MOC_SRC}
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Core
  Qt5::SerialPort
  dftisensor
  dftiutil
)

add_executable(${APP_NAME}
  autopilot_test.cc
)

target_link_libraries(${APP_NAME}
  Qt5::Core
  Qt5::SerialPort
  dftisensor
  dftiap
  dftiutil
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
