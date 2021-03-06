file(GLOB sources_Networking networking/*.cpp networking/*.h)
file(GLOB sources_Session session/*.cpp session/*.h)
file(GLOB sources_Command command/*.cpp command/*.h)
file(GLOB sources_localdir *.cpp *.h)

set(client_SRCS
  ${sources_Session}
  ${sources_Networking}
  ${sources_Command}
  ${sources_localdir}
)

include_directories(
  ${CMAKE_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/../shared
  ${CMAKE_CURRENT_SOURCE_DIR}/session
  ${CMAKE_CURRENT_SOURCE_DIR}/networking
  ${CMAKE_CURRENT_SOURCE_DIR}/command
  ${OPENSSL_INCLUDE_DIR}
  ${GTK3_INCLUDE_DIRS}
  ${Confuse_INCLUDE_DIRS}
  ${TINYXML_INCLUDE_DIRS}
  ${Glib_PKGCONF_INCLUDE_DIRS}
)

set(client_LINK_FLAGS "")

add_executable(client
  ${client_SRCS}
)

add_dependencies(client revision.h)

if( NOT WIN32 )
  add_definitions(-D_CLIENT_CONFIG="${CONF_DIR}/client.conf")
endif()

if( UNIX )
  set(client_LINK_FLAGS "-pthread ${client_LINK_FLAGS}")
endif()

if( CMAKE_SYSTEM_NAME MATCHES "Darwin" )
  set(client_LINK_FLAGS "-framework Carbon ${client_LINK_FLAGS}")
endif()

set_target_properties(client PROPERTIES LINK_FLAGS "${client_LINK_FLAGS}")

target_link_libraries(client    
  shared_clie
  ${OPENSSL_LIBRARIES}
  ${OPENSSL_EXTRA_LIBRARIES}
  ${GTK3_LIBRARIES}  
  ${Glib_PKGCONF_LIBRARY_DIRS}
  ${Confuse_LIBRARIES}
  ${TINYXML_LIBRARIES}
  ${OSX_LIBS}
)

if( WIN32 )
  add_custom_command(TARGET client
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/data/psc-client.conf.dist ${CMAKE_BINARY_DIR}/bin/$(ConfigurationName)/
  )
endif()

if( UNIX )
  install(TARGETS client DESTINATION bin)
  install(FILES  ${CMAKE_SOURCE_DIR}/data/psc.png DESTINATION bin/data/)
  install(FILES  ${CMAKE_SOURCE_DIR}/data/psc-client.conf.dist DESTINATION etc)
  install(FILES  ${CMAKE_SOURCE_DIR}/data/psc.png DESTINATION etc)
    
  install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/keys/
    DESTINATION etc/keys
    FILES_MATCHING
    PATTERN "*.pem"
    PATTERN "server" EXCLUDE
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

  install(DIRECTORY ${CMAKE_SOURCE_DIR}/data/keys/
    DESTINATION etc/keys
    FILES_MATCHING
    PATTERN "*.pub"
    PATTERN "server" EXCLUDE
    PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
  
  
elseif( WIN32 )
  install(TARGETS client DESTINATION "${CMAKE_INSTALL_PREFIX}")
  install(FILES ${CMAKE_SOURCE_DIR}/data/psc-client.conf.dist DESTINATION "${CMAKE_INSTALL_PREFIX}")
endif()

