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
ifneq ($COVERAGE,)
	CXXFLAGS += -fprofile-arcs -ftest-coverage
	LDFLAGS += -fprofile-arcs -ftest-coverage -lgcov
endif
ifneq ($(RELEASE),)
	CXXFLAGS += -O2
else
	CXXFLAGS += -g -D_DEBUG
endif


LDFLAGS += -L../../../lib
ifeq ($(UNAME), MINGW32)
	LDFLAGS += /ignore:4006 /ignore:4221
endif


ifeq ($(UNAME), Linux)
	LIBS += -lrt -lstdc++
endif
ifeq ($(UNAME), Solaris)
	LIBS += -lm -lrt -lstdc++
endif
LIBS += -lyield_marshal -lyield_common


DEP_FILE_PATHS := $(shell find ../../../build/yield/marshal_test -name "*.d")


OBJECT_FILE_PATHS += ../../../build/yield/marshal_test/json/json_marshaller_test.o ../../../build/yield/marshal_test/pretty_printer_test.o ../../../build/yield/marshal_test/xdr/xdr_marshaller_test.o ../../../build/yield/marshal_test/xml/xml_marshaller_test.o ../../../build/yield/marshal_test/yield_marshal_test_main.o


../../../bin/yield_marshal_test: $(OBJECT_FILE_PATHS)
	-mkdir -p ../../../bin 2>/dev/null
	$(LINK.cpp) $(OBJECT_FILE_PATHS) -o $@ $(LIBS)

clean:
	$(RM) ../../../bin/yield_marshal_test $(OBJECT_FILE_PATHS)

depclean:
	$(RM) $(DEP_FILE_PATHS)

-include $(DEP_FILE_PATHS)


../../../build/yield/marshal_test/json/json_marshaller_test.o: ../../../src/yield/marshal/json/json_marshaller_test.cpp
	-mkdir -p ../../../build/yield/marshal_test/json 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal_test/json/json_marshaller_test.o -MD $(CXXFLAGS) ../../../src/yield/marshal/json/json_marshaller_test.cpp

../../../build/yield/marshal_test/pretty_printer_test.o: ../../../src/yield/marshal/pretty_printer_test.cpp
	-mkdir -p ../../../build/yield/marshal_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal_test/pretty_printer_test.o -MD $(CXXFLAGS) ../../../src/yield/marshal/pretty_printer_test.cpp

../../../build/yield/marshal_test/xdr/xdr_marshaller_test.o: ../../../src/yield/marshal/xdr/xdr_marshaller_test.cpp
	-mkdir -p ../../../build/yield/marshal_test/xdr 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal_test/xdr/xdr_marshaller_test.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xdr/xdr_marshaller_test.cpp

../../../build/yield/marshal_test/xml/xml_marshaller_test.o: ../../../src/yield/marshal/xml/xml_marshaller_test.cpp
	-mkdir -p ../../../build/yield/marshal_test/xml 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal_test/xml/xml_marshaller_test.o -MD $(CXXFLAGS) ../../../src/yield/marshal/xml/xml_marshaller_test.cpp

../../../build/yield/marshal_test/yield_marshal_test_main.o: ../../../src/yield/marshal/yield_marshal_test_main.cpp
	-mkdir -p ../../../build/yield/marshal_test 2>/dev/null
	$(CXX) -c -o ../../../build/yield/marshal_test/yield_marshal_test_main.o -MD $(CXXFLAGS) ../../../src/yield/marshal/yield_marshal_test_main.cpp

