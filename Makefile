#   Project mapc
#   Copyright (C) 2019 tania@df9ry.de
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as
#   published by the Free Software Foundation, either version 3 of the
#   License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
ifeq (,$(filter _%,$(notdir $(CURDIR))))
include target.mk
else

VPATH    =  $(SRCDIR)
CFLAGS   =  -Wall -Werror -g -ggdb -fpic -fmessage-length=0 -pthread

OBJS     =  mapc.o
LIBS     =  -lpthread
TARGET   =  libmapc.$(SOEXT)

all: $(TARGET)
	echo "Build OK"

$(TARGET): $(OBJS)
	$(CC) -shared $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
	
%.o: %.c $(SRCDIR)
	$(CC) -shared $(CFLAGS) -c $<	
	
doc:
	doxygen ../doxygen.conf
	( cd ../$(DOCDIR)/latex && make )
	cp ../$(DOCDIR)/latex/refman.pdf ../$(DOCDIR)/mapc.pdf

test: $(TARGET)
	cp /usr/local/lib/libstringc.$(SOEXT) .
	$(CC) $(CFLAGS) -o mapc_test -L../_Debug -lmapc	-lstringc ../mapc_test.c
	sh -c "LD_LIBRARY_PATH=./ ./mapc_test"
	
install: $(TARGET)
ifneq ($(OS),Linux)
	cp libmapc.dll /usr/local/lib
else
	cp libmapc.so /usr/local/lib/libmapc.so.0.1.0
	chmod 0755    /usr/local/lib/libmapc.so.0.1.0	
	( cd /usr/local/lib && ln -sf libmapc.so.0.1.0 libmapc.so.0.1 )
	( cd /usr/local/lib && ln -sf libmapc.so.0.1.0 libmapc.so.0   )
	( cd /usr/local/lib && ln -sf libmapc.so.0.1.0 libmapc.so     )
endif
	cp -rf ../mapc /usr/local/include
	
endif
