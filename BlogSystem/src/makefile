blogsystem:blog.cpp database.h
	g++ -std=c++11 $^ -o $@ -L/usr/lib64/mysql -ljsoncpp -lmysqlclient -lpthread  -lcrypto
#.PHONY:clean
clean:	
	rm -rf blogsystem
