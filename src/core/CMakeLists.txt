project(dfti)

set(SOURCES
  logger.cc
)

set(HEADERS
  consts.hh
  logger.hh
)

add_executable(${PROJECT_NAME}
  main.cc
  ${SOURCES}
  ${MOC_SRC}
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Core
  dftiap
  dftirio
  dftisensor
  dftiserver
  dftisettings
  dftiuadc
  dftiutil
  dftivn200
)

install(TARGETS ${PROJECT_NAME} DESTINATION bin)
