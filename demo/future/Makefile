thread_pool:threadpool.cc
	g++ -std=c++11 $^ -o $@ -lpthread

package_task:package_task.cc
	g++ -std=c++11 $^ -o $@ -lpthread
promise:promise.cc
	g++ -std=c++11 $^ -o $@ -lpthread
async:async.cc
	g++ -std=c++11 $^ -o $@ -lpthread

.PHONY:clean
clean:
	rm -f package_task promise async thread_pool