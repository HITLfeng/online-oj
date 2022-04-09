
oj_server:oj_server.cc 
	g++ oj_server.cc -o oj_server -std=c++11 -lpthread \
		-ljsoncpp -lctemplate -I ~/third_part/include \
		-L ~/third_part/lib 

com:com.cc
	g++ -o $@ $^ -lpthread -std=c++11 -ljsoncpp

.PHONY:clean
clean:
	rm com oj_server


#src=$(wildcard *.cc)
#target=$(patsubst %.cc,%,$(src))
#
#LIB=-std=c++11 -lpthread
#
#.PHONY:all clean
#all:$(target)
#
#%:%.cc
#	g++ -o $@ $< $(LIB)
#
#clean:
#	rm $(target)
