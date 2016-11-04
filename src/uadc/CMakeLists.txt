project(uadc)
set(APP_NAME uadc_test)

set(SOURCES
  uadc.cc
)

set(HEADERS
  uadc.hh
)

add_library(${PROJECT_NAME} SHARED
   ${SOURCES}
   ${MOC_SRC}
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Core
  Qt5::SerialPort
  dftiutil
)

add_executable(${APP_NAME}
  main.cc
  ${SOURCES}
  ${MOC_SRC}
)

target_link_libraries(${APP_NAME}
  Qt5::Core
  Qt5::SerialPort
  dftiutil
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
