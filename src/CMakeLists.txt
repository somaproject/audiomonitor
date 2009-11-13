link_directories(
  ${Boost_LIBRARY_DIRS}
  ${GUIDEPS_LIBRARY_DIRS}
  ${SOMADEPS_LIBRARY_DIRS}
)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GUIDEPS_INCLUDE_DIRS})
include_directories(${SOMADEPS_INCLUDE_DIRS})
#include_directories(logging/src)
#include_directories(logging/boost)

set(monitor_SRCS
   test.cc
)

set(logging_SRCS
  logging.cc
  ../logging-ext/src/logging.cc
)

add_library(logging
  ${logging_SRCS}
)


add_executable(soma_monitor
  ${monitor_SRCS}
  #  ${ext_SRCS}
  #${logging_SRCS}
)


target_link_libraries(soma_monitor
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}

)



add_executable(test3
	test3.cc
)


target_link_libraries(test3
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}

)



add_executable(test4
	test4.cc
)


target_link_libraries(test4
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}
)


add_executable(testjack
  testjack.cc
)


target_link_libraries(testjack
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}

)


add_executable(jacksink
  jacksinktest.cc
)


target_link_libraries(jacksink
    sinks
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}
)


add_library(sinks
   jacksink.cc
   pulsesink.cc
)


target_link_libraries(sinks
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}
)


add_executable(pulsesinktest
    pulsesinktest.cc
)


target_link_libraries(pulsesinktest
    sinks
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}
)

add_library(core
  audiomon.cc

)


target_link_libraries(core
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}
)



add_executable(audiomoncli
  audiomoncli.cc

)


target_link_libraries(audiomoncli
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}
    core
    sinks	
    logging
)



add_executable(audiomonitor
  main.cc
  audiomongui.cc
)


target_link_libraries(audiomonitor
    ${Boost_LIBRARIES} 
    ${GUIDEPS_LIBRARIES}
    ${SOMADEPS_LIBRARIES}
    core
    sinks	
    logging
)
