#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <papi.h>
#include <time.h>

const char *matrix_names[6] = {
    "martix_ijk",
    "martix_ikj",
    "martix_jik",
    "martix_jki",
    "martix_kij",
    "martix_kji"
};


// should evict the cache. 

// L1d cache:                       256 KiB
// L1i cache:                       256 KiB
// L2 cache:                        2 MiB
// L3 cache:                        16 MiB
void evict_cache() {
    size_t cache_size = 1024 * 1024 * 2;

    size_t elems = 16 *cache_size / sizeof(int);

    int * evictArray = (int *) malloc(elems * sizeof(int));

    if(evictArray == NULL) {
        printf("Failed to create array");
        exit(1);
    }

    // clears the cache.
    for(size_t i = 0; i < elems; i++) {
        evictArray[i] = i;
    }
    free(evictArray);
}

// i-j-k
void martix_ijk(int given_size, double ** result, double ** a, double ** b) {
    for (int i = 0; i < given_size; i++) {
        for (int j = 0; j < given_size; j++) {
            for (int k = 0; k < given_size; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// j-i-k
void martix_jik(int given_size, double ** result, double ** a, double ** b) {
    for (int j = 0; j < given_size; j++) {
        for (int i = 0; i < given_size; i++) {
            for (int k = 0; k < given_size; k++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// j-k-i
void martix_jki(int given_size, double ** result, double ** a, double ** b) {
    for (int j = 0; j < given_size; j++) {
        for (int k = 0; k < given_size; k++) {
            for (int i = 0; i < given_size; i++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// k-j-i
void martix_kji(int given_size, double ** result, double ** a, double ** b) {
    for (int k = 0; k < given_size; k++) {
        for (int j = 0; j < given_size; j++) {
            for (int i = 0; i < given_size; i++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }
}

// i-k-j
void martix_ikj(int given_size, double ** result, double ** a, double ** b) {
    for (int i = 0; i < given_size; i++) {
        for (int k = 0; k < given_size; k++) {
            for (int j = 0; j < given_size; j++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }


}

// k-i-j
void martix_kij(int given_size, double ** result, double ** a, double ** b) {
    for (int k = 0; k < given_size; k++) {
        for (int i = 0; i < given_size; i++) {
            for (int j = 0; j < given_size; j++) {
                result[i][j] += a[i][k] * b[k][j];
            }
        }
    }

}

void init_matrix(int given_size, double ** given_martix) {
    for(int i = 0; i < given_size; i ++) {
        for(int j = 0; j < given_size; j++) {
            given_martix[i][j] = 2.0;
        }
    }
}

void print_papi(long long counters[5], long long cache_counters[4],  int size, int fun){

    printf("Tests measured at size: %d\n", size);
    // total cycles

    printf("Function: %s \n", matrix_names[fun]);
    // PAPI_TOT_CYC,
    printf("PAPI_TOT_CYC: %lld \n", counters[0]);

    // total instructions
    // PAPI_TOT_INS,
    printf("PAPI_TOT_INS: %lld \n", counters[1]);

    // total load store instructions
    // PAPI_LD_INS,
    // PAPI_SR_INS,
    printf("PAPI_LD_INS: %lld \n", counters[2]);
    printf("PAPI_SR_INS: %lld \n", counters[3]);

    // total floating point instructions
    // PAPI_FP_INS,
    printf("PAPI_FP_INS: %lld \n", counters[4]);

    // L1 data cache accesses and misses
    // PAPI_L1_DCA,
    // PAPI_L1_DCM,
    printf("PAPI_L1_DCA: %lld \n", cache_counters[0]);

    printf("PAPI_L1_DCM: %lld \n", cache_counters[1]);

    // L2 data cache accesses and misses
    // PAPI_L2_DCA,
    // PAPI_L2_DCM
    printf("PAPI_L2_DCA: %lld \n", cache_counters[2]);
    printf("PAPI_L2_DCM: %lld\n", cache_counters[3]);

    printf("\n\n\n\n");
}

void fill_papi_counters(int num_elem, int size, int current_fun, int event[],
                        long long counters[num_elem], void (*matrix_array[])(int given_size, double **result, double **a, double **b))
{
    // structure for the arrays
    evict_cache();
    double **result = (double **)malloc(size * sizeof(double *));
    double **a = (double **)malloc(size * sizeof(double *));
    double **b = (double **)malloc(size * sizeof(double *));

    // Allocate memory for rows
    for (int j = 0; j < size; j++)
    {
        result[j] = (double *)malloc(size* sizeof(double));
        a[j] = (double *)malloc(size * sizeof(double));
        b[j] = (double *)malloc(size * sizeof(double));
    }

    // check to see if the matrices got allocated
    if (result == NULL || a == NULL || b == NULL)
    {
        exit(-1);
    }

    // fills the arrays with 2.0
    init_matrix(size, a);
    init_matrix(size, b);

    // starts the papi counter
    PAPI_start_counters(event, num_elem);

    // matrix calcs.
    matrix_array[current_fun](size, result, a, b);

    // stops the papi counter
    PAPI_stop_counters(counters, num_elem);

    // writes to counter.
    PAPI_read_counters(counters, num_elem);

    for (int remove = 0; remove < size; remove++)
    {
        free(result[remove]);
        free(a[remove]);
        free(b[remove]);
    }
    free(result);
    free(a);
    free(b);
}

void mulitply_matrix_papi(int event_set[], int event_cache[]) {

    int sizes[] = {50, 100, 200, 400, 800, 1200, 1600, 2000};

    void (*matrix_array[])
        (int given_size, double **result, double **a, double **b) = {
        martix_ijk,
        martix_ikj,
        martix_jik,
        martix_jki,
        martix_kij,
        martix_kji
    };

    // for loop to go through the functions
    for(int current_fun = 0; current_fun < 6; current_fun++) {

        // loop to go through the sizes.
        for (int i = 0; i < 8; i++) {

            // container to keep the measurements in from events.
            long long  * counters = malloc(sizeof(long long) * 5);

            fill_papi_counters(5, sizes[i], current_fun, event_set, counters, matrix_array);

            long long  * cache_counters = malloc(sizeof(long long) * 4);

            fill_papi_counters(4, sizes[i], current_fun, event_cache, cache_counters, matrix_array);

            // prints out the tests.
            print_papi(counters, cache_counters, sizes[i], current_fun);

            free(counters);
            free(cache_counters);
        }
    }
}

void papi_measurements() {
    int PAPI_events[] = {
        // total cycles
        PAPI_TOT_CYC,
        // total instructions
        PAPI_TOT_INS,
        // total load store instructions
        PAPI_LD_INS,
        PAPI_SR_INS,
        // total floating point instructions
        PAPI_FP_INS,
    };

    int PAPI_events_cache[] = {
        // L1 data cache accesses and misses
        PAPI_L1_DCA,
        PAPI_L1_DCM,
        // L2 data cache accesses and misses
        PAPI_L2_DCA,
        PAPI_L2_DCM
    };

    int init = PAPI_library_init(PAPI_VER_CURRENT);
    if(init != PAPI_VER_CURRENT) {
        printf("Failed to get the Current Verison \n");
        exit(1);
    }


    mulitply_matrix_papi(PAPI_events, PAPI_events_cache);

    PAPI_shutdown();
}


void mulitply_matrix_time(int choice) {

    // will meaure real time or thread time.
    int index = -1;
    if (choice == 1) {
        index = CLOCK_REALTIME;
    }
    else if (choice == 2) {
        index = CLOCK_THREAD_CPUTIME_ID;
    }

    int sizes[] = {50, 100, 200, 400, 800, 1200, 1600, 2000};

    void (*matrix_array[])(int given_size, double **result, double **a, double **b) = {
        martix_ijk,
        martix_ikj,
        martix_jik,
        martix_jki,
        martix_kij,
        martix_kji
    };

    // for loop to go through the functions
    for (int current_fun = 0; current_fun < 6; current_fun++)
    {

        uint64_t exec_time;

        struct timespec tick, tock;

        // loop to go through the sizes.
        for (int i = 0; i < 8; i++)
        {
            // structure for the arrays
            evict_cache();
            double **result = (double **)malloc(sizes[i] * sizeof(double *));
            double **a = (double **)malloc(sizes[i] * sizeof(double *));
            double **b = (double **)malloc(sizes[i] * sizeof(double *));

            // Allocate memory for rows
            for (int j = 0; j < sizes[i]; j++) {
                result[j] = (double *)malloc(sizes[i] * sizeof(double));
                a[j] = (double *)malloc(sizes[i] * sizeof(double));
                b[j] = (double *)malloc(sizes[i] * sizeof(double));
            }

            // check to see if the matrices got allocated
            if (result == NULL || a == NULL || b == NULL) {
                exit(-1);
            }

            // fills the arrays with 2.0
            init_matrix(sizes[i], a);
            init_matrix(sizes[i], b);

            // start time goes here.
            clock_gettime(index, &tick);

            // matrix calcs.
            matrix_array[current_fun](sizes[i], result, a, b);

            // stop time goes here.
            clock_gettime(index, &tock);

            exec_time = (1000000000 * (tock.tv_sec - tick.tv_sec)
             + tock.tv_nsec - tick.tv_nsec);

            printf("This is a measurement of the Size: %d and with the function: %s \n", sizes[i], matrix_names[current_fun]);

            if(choice == 1) {
                //real time print;
                printf("Elapsed real time of the CPU = %llu nanoseconds \n",
                    (long long unsigned int) exec_time);

            } else if(choice == 2) {
                //thread time print;
                printf("Elapsed thread time of the CPU = %llu nanoseconds \n",
                    (long long unsigned int) exec_time);

            }

            // cleans up memory
            for (int remove = 0; remove < sizes[i]; remove++) {
                free(result[remove]);
                free(a[remove]);
                free(b[remove]);
            }
            free(result);
            free(a);
            free(b);
        }
    }
}

int main (int argc, char *argv[]) {

    int input = 0;

    printf("Do you want to run time_real_time == 1 or time_thread_time == 2 or papi == 3: \n");
    scanf("%d", &input);

    if(input == 1) {
        mulitply_matrix_time(1);
    }

    if(input == 2) {
        mulitply_matrix_time(2);
    }

    if(input == 3) {
        papi_measurements(); 
    }

    // ez :)
    else {
        main(argc, argv);
    }


    return 0;
}