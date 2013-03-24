#include <papi.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef __APPLE_
	#include <malloc/malloc.h>
#endif

#include <string.h>

extern void bt_papi_dump();
extern void bt_papi_shutdown();

#define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__); bt_papi_dump(); exit(retval); }   

#define SHOW_VAL

typedef struct s_bt_papi_data{
	//char *event;
	int event;
	char *str;
	long long *values;
} bt_papi_data;

typedef struct s_bt_papi_system{
	bt_papi_data *data;
	int* counters;
	int mem_linesize;
	int num_events;
	int EventSet;
	int it;
	float clock_mhz;
	int best;
	int num_func;
	void (**cmd)(int i);
} bt_papi_system;

bt_papi_system *bt_sys;


long long bt_papi_get_value_it_named_event(const char *event, int it){
	int code;
	int retval;

	if((retval = PAPI_event_name_to_code((char*)event, &code)) != PAPI_OK)
		ERROR_RETURN(retval);

	int i = 0;
	for (i = 0; i < bt_sys->num_events; ++i){
		//if(!strcmp(bt_sys->data[i]->event, event)) return bt_sys->data[i]->values[it];
		if(code == bt_sys->data[i].event) return bt_sys->data[i].values[it];
	}
	return -1;
}

long long* bt_papi_get_value_all_named_event(const char *event){
	int code;
	int retval;

	if((retval = PAPI_event_name_to_code((char*)event, &code)) != PAPI_OK)
		ERROR_RETURN(retval);

	int i = 0;
	for (i = 0; i < bt_sys->num_events; ++i){
		//if(!strcmp(bt_sys->data[i]->event, event)) return bt_sys->data[i]->values;
		if(code == bt_sys->data[i].event) return bt_sys->data[i].values;

	}
	return NULL;
}

void bt_papi_add_named_event(/*const char **/ int event){
	int i = bt_sys->num_events;
	(bt_sys->num_events)++;
	//int code;
	int retval;
/*
	if((retval = PAPI_event_name_to_code((char*)event, &code)) != PAPI_OK)
		ERROR_RETURN(retval);
*/

	PAPI_event_info_t info;
	if((retval = PAPI_get_event_info(event,&info)) != PAPI_OK)
		ERROR_RETURN(retval);

	//printf("adding %s at %d\n", info.symbol, i);

	bt_sys->data = (bt_papi_data*)realloc(bt_sys->data, sizeof(bt_papi_data) * bt_sys->num_events);
	bt_sys->data[i].event = event; //code;
	bt_sys->data[i].values = (long long*)malloc(sizeof(long long) * bt_sys->it);
	bt_sys->data[i].str = strdup(info.symbol); /*strdup(event);*/

	int j = 0;
	for(j = 0; j < bt_sys->it; j++){
		bt_sys->data[i].values[j] = 0;
	}
}

