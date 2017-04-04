project(dftitest)

add_executable(${PROJECT_NAME}
  dfti_test.cc
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Core
  Qt5::SerialPort
  dftisensor
  dftisettings
  dftiap
  dftieffectors
  dftiuadc
  dftiutil
  dftivn200
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
