cmake_minimum_required(VERSION 3.5.1)

project(RNBSS C CXX)

include(common.cmake)

# Proto file
get_filename_component(hw_proto "hafs.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

# Generated sources
set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/hafs.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/hafs.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/hafs.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/hafs.grpc.pb.h")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
set(CMAKE_BUILD_TYPE Debug)
add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}"
      DEPENDS "${hw_proto}")

# Include generated *.pb.h files
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

# hw_grpc_proto
add_library(hw_grpc_proto
  ${hw_grpc_srcs}
  ${hw_grpc_hdrs}
  ${hw_proto_srcs}
  ${hw_proto_hdrs})
target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

# Pending:- Add addition Lib Files

add_executable(server "server.cc")
target_link_libraries(server 
  ${PTHREAD_LIB} hw_grpc_proto
  ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

add_executable(basic_read_write "basic_read_write.cc")
  target_link_libraries(basic_read_write 
    ${PTHREAD_LIB} hw_grpc_proto
    ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

add_executable(status_checker "status_checker.cc")
  target_link_libraries(status_checker 
    ${PTHREAD_LIB} hw_grpc_proto
    ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

add_executable(ConsistancyTest "ConsistancyTest.cc")
  target_link_libraries(ConsistancyTest 
    ${PTHREAD_LIB} hw_grpc_proto
    ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})

add_executable(MultiConsistancyTest "MultiConsistancyTest.cc")
  target_link_libraries(MultiConsistancyTest 
    ${PTHREAD_LIB} hw_grpc_proto
    ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})


add_executable(recovery_testing "recovery_testing.cc")
  target_link_libraries(recovery_testing 
    ${PTHREAD_LIB} hw_grpc_proto
    ${_REFLECTION} ${_GRPC_GRPCPP} ${_PROTOBUF_LIBPROTOBUF})
  