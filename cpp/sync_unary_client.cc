#include <grpcpp/create_channel.h>
#include <thread>

#include "examples.grpc.pb.h"

#define REUSE_CHANNEL

int main() {
#ifdef REUSE_CHANNEL
  std::cout << "reusing channel" << std::endl;
  auto chan(grpc::CreateChannel("localhost:6000", grpc::InsecureChannelCredentials()));
  auto stub(examples::ExamplesService::NewStub(chan));
  while (true) {
#else
  while (true) {
    auto chan(grpc::CreateChannel("localhost:6000", grpc::InsecureChannelCredentials()));
    auto stub(examples::ExamplesService::NewStub(chan));
#endif

    grpc::ClientContext context;
    examples::White white;
    examples::Red red;

    grpc::Status status(stub->Unary(&context, white, &red));
    if (status.ok()) {
      std::cout << "OK" << std::endl;
    } else {
      std::cout << "NOT OK " << std::to_string(status.error_code()) << std::endl;
    }

    std::cout << "PAUSE" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
