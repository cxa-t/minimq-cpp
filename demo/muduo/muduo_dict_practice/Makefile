all:dict_server dict_client
dict_server:dict_server.cc
	g++ -std=c++11 $^ -o $@ -I./include -L./lib -lmuduo_net -lmuduo_base

dict_client:dict_client.cc
	g++ -std=c++11 $^ -o $@ -I./include -L./lib -lmuduo_net -lmuduo_base

.PHONY:clean
clean:
	rm -f dict_server dict_client