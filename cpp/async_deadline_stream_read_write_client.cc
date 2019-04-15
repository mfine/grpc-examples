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

    grpc::CompletionQueue cq;
    uint64_t tag_in(0);
    uint64_t *tag_out;
    bool ok;

    tag_in++;
    auto data(stub->AsyncStreamReadWrite(&context, &cq, &tag_in));

    ok = false;
    if (!cq.Next(reinterpret_cast<void **>(&tag_out), &ok)) {
      std::cout << "BAD NEXT 1" << std::endl;
      assert(false);
    }

    bool timeout(false);

    if (ok) {
      assert(tag_in == *tag_out);
    }

    std::cout << "SLEEP" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return 0;
}
