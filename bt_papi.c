#include <papi.h>
#include <stdlib.h>
#include <stdio.h>
#include "bt_profile.h"


// calcula o tempo de execução, necessário para escolher o melhor tempo dentro dos 5%
double* bt_papi_calctcpu(){
	int i = 0;
	double *tcpu = (double*)malloc(sizeof(double)*IT);
	for (i = 0; i < IT; i++){
		tcpu[i] = 0.001*values[i][0]/(1000*CLOCK_MHZ);
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
	int i = 0, j = 0;
	qsort(tcpu, IT, sizeof(double), bt_papi_cmp);

	for(i = 0; i < IT-2; i++){
		double p_095 = 0.95*tcpu[i];
		double p_105 = 1.05*tcpu[i];
		if(tcpu[i+1] >= p_095 && tcpu[i+2] >= p_095 && tcpu[i+1] <= p_105 && tcpu[i+2] <= p_105){
			printf("iteration %d has best time\n", i);
			for(j = 0; j < NUM_EVENTS; j++){
				values[0][j] = values[i][j];
			}
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
	for(i = 0; i < IT; i++){
		for(j = 0; j < NUM_EVENTS; j++){
			PAPI_get_event_info(counters[j],&info);
			printf("it: %d | ev: %s | val: %lld\n", i, info.symbol, values[i][j]);
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
	for(i = 0; i < NUM_FUNC; i++) (*cmd[i])(0);	
#elif defined EVERYTHING
	for(i = 0; i < NUM_FUNC; i++){
		for(j = 0; j < IT; j++) (*cmd[i])(j);
	}	
#endif
}

void bt_papi_start(int event){
    EventSet = PAPI_NULL;
    int retval;
        
    if((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT )
		ERROR_RETURN(retval);
    
    if((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);

    if((retval = PAPI_add_event(EventSet,event)) != PAPI_OK)
        ERROR_RETURN(retval);

    if((retval = PAPI_start(EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);
}

void bt_papi_stop(int event, long long *value){
	int retval;

	if ( (retval = PAPI_stop(EventSet, value)) != PAPI_OK)
        ERROR_RETURN(retval);

    if( (retval = PAPI_remove_event(EventSet, event)) != PAPI_OK)
      ERROR_RETURN(retval);

    PAPI_shutdown();
}



void bt_papi_n_start(){
    EventSet = PAPI_NULL;
    int retval;
        
    if((retval = PAPI_library_init(PAPI_VER_CURRENT)) != PAPI_VER_CURRENT )
		ERROR_RETURN(retval);
    
    if((retval = PAPI_create_eventset(&EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);

    if((retval = PAPI_add_events(EventSet,counters, NUM_EVENTS)) != PAPI_OK)
        ERROR_RETURN(retval);

    if((retval = PAPI_start(EventSet)) != PAPI_OK)
        ERROR_RETURN(retval);
}

void bt_papi_n_stop(){
	int retval;
	long long vals[NUM_EVENTS];
	if ( (retval = PAPI_stop(EventSet, vals)) != PAPI_OK)
        ERROR_RETURN(retval);

    if( (retval = PAPI_remove_events(EventSet, counters, NUM_EVENTS)) != PAPI_OK)
      ERROR_RETURN(retval);

    PAPI_shutdown();
	int i = 0;
	for(i = 0; i < NUM_EVENTS; i++){
		values[0][i] += vals[i];
	}
}


void bt_papi_init(){
	int i = 0, j = 0;
	for(i = 0; i < IT; i++){
		for(j = 0; j < NUM_EVENTS; j++){
			values[i][j] = 0;
		}
	}
}

