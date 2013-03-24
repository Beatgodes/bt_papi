PREFIX=/usr/local
#PREFIX=/home/cpd22840
CC=gcc
CFLAGS= -Wall -Wextra -g
TARGET=a.out

#for ie.grid.prociv.pt
#EXTRA=-L/opt/papi/lib -I/opt/papi/include

mac : bt_papi.c bt_profile.h bt_header.h
	$(CC) $(CFLAGS) $(EXTRA) -fPIC -shared -lpapi bt_papi.c -o libbtpapi.dylib

unix : bt_papi.c bt_profile.h bt_header.h
	$(CC) $(CFLAGS) $(EXTRA) -fPIC -shared -lpapi bt_papi.c -o libbtpapi.so

$(TARGET) : matrix.c
	$(CC) $(CFLAGS) $(EXTRA) matrix.c -lpapi -lbtpapi -L$(PREFIX)/lib -I$(PREFIX)/include -o $(TARGET)

install :
	if [ -e libbtpapi.dylib ]; then cp libbtpapi.dylib $(PREFIX)/lib ; fi;
	if [ -e libbtpapi.so ]; then cp libbtpapi.so $(PREFIX)/lib; fi;
	cp bt_header.h $(PREFIX)/include
