project(dftiutil)

set(SOURCES
   util.cc
)

set(HEADERS
   util.hh
)

add_library(${PROJECT_NAME} SHARED
   ${SOURCES}
   ${MOC_SRC}
)

target_link_libraries(${PROJECT_NAME}
  Qt5::Core
)

install(TARGETS ${PROJECT_NAME} DESTINATION ${dfti_TARGET_LIB_DIRECTORY})
