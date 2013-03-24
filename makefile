lib : bt_papi.c bt_profile.h bt_header.h
	g++ -fPIC -shared -lpapi bt_papi.c -o libbtpapi.so -I/opt/papi/include -L/opt/papi/lib
