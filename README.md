# RENCI-Demo
This is the assessment written with gRPC, Bazel, and Protocol Buffers since the provided assessment did not compile. The executables for the chat room and client have been included in the repository. If you wish to build the code yourself, the following command will do the trick: `./bazelisk-linux-amd64 build //...`. To run, use the following `./bazelisk-linux-amd64 run //chat_room` followed by `./bazelisk-linux-amd64 run //client`.

The following should be noted about this repository.
* First and foremost this is a public repository. This allows easy access by all parties.
* All dependencies are included within the build process itself. There is no need to ensure cmake/make, gRPC, Protocol Buffers, or Bazel are included.
* The usual permission controls are set in place to prevent anyone from committing to this repository.
* Additional checks not put in place to keep from merging directly with master for simplicity.
* The source code can be built and run on both Windows and Linux

Additional information you may find that tickles your fancy. The build system used (Bazel) allows for multiple languages to be built or packaged appropriately. This 
allows for flexibility and adaptability that is hard to make work with cmake/make. If you are curious, a glance at the BUILD.bazel and WORKSPACE will show the
simplicity of the build system.

The original source code provided lacks scalability in two ways: type safety and utility of messages. Most organizations use either JSON or XML. Without going into the
nitty gritty, Protocol Buffers allows backwards compatability with older versions of the same message as well as expansion of a message. It is both faster and more 
compact than both JSON and XML with explicit built to purpose for over the wire communications. Serialization is built-in. Type safety is a bonus due to having to 
define the data schematic. Protocol buffers is also available in a number of languages and acts as standardization of the representation of data between languages.

My final note is about gRPC. boost::asio is a very low level abstraction. It is not much better than C sockets to be frank. For some background, I have overhauled the 
entire communication layer for two companies across four different projects for stability, standardization, scalability, and type safety. gRPC is a higher level 
abstraction that handles the establishment of the sockets, passing of data, serialization, deserialization, and redundancy. Errors do not automatically crash the 
server. Timeouts, responses, and connection establishment are handled too. There is the ability to add additional layers of security with encrypted channels.
