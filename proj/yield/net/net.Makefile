# SHELL = /bin/bash
UNAME := $(shell uname)

CXXFLAGS += -I../../../include
ifeq ($(UNAME), Linux)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), FreeBSD)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), Darwin)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), Solaris)
	CXXFLAGS += -fno-rtti -Wall -Wold-style-cast -Wunused-macros
endif
ifeq ($(UNAME), MINGW32)
	CXXFLAGS += /EHsc /GR- /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif


LDFLAGS += -L../../../lib
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++ -lnsl -lsocket -luuid
endif
LIBS += -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/net -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/net/uri.o ../../../build/yield/net/uuid.o ../../../build/yield/net/sockets/socket_pair.o ../../../build/yield/net/sockets/ssl/ssl_context.o ../../../build/yield/net/sockets/ssl/ssl_exception.o ../../../build/yield/net/sockets/ssl/ssl_socket.o
ifeq ($(UNAME), Darwin)
	OBJECT_FILE_PATHS += ../../../build/yield/net/sockets/posix/datagram_socket.o ../../../build/yield/net/sockets/posix/socket.o ../../../build/yield/net/sockets/posix/socket_address.o ../../../build/yield/net/sockets/posix/stream_socket.o ../../../build/yield/net/sockets/posix/tcp_socket.o ../../../build/yield/net/sockets/posix/udp_socket.o
endif
ifeq ($(UNAME), FreeBSD)
	OBJECT_FILE_PATHS += ../../../build/yield/net/sockets/posix/datagram_socket.o ../../../build/yield/net/sockets/posix/socket.o ../../../build/yield/net/sockets/posix/socket_address.o ../../../build/yield/net/sockets/posix/stream_socket.o ../../../build/yield/net/sockets/posix/tcp_socket.o ../../../build/yield/net/sockets/posix/udp_socket.o
endif
ifeq ($(UNAME), Linux)
	OBJECT_FILE_PATHS += ../../../build/yield/net/linux/uuid.o
	OBJECT_FILE_PATHS += ../../../build/yield/net/sockets/posix/datagram_socket.o ../../../build/yield/net/sockets/posix/socket.o ../../../build/yield/net/sockets/posix/socket_address.o ../../../build/yield/net/sockets/posix/stream_socket.o ../../../build/yield/net/sockets/posix/tcp_socket.o ../../../build/yield/net/sockets/posix/udp_socket.o
endif
ifeq ($(UNAME), MINGW32)
	OBJECT_FILE_PATHS += ../../../build/yield/net/win32/uuid.o ../../../build/yield/net/sockets/win32/datagram_socket.o ../../../build/yield/net/sockets/win32/socket.o ../../../build/yield/net/sockets/win32/socket_address.o ../../../build/yield/net/sockets/win32/stream_socket.o ../../../build/yield/net/sockets/win32/tcp_socket.o ../../../build/yield/net/sockets/win32/udp_socket.o
endif
ifeq ($(UNAME), Solaris)
	OBJECT_FILE_PATHS += ../../../build/yield/net/sockets/posix/datagram_socket.o ../../../build/yield/net/sockets/posix/socket.o ../../../build/yield/net/sockets/posix/socket_address.o ../../../build/yield/net/sockets/posix/stream_socket.o ../../../build/yield/net/sockets/posix/tcp_socket.o ../../../build/yield/net/sockets/posix/udp_socket.o
	OBJECT_FILE_PATHS += ../../../build/yield/net/sunos/uuid.o
endif


../../../lib/libyield_net.a: $(OBJECT_FILE_PATHS)
	$(AR) -r $@ $(OBJECT_FILE_PATHS)

clean:
	$(RM) ../../../lib/libyield_net.a $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/net/linux/uuid.o: ../../../src/yield/net/linux/uuid.cpp
	$(CXX) -c -o ../../../build/yield/net/linux/uuid.o -MD $(CXXFLAGS) ../../../src/yield/net/linux/uuid.cpp

