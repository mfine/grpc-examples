package main

import (
	"context"
	"io"
	"log"
	"net"
	"os"
	"time"

	"google.golang.org/grpc"
)

type service struct {
}

func (s *service) Unary(ctx context.Context, white *White) (red *Red, err error) {
	red = &Red{}
	return
}

func (s *service) StreamRead(white *White, stream ExamplesService_StreamReadServer) (err error) {
	ticker := time.NewTicker(time.Second)
	defer ticker.Stop()

	for {
		select {
		case <-ticker.C:
			if _, err = os.Stat("/tmp/STREAM_READ"); os.IsNotExist(err) {
				red := &Red{}
				if err = stream.Send(red); err != nil {
					return
				}
			}
		}
	}
	return
}

func (s *service) StreamWrite(stream ExamplesService_StreamWriteServer) (err error) {
	for {
		_, err := stream.Recv()
		if err == io.EOF {
			red := &Red{}
			return stream.SendAndClose(red)
		}
		if err != nil {
			return err
		}
	}

	return
}

func (s *service) StreamReadWrite(stream ExamplesService_StreamReadWriteServer) (err error) {
	for {
		_, err = stream.Recv()
		if err == io.EOF {
			err = nil
			return
		}
		if err != nil {
			return err
		}

		red := &Red{}
		if err = stream.Send(red); err != nil {
			return
		}
	}
	return
}

func main() {
	l, err := net.Listen("tcp", "localhost:6000")
	if err != nil {
		log.Panicln(err)
	}
	defer l.Close()

	s := grpc.NewServer()
	RegisterExamplesServiceServer(s, &service{})
	s.Serve(l)
}
