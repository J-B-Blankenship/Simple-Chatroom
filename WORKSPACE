workspace(name="renci_demo")

# Tool needed to do the below
load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# Fairly new version of Protocol Buffers
http_archive(
    name = "com_google_protobuf",
    sha256 = "3bd7828aa5af4b13b99c191e8b1e884ebfa9ad371b0ce264605d347f135d2568",
    urls = ["https://github.com/protocolbuffers/protobuf/archive/v3.19.4.tar.gz"],
    strip_prefix = "protobuf-3.19.4",
    )

# Fairly new version of gRPC
http_archive(
    name = "com_github_grpc_grpc",
    sha256 = "9647220c699cea4dafa92ec0917c25c7812be51a18143af047e20f3fb05adddc",
    urls = ["https://github.com/grpc/grpc/archive/v1.43.0.tar.gz"],
    strip_prefix = "grpc-1.43.0",
    )

load("@com_google_protobuf//:protobuf_deps.bzl", "protobuf_deps")
protobuf_deps()

# Additional gRPC setup
load("@com_github_grpc_grpc//bazel:grpc_deps.bzl", "grpc_deps", "grpc_test_only_deps")
grpc_deps()
load("@com_github_grpc_grpc//bazel:grpc_extra_deps.bzl", "grpc_extra_deps")
grpc_extra_deps()
    
