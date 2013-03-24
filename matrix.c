#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#include <papi.h>
#include <bt_header.h>

#define SIZE 32


void mul_matrix(float (* __restrict__ a)[SIZE], float (* __restrict__ b)[SIZE], float (* __restrict__ c)[SIZE]){
	size_t i = 0, j = 0, k = 0;
	float tmp1;

	for(i = 0; i < SIZE; i++){
		for(k = 0; k < SIZE; k++){
			tmp1 = a[i][k];
			for(j = 0; j < SIZE; j++){
				 c[i][j] += tmp1 * b[k][j];		
			}
		}
	}
}


float randomFloat()
{
	float r = (float)rand()/(float)RAND_MAX;
	return r;
}

void test1(int a){
	printf("im here %d\n", a);
}

void test2(int a){
	printf("im also here %d\n", a);
}

int main(){
	int i = 0, j = 0;
	int seed = time(NULL);
	srand(seed);

	bt_papi_init(64, 5, 2670);
	bt_papi_add_named_event(PAPI_TOT_CYC);
	bt_papi_add_named_event(PAPI_VEC_SP);
	bt_create_events();

	bt_papi_add_function(test1);
	bt_papi_add_function(test2);

	// set up matrix açoreano style
/*	float (*a)[SIZE] = (float (*)[SIZE])malloc(sizeof *a * SIZE);
	float (*b)[SIZE] = (float (*)[SIZE])malloc(sizeof *b * SIZE);
	float (*c)[SIZE] = (float (*)[SIZE])malloc(sizeof *c * SIZE);
	float (*d)[SIZE] = (float (*)[SIZE])malloc(sizeof *d * SIZE);



	for(i = 0; i < SIZE; i++){
		for(j = 0; j < SIZE; j++){
			a[i][j] = randomFloat();
			b[i][j] = 1;//randomFloat();
			c[i][j] = 0;
			d[i][j] = 0;			
		}
	}
*/
	//		bt_papi_n_start();
//			mul_matrix(a,b,c);
	//		bt_papi_n_stop();
	bt_papi_postprocess();
/*
	free(a);
	free(b);
	free(c);
	free(d);
*/
	return 0;
}

