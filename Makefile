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

VPATH = $(SRCDIR)

CFLAGS   =  -Wall -g -ggdb -fpic -fmessage-length=0 -pthread

LDFLAGS  =  -Wall -g -ggdb -fpic -fmessage-length=0 -pthread
			
OBJS     =  mapc.o
			
LIBS     =  -lpthread

TARGET   =  libmapc.so

$(TARGET):  $(OBJS)
	$(CC) -shared $(LDFLAGS) -o $(TARGET) $(OBJS) $(LIBS)
	
%.o: %.c $(SRCDIR)
	$(CC) $(CFLAGS) -c $<	
	
all: $(TARGET)
	echo "Build OK"

doc:
	doxygen $(SRCDIR)/doxygen.conf
	( cd $(SRCDIR)/_Documentation/latex && make )
	cp $(SRCDIR)/_Documentation/latex/refman.pdf \
		$(SRCDIR)/_Documentation/mapc.pdf

test: $(TARGET)
	$(CC) -Wall -Werror -g -ggdb -fmessage-length=0 -o mapc_test \
		-L$(SRCDIR)/_Debug \
		-lmapc \
		-lstringc \
		$(SRCDIR)/mapc_test.c
	sh -c "LD_LIBRARY_PATH=./ ./mapc_test"
	
install: libmapc.so doc
	sudo cp libmapc.so /usr/local/lib/libmapc.so.0.1.0
	sudo chown root:staff /usr/local/lib/libmapc.so.0.1.0	
	sudo chmod 0755       /usr/local/lib/libmapc.so.0.1.0	
	( cd /usr/local/lib && sudo ln -sf libmapc.so.0.1.0 libmapc.so.0.1 )
	( cd /usr/local/lib && sudo ln -sf libmapc.so.0.1.0 libmapc.so.0   )
	( cd /usr/local/lib && sudo ln -sf libmapc.so.0.1.0 libmapc.so     )
	sudo cp mapc_test /usr/local/bin
	sudo cp -rf $(SRCDIR)/mapc /usr/local/include
	sudo chown -R root:staff /usr/local/include/mapc
	sudo mkdir -p /usr/local/doc
	sudo cp $(SRCDIR)/_Documentation/mapc.pdf /usr/local/doc
	
endif
