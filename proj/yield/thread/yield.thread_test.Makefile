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
	CXXFLAGS += /EHsc /GR- /MP /nologo /wd"4100" /wd"4127" /wd"4290" /wd"4355" /wd"4512"
endif
ifneq ($COVERAGE,)
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


ifeq ($(UNAME), FreeBSD)
	LIBS += -lpthread
endif
ifeq ($(UNAME), Linux)
	LIBS += -lpthread -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lcpc -lkstat -lm -lrt -lstdc++
endif
LIBS += -lyield_thread -lyield


DEP_FILE_PATHS := $(shell find ../../../build/yield/thread_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/thread_test/blocking_concurrent_queue_test.o ../../../build/yield/thread_test/condition_variable_test.o ../../../build/yield/thread_test/lightweight_mutex_test.o ../../../build/yield/thread_test/mutex_test.o ../../../build/yield/thread_test/non_blocking_concurrent_queue_test.o ../../../build/yield/thread_test/performance_counter_set_test.o ../../../build/yield/thread_test/pipe_test.o ../../../build/yield/thread_test/processor_set_test.o ../../../build/yield/thread_test/reader_writer_lock_test.o ../../../build/yield/thread_test/sampler_test.o ../../../build/yield/thread_test/semaphore_test.o ../../../build/yield/thread_test/synchronized_event_queue_test.o ../../../build/yield/thread_test/synchronized_queue_test.o ../../../build/yield/thread_test/synchronized_response_queue_test.o ../../../build/yield/thread_test/thread_test.o ../../../build/yield/thread_test/tls_concurrent_queue_test.o ../../../build/yield/thread_test/unit_concurrent_queue_test.o ../../../build/yield/thread_test/yield_thread_test_main.o


../../../bin/yield/yield_thread_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin/yield 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield/yield_thread_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/thread_test/blocking_concurrent_queue_test.o: ../../../test/yield/thread/blocking_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/blocking_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/blocking_concurrent_queue_test.cpp

../../../build/yield/thread_test/condition_variable_test.o: ../../../test/yield/thread/condition_variable_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/condition_variable_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/condition_variable_test.cpp

../../../build/yield/thread_test/lightweight_mutex_test.o: ../../../test/yield/thread/lightweight_mutex_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/lightweight_mutex_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/lightweight_mutex_test.cpp

../../../build/yield/thread_test/mutex_test.o: ../../../test/yield/thread/mutex_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/mutex_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/mutex_test.cpp

../../../build/yield/thread_test/non_blocking_concurrent_queue_test.o: ../../../test/yield/thread/non_blocking_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/non_blocking_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/non_blocking_concurrent_queue_test.cpp

../../../build/yield/thread_test/performance_counter_set_test.o: ../../../test/yield/thread/performance_counter_set_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/performance_counter_set_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/performance_counter_set_test.cpp

../../../build/yield/thread_test/pipe_test.o: ../../../test/yield/thread/pipe_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/pipe_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/pipe_test.cpp

../../../build/yield/thread_test/processor_set_test.o: ../../../test/yield/thread/processor_set_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/processor_set_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/processor_set_test.cpp

../../../build/yield/thread_test/reader_writer_lock_test.o: ../../../test/yield/thread/reader_writer_lock_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/reader_writer_lock_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/reader_writer_lock_test.cpp

../../../build/yield/thread_test/sampler_test.o: ../../../test/yield/thread/sampler_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/sampler_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/sampler_test.cpp

../../../build/yield/thread_test/semaphore_test.o: ../../../test/yield/thread/semaphore_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/semaphore_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/semaphore_test.cpp

../../../build/yield/thread_test/synchronized_event_queue_test.o: ../../../test/yield/thread/synchronized_event_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/synchronized_event_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/synchronized_event_queue_test.cpp

../../../build/yield/thread_test/synchronized_queue_test.o: ../../../test/yield/thread/synchronized_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/synchronized_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/synchronized_queue_test.cpp

../../../build/yield/thread_test/synchronized_response_queue_test.o: ../../../test/yield/thread/synchronized_response_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/synchronized_response_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/synchronized_response_queue_test.cpp

../../../build/yield/thread_test/thread_test.o: ../../../test/yield/thread/thread_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/thread_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/thread_test.cpp

../../../build/yield/thread_test/tls_concurrent_queue_test.o: ../../../test/yield/thread/tls_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/tls_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/tls_concurrent_queue_test.cpp

../../../build/yield/thread_test/unit_concurrent_queue_test.o: ../../../test/yield/thread/unit_concurrent_queue_test.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/unit_concurrent_queue_test.o -MD $(CXXFLAGS) ../../../test/yield/thread/unit_concurrent_queue_test.cpp

../../../build/yield/thread_test/yield_thread_test_main.o: ../../../test/yield/thread/yield_thread_test_main.cpp
	-mkdir -p ../../../build/yield/thread_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/thread_test/yield_thread_test_main.o -MD $(CXXFLAGS) ../../../test/yield/thread/yield_thread_test_main.cpp

