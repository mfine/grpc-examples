.PHONY: all clean

%.proto: ../%.proto
	cp $< .

%.pb.go: %.proto
	protoc $? --go_out=plugins=grpc:.

all: server

server: server.go examples.pb.go
	go build server.go examples.pb.go

clean:
	rm -f *.pb.go
