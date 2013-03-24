#PREFIX=/usr/local
PREFIX=/home/cpd22840
CC=gcc

#for ie.grid.prociv.pt
EXTRA=-L/opt/papi/lib -I/opt/papi/include

mac : bt_papi.c bt_profile.h bt_header.h
	$(CC) -fPIC -shared -lpapi bt_papi.c -o libbtpapi.dylib -Wall -Wextra -g

unix : bt_papi.c bt_profile.h bt_header.h
	$(CC) -fPIC -shared -lpapi bt_papi.c -o libbtpapi.so -Wall -Wextra -g $(EXTRA)

matrix : matrix.c
	$(CC) matrix.c -lpapi -lbtpapi -O3 -L$(PREFIX)/lib -I$(PREFIX)/include $(EXTRA)

install : libbtpapi.so bt_header.h
	if [ -e libbtpapi.dylib ]; then cp libbtpapi.dylib $(PREFIX)/lib ; fi;
	if [ -e libbtpapi.so ]; then cp libbtpapi.so $(PREFIX)/lib; fi;
	cp bt_header.h $(PREFIX)/include
