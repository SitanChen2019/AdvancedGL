CMAKE_MINIMUM_REQUIRED(VERSION 3.12)
project( PDBSim LANGUAGES  CXX )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


file(GLOB PBD_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h   )
file(GLOB PBD_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )

add_library(PBDSim SHARED ${PBD_HEAD} ${PBD_SRC})
target_include_directories( PBDSim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include )
target_include_directories( PBDSim PRIVATE ${THIRD_PARTY_ROOT}/glm/include  ${THIRD_PARTY_ROOT}/boost-1.71/include  )