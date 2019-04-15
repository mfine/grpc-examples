#include <grpcpp/create_channel.h>
#include <thread>

#include "examples.grpc.pb.h"

#define REUSE_CHANNEL

int main() {
#ifdef REUSE_CHANNEL
  std::cout << "reusing channel" << std::endl;
  auto chan(grpc::CreateChannel("localhost:6000", grpc::InsecureChannelCredentials()));
  auto stub(examples::ExamplesService::NewStub(chan));
  grpc::CompletionQueue cq;
  uint64_t tag_in(0);
  while (true) {
#else
  while (true) {
    auto chan(grpc::CreateChannel("localhost:6000", grpc::InsecureChannelCredentials()));
    auto stub(examples::ExamplesService::NewStub(chan));
    grpc::CompletionQueue cq;
    uint64_t tag_in(0);
#endif

    grpc::ClientContext context;
    examples::White white;
    examples::Red red;

    bool ok;
    uint64_t *tag_out;

    tag_in++;
    auto data(stub->AsyncStreamRead(&context, white, &cq, &tag_in));

    ok = false;
    if (!cq.Next(reinterpret_cast<void **>(&tag_out), &ok)) {
      std::cout << "BAD NEXT 1" << std::endl;
      assert(false);
    }

    if (ok) {
      assert(tag_in == *tag_out);
      while (true) {
        tag_in++;
        data->Read(&red, &tag_in);

        ok = false;
        if (!cq.Next(reinterpret_cast<void **>(&tag_out), &ok)) {
          std::cout << "BAD NEXT 2" << std::endl;
          assert(false);
        }

        if (!ok) {
          std::cout << "BAD READ" << std::endl;
          break;
        }

        assert(tag_in == *tag_out);
        std::cout << "READ" << std::endl;
      }
    }

    grpc::Status status;

    tag_in++;
    data->Finish(&status, &tag_in);

    ok = false;
    if (!cq.Next(reinterpret_cast<void **>(&tag_out), &ok)) {
      std::cout << "BAD NEXT 3" << std::endl;
      assert(false);
    }

    if (!ok) {
      std::cout << "BAD STATUS" << std::endl;
      assert(false);
    }

    if (status.ok()) {
      std::cout << "OK" << std::endl;
    } else {
      std::cout << "NOT OK " << std::to_string(status.error_code()) << std::endl;
    }

    std::cout << "SLEEP" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
  }

  return 0;
}
