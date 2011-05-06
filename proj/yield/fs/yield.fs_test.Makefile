TIMESTAMP=$(shell date +%Y%m%dT%H%M%S)
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
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($(COVERAGE),)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../lib/yield
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Darwin)
	LIBS += -liconv
endif
ifeq ($(UNAME), FreeBSD)
	LIBS += -liconv -lintl
endif
ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -liconv -lm -lrt -lstdc++
endif
LIBS += -lyield_fs -lyield_i18n -lyield


D_FILE_PATHS := $(shell find ../../../build/yield/fs_test -name "*.d")


O_FILE_PATHS += ../../../build/yield/fs_test/directory_test.o ../../../build/yield/fs_test/file_log_test.o ../../../build/yield/fs_test/named_pipe_test.o ../../../build/yield/fs_test/path_test.o ../../../build/yield/fs_test/yield_fs_test_main.o
ifeq ($(UNAME), Darwin)
	O_FILE_PATHS += ../../../build/yield/fs_test/posix/file_system_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o
endif
ifeq ($(UNAME), FreeBSD)
	O_FILE_PATHS += ../../../build/yield/fs_test/posix/file_system_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o
endif
ifeq ($(UNAME), Linux)
	O_FILE_PATHS += ../../../build/yield/fs_test/posix/file_system_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o
endif
ifeq ($(UNAME), Solaris)
	O_FILE_PATHS += ../../../build/yield/fs_test/posix/file_system_test.o ../../../build/yield/fs_test/posix/file_test.o ../../../build/yield/fs_test/posix/stat_test.o
endif
ifeq ($(UNAME), MINGW32)
	O_FILE_PATHS += ../../../build/yield/fs_test/win32/file_system_test.o ../../../build/yield/fs_test/win32/file_test.o ../../../build/yield/fs_test/win32/stat_test.o
endif


all: ../../../bin/yield/yield_fs_test

clean:
	$(RM) ../../../bin/yield/yield_fs_test $(O_FILE_PATHS)

depclean:
	$(RM) $(D_FILE_PATHS)

-include $(D_FILE_PATHS)
			
lcov: ../../../bin/yield/yield_fs_test
	lcov --directory ../../../build/yield/fs_test --zerocounters
	-../../../bin/yield/yield_fs_test
	lcov --base-directory . --directory ../../../build/yield/fs_test --capture --output-file yield.fs_test_lcov-$(TIMESTAMP)
	mkdir yield.fs_test_lcov_html-$(TIMESTAMP)
	genhtml -o yield.fs_test_lcov_html-$(TIMESTAMP) yield.fs_test_lcov-$(TIMESTAMP)
	-cp -R yield.fs_test_lcov_html-$(TIMESTAMP) /mnt/hgfs/minorg/Desktop
	zip -qr yield.fs_test_lcov_html-$(TIMESTAMP).zip yield.fs_test_lcov_html-$(TIMESTAMP)/*
	rm -fr yield.fs_test_lcov_html-$(TIMESTAMP)


../../../lib/yield/libyield_fs.a:
	$(MAKE) -C . yield.fs.Makefile


../../../bin/yield/yield_fs_test: $(O_FILE_PATHS) ../../../lib/yield/libyield_fs.a
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(O_FILE_PATHS) -o $@ $(LIBS)

../../../build/yield/fs_test/directory_test.o: ../../../test/yield/fs/directory_test.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/directory_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/directory_test.cpp

../../../build/yield/fs_test/file_log_test.o: ../../../test/yield/fs/file_log_test.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/file_log_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/file_log_test.cpp

../../../build/yield/fs_test/named_pipe_test.o: ../../../test/yield/fs/named_pipe_test.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/named_pipe_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/named_pipe_test.cpp

../../../build/yield/fs_test/path_test.o: ../../../test/yield/fs/path_test.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/path_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/path_test.cpp

../../../build/yield/fs_test/posix/file_system_test.o: ../../../test/yield/fs/posix/file_system_test.cpp
	-mkdir -p ../../../build/yield/fs_test/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/posix/file_system_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/posix/file_system_test.cpp

../../../build/yield/fs_test/posix/file_test.o: ../../../test/yield/fs/posix/file_test.cpp
	-mkdir -p ../../../build/yield/fs_test/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/posix/file_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/posix/file_test.cpp

../../../build/yield/fs_test/posix/stat_test.o: ../../../test/yield/fs/posix/stat_test.cpp
	-mkdir -p ../../../build/yield/fs_test/posix 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/posix/stat_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/posix/stat_test.cpp

../../../build/yield/fs_test/win32/file_system_test.o: ../../../test/yield/fs/win32/file_system_test.cpp
	-mkdir -p ../../../build/yield/fs_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/win32/file_system_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/win32/file_system_test.cpp

../../../build/yield/fs_test/win32/file_test.o: ../../../test/yield/fs/win32/file_test.cpp
	-mkdir -p ../../../build/yield/fs_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/win32/file_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/win32/file_test.cpp

../../../build/yield/fs_test/win32/stat_test.o: ../../../test/yield/fs/win32/stat_test.cpp
	-mkdir -p ../../../build/yield/fs_test/win32 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/win32/stat_test.o -MD $(CXXFLAGS) ../../../test/yield/fs/win32/stat_test.cpp

../../../build/yield/fs_test/yield_fs_test_main.o: ../../../test/yield/fs/yield_fs_test_main.cpp
	-mkdir -p ../../../build/yield/fs_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/fs_test/yield_fs_test_main.o -MD $(CXXFLAGS) ../../../test/yield/fs/yield_fs_test_main.cpp
