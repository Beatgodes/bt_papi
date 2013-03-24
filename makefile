mac : bt_papi.c bt_profile.h bt_header.h
	g++ -fPIC -shared -lpapi bt_papi.c -o libbtpapi.dylib -Wall -Wextra -g

unix : bt_papi.c bt_profile.h bt_header.h
	g++ -fPIC -shared -lpapi bt_papi.c -o libbtpapi.so -Wall -Wextra -g

matrix : matrix.c
	g++ matrix.c -lpapi -lbtpapi -g

install : libbtpapi.so bt_header.h
	cp libbtpapi.dylib /usr/local/lib
	cp bt_header.h /usr/local/include
