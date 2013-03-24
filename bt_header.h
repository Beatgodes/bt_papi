extern void bt_papi_postprocess();
extern void bt_papi_start(int event);
extern void bt_papi_stop(int event, long long *values);
extern void bt_papi_n_start();
extern void bt_papi_n_stop();
extern void bt_papi_init(int mem_linesize, int it, float clock_mhz);
extern void bt_papi_add_named_event(int event);
extern void bt_create_events();
extern void bt_papi_shutdown();