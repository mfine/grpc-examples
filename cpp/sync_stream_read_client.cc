#include <grpcpp/create_channel.h>
#include <thread>

#include "examples.grpc.pb.h"

int main() {
  while (true) {
    auto chan(grpc::CreateChannel("localhost:6000", grpc::InsecureChannelCredentials()));
    auto stub(examples::ExamplesService::NewStub(chan));

    grpc::ClientContext context;
    examples::White white;
    examples::Red red;

    auto data(stub->StreamRead(&context, white));
    while (data->Read(&red)) {
      std::cout << "READ" << std::endl;
    }

    grpc::Status status(data->Finish());
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
