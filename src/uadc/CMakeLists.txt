project(dftiuadc)
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
  dftisensor
  dftiutil
)

add_executable(${APP_NAME}
  main.cc
)

target_link_libraries(${APP_NAME}
  Qt5::Core
  Qt5::SerialPort
  dftisensor
  dftiuadc
  dftiutil
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
