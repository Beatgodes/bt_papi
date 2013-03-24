#define EVERYTHING // calculates all iterations

//best time is currently bugged.
// como vamos apenas correr uma iteração, acho que não interessa estar a corrigir/usar isto
//#define BEST_TIME // calculates only the iteration with best time

#define SHOW_VAL // show counter values

#define IT 1 // numero de iterações a executar
#define CLOCK_MHZ 2670 // frequencia em MHz
#define NUM_EVENTS 4 // numero de contadores
#define MEM_LINESIZE 64 // last level cache line size in bytes

#define ERROR_RETURN(retval) { fprintf(stderr, "Error %d %s:line %d: \n", retval,__FILE__,__LINE__);  exit(retval); }                              

int EventSet;
long long values[IT][NUM_EVENTS];
// declara contadores

int counters[NUM_EVENTS] = {
                                PAPI_L3_LDM,
                                PAPI_LD_INS,
                                PAPI_L1_LDM,
                                PAPI_L2_LDM
                            };



// declara funções que processam os resultados
// todas as funções recebem um inteiro para identificar a iteração
// todas as funções retornam void
// podes definir variaveis globais para utilizar valores entre funções
/*
float mflps[IT];
void mflops(int i){
	mflps[i] =  (float)CLOCK_MHZ*(float)(values[i][1])/(float)(values[i][0]);
	printf("MFLOPS IT %d: %f\n", i, mflps[i]);
}

void tcpu(int i){
	printf("TCPU IT %d: %f\n", i, (0.001*(double)(values[i][0]))/(1000*(double)(CLOCK_MHZ)));
}

long long b2r[IT];
void byte2ram(int i){
	b2r[i] = MEM_LINESIZE * values[i][3];
	printf("byte2ram IT %d: %lld\n", i, b2r[i]);
}

void opIntens(int i){
	printf("OpIntens IT %d: %f\n", i, (float)(values[i][1])/(float)(b2r[i]));
}

void ipermemacc(int i){
	printf("iPERmemacc IT %d: %f\n", i, (float)(values[i][2])/(float)(b2r[i]));
}

void cpi(int i){
	printf("CPI %d: %f\n", i, (float)(values[i][0])/(float)(values[i][2]));
}
*/


float l1ms[IT];
void l1miss(int i){
    l1ms[i] =  100*(float)(values[i][2])/(float)(values[i][1]);
    printf("L1Miss IT %d: %f\n", i, l1ms[i]);
}

float l2ms[IT];
void l2miss(int i){
    l2ms[i] = 100*(float)(values[i][3])/(float)(values[i][2]);
    printf("L2Miss IT %d: %f\n", i, l2ms[i]);
}

float l3ms[IT];
void l3miss(int i){
    l3ms[i] = 100*(float)(values[i][0])/(float)(values[i][3]);
    printf("L3Miss IT %d: %f\n", i, l3ms[i]);
}


#define NUM_FUNC 3 // numero de funções de postprocessing

// associar as funções no array
void (*cmd[NUM_FUNC])(int i) = {
									l1miss,
									l2miss,
									l3miss
								};

