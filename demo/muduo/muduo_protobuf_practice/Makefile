all: server client
server:protobuf_server.cc request.pb.cc ../include/muduo/proto/codec.cc
	g++ -std=c++11 $^ -o $@ -I../include -L../lib -lmuduo_net -lmuduo_base -lprotobuf -lz
client:protobuf_client.cc request.pb.cc ../include/muduo/proto/codec.cc
	g++ -std=c++11 $^ -o $@ -I../include -L../lib -lmuduo_net -lmuduo_base -lprotobuf -lz

.PHONY:clean
clean:
	rm -f server client