void bt_papi_init(int mem_linesize, int it, float clock_mhz){
	int retval;

	bt_sys = (bt_papi_system*)malloc(sizeof(bt_papi_system));
	bt_sys->data = NULL;
	bt_sys->mem_linesize = mem_linesize;
	bt_sys->num_events = 0;
	bt_sys->EventSet = PAPI_NULL;
	bt_sys->it = it;
	bt_sys->clock_mhz = clock_mhz;
	bt_sys->best = -1;
	bt_sys->num_func = 0;
	bt_sys->cmd = NULL;
	bt_sys->counters = NULL;

    if((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT )
		ERROR_RETURN(retval);

}

// calcula o tempo de execução, necessário para escolher o melhor tempo dentro dos 5%
double* bt_papi_calctcpu(){
	int i = 0;
	float clock_mhz = bt_sys->clock_mhz;
	long long *values = bt_papi_get_value_all_named_event("PAPI_TOT_CYC");
	double *tcpu = (double*)malloc(sizeof(double)*bt_sys->it);
	for (i = 0; i < bt_sys->it; i++){
		tcpu[i] = 0.001*values[i]/(1000*clock_mhz);
	}
	return tcpu;
}

// função de comparação para o qsort
int bt_papi_cmp(const void *x, const void *y)
{
  double xx = *(double*)x, yy = *(double*)y;
  if (xx < yy) return -1;
  if (xx > yy) return  1;
  return 0;
}

// escolhe o melhor tempo, estando o segundo e terceiro melhor num range de 5%
void bt_papi_filtervalues(double *tcpu){
	int i = 0;
	qsort(tcpu, bt_sys->it, sizeof(double), bt_papi_cmp);

	for(i = 0; i < bt_sys->it - 2; i++){
		double p_095 = 0.95*tcpu[i];
		double p_105 = 1.05*tcpu[i];
		if(tcpu[i+1] >= p_095 && tcpu[i+2] >= p_095 && tcpu[i+1] <= p_105 && tcpu[i+2] <= p_105){
			printf("iteration %d has best time\n", i);
			bt_sys->best = i;
			return;
		}
	}
	printf("System was not able to determine best execution time, assuming best overall time...\n");
}

/***************************
****** PUBLIC FUNCTIONS ****
***************************/


void bt_papi_postprocess(){
	int i = 0;
	int j = 0;

#ifdef SHOW_VAL
	PAPI_event_info_t info;
	for(j = 0; j < bt_sys->num_events; j++){
		PAPI_get_event_info(bt_sys->counters[j],&info);
		for(i = 0; i < bt_sys->it; i++){
			printf("it: %d | ev: %s | val: %lld\n", i, info.symbol, bt_sys->data[j].values[i]);
			//printf("it: %d | ev: %s | val: %lld\n", i, info.symbol, values[i][j]);
		}
		printf("------------------\n");
	}
#endif

#ifdef BEST_TIME
	if(IT < 3){
		printf("System requires at least 3 iterations to determine best time\n");
		return;
	}
	double *tcpu = bt_papi_calctcpu();
	bt_papi_filtervalues(tcpu);
	for(i = 0; i < bt_sys->num_func; i++) (bt_sys->cmd[i])(bt_sys->best);
	/* (*cmd[i])(bt_sys->best);	*/
#elif defined EVERYTHING
	for(i = 0; i < bt_sys->num_func; i++){
		for(j = 0; j < bt_sys->it; j++) (bt_sys->cmd[i])(j);
	}	
#endif
}

void bt_create_events(){
	bt_sys->counters = (int*)malloc(sizeof(int) * bt_sys->num_events);
	int i = 0;
	for(i = 0; i < bt_sys->num_events; i++){
		bt_sys->counters[i] = bt_sys->data[i].event;
	}
}


void bt_papi_start(int event){
    bt_sys->EventSet = PAPI_NULL;
    int retval;
    
    if((retval = PAPI_create_eventset(&(bt_sys->EventSet))) != PAPI_OK)
        ERROR_RETURN(retval);

    if((retval = PAPI_add_event(bt_sys->EventSet,event)) != PAPI_OK)
        ERROR_RETURN(retval);

    if((retval = PAPI_start(bt_sys->EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);
}

void bt_papi_stop(int event, long long *value){
	int retval;

	if ((retval = PAPI_stop(bt_sys->EventSet, value)) != PAPI_OK)
        ERROR_RETURN(retval);

    if( (retval = PAPI_remove_event(bt_sys->EventSet, event)) != PAPI_OK)
      ERROR_RETURN(retval);

    //PAPI_shutdown();
}



void bt_papi_n_start(){
    bt_sys->EventSet = PAPI_NULL;
    int retval;
        
    if((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT )
		ERROR_RETURN(retval);
    
    if((retval = PAPI_create_eventset(&(bt_sys->EventSet))) != PAPI_OK)
        ERROR_RETURN(retval);

    if((retval = PAPI_add_events(bt_sys->EventSet, bt_sys->counters, bt_sys->num_events)) != PAPI_OK)
        ERROR_RETURN(retval);

    if((retval = PAPI_start(bt_sys->EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);
}

void bt_papi_n_stop(){
	int retval;
	long long vals[bt_sys->num_events];
	if ( (retval = PAPI_stop(bt_sys->EventSet, vals)) != PAPI_OK)
        ERROR_RETURN(retval);

    if( (retval = PAPI_remove_events(bt_sys->EventSet, bt_sys->counters, bt_sys->num_events)) != PAPI_OK)
      ERROR_RETURN(retval);

    //PAPI_shutdown();
	int i = 0;
	for(i = 0; i < bt_sys->num_events; i++){
		bt_sys->data[i].values[0] += vals[i];
		//values[0][i] += vals[i];
	}
}

void bt_papi_shutdown(){
	//printf("Shutting down system...\n");
	PAPI_shutdown();
	int i = 0;
	for(i = 0; i < bt_sys->num_events; i++){
		free(bt_sys->data[i].values);
		free(bt_sys->data[i].str);
	}
	//free(bt_sys->data);
	free(bt_sys->counters);
	free(bt_sys->cmd);
	free(bt_sys);
}

void bt_papi_dump(){
	printf("data pointer: %p\n", bt_sys->data);
	printf("counters pointer: %p\n", bt_sys->counters);
	printf("mem_linesize: %d\n", bt_sys->mem_linesize);
	printf("num_events: %d\n", bt_sys->num_events);
	printf("eventset: %d\n", bt_sys->EventSet);
	printf("it: %d\n", bt_sys->it);
	printf("clock mhz: %f\n", bt_sys->clock_mhz);
	printf("best: %d\n", bt_sys->best);
	printf("num_func: %d\n", bt_sys->num_func);
	printf("cmd pointer %p\n", bt_sys->cmd);

	printf("DATA content\n");
	int i = 0;
	for(i = 0; i < bt_sys->num_events; i++){
		printf("%d event %d | %s\n", i, bt_sys->data[i].event, bt_sys->data[i].str);
	}

	printf("COUNTERS content\n");
	for(i = 0; i < bt_sys->num_events; i++){
		printf("%d : %d\n", i, bt_sys->counters[i]);
	}

	bt_papi_shutdown();

}