../../../build/yield/net/sockets/posix/datagram_socket.o: ../../../src/yield/net/sockets/posix/datagram_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/posix/datagram_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/posix/datagram_socket.cpp

../../../build/yield/net/sockets/posix/socket.o: ../../../src/yield/net/sockets/posix/socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/posix/socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/posix/socket.cpp

../../../build/yield/net/sockets/posix/socket_address.o: ../../../src/yield/net/sockets/posix/socket_address.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/posix/socket_address.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/posix/socket_address.cpp

../../../build/yield/net/sockets/posix/stream_socket.o: ../../../src/yield/net/sockets/posix/stream_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/posix/stream_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/posix/stream_socket.cpp

../../../build/yield/net/sockets/posix/tcp_socket.o: ../../../src/yield/net/sockets/posix/tcp_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/posix/tcp_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/posix/tcp_socket.cpp

../../../build/yield/net/sockets/posix/udp_socket.o: ../../../src/yield/net/sockets/posix/udp_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/posix/udp_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/posix/udp_socket.cpp

../../../build/yield/net/sockets/socket_pair.o: ../../../src/yield/net/sockets/socket_pair.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/socket_pair.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/socket_pair.cpp

../../../build/yield/net/sockets/ssl/ssl_context.o: ../../../src/yield/net/sockets/ssl/ssl_context.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/ssl/ssl_context.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/ssl/ssl_context.cpp

../../../build/yield/net/sockets/ssl/ssl_exception.o: ../../../src/yield/net/sockets/ssl/ssl_exception.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/ssl/ssl_exception.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/ssl/ssl_exception.cpp

../../../build/yield/net/sockets/ssl/ssl_socket.o: ../../../src/yield/net/sockets/ssl/ssl_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/ssl/ssl_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/ssl/ssl_socket.cpp

../../../build/yield/net/sockets/win32/datagram_socket.o: ../../../src/yield/net/sockets/win32/datagram_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/win32/datagram_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/win32/datagram_socket.cpp

../../../build/yield/net/sockets/win32/socket.o: ../../../src/yield/net/sockets/win32/socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/win32/socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/win32/socket.cpp

../../../build/yield/net/sockets/win32/socket_address.o: ../../../src/yield/net/sockets/win32/socket_address.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/win32/socket_address.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/win32/socket_address.cpp

../../../build/yield/net/sockets/win32/stream_socket.o: ../../../src/yield/net/sockets/win32/stream_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/win32/stream_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/win32/stream_socket.cpp

../../../build/yield/net/sockets/win32/tcp_socket.o: ../../../src/yield/net/sockets/win32/tcp_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/win32/tcp_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/win32/tcp_socket.cpp

../../../build/yield/net/sockets/win32/udp_socket.o: ../../../src/yield/net/sockets/win32/udp_socket.cpp
	$(CXX) -c -o ../../../build/yield/net/sockets/win32/udp_socket.o -MD $(CXXFLAGS) ../../../src/yield/net/sockets/win32/udp_socket.cpp

../../../build/yield/net/sunos/uuid.o: ../../../src/yield/net/sunos/uuid.cpp
	$(CXX) -c -o ../../../build/yield/net/sunos/uuid.o -MD $(CXXFLAGS) ../../../src/yield/net/sunos/uuid.cpp

../../../build/yield/net/uri.o: ../../../src/yield/net/uri.cpp
	$(CXX) -c -o ../../../build/yield/net/uri.o -MD $(CXXFLAGS) ../../../src/yield/net/uri.cpp

../../../build/yield/net/uuid.o: ../../../src/yield/net/uuid.cpp
	$(CXX) -c -o ../../../build/yield/net/uuid.o -MD $(CXXFLAGS) ../../../src/yield/net/uuid.cpp

../../../build/yield/net/win32/uuid.o: ../../../src/yield/net/win32/uuid.cpp
	$(CXX) -c -o ../../../build/yield/net/win32/uuid.o -MD $(CXXFLAGS) ../../../src/yield/net/win32/uuid.cpp


