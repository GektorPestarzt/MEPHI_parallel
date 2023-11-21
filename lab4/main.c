#include <stdio.h>
#include <omp.h>
#include <time.h>

// int schedule();
int lock(int numeral, int max_number, FILE *fd);

int main() {
    // printf("%d %d\n", omp_get_num_procs(), omp_get_max_threads());
    // printf("%d\n", _OPENMP);
    // printf("%d\n", omp_get_dynamic());
    // printf("%.10lf\n", omp_get_wtick());
    // printf("%d %d\n", omp_get_nested(), omp_get_max_active_levels());
    
    // schedule();

    FILE *fd = fopen("hello.txt", "w+");

    lock(7, 10000, fd);

    fclose(fd);
    return 0;
}
/*
int schedule() {
    omp_sched_t kind;
	int chunk_size;
	omp_get_schedule(&kind, &chunk_size);

	printf("In case a runtime schedule is encountered, the ");
	switch(kind)
	{
		case omp_sched_static:
			printf("static");
			break;
		case omp_sched_dynamic:
			printf("dynamic");
			break;
		case omp_sched_guided:
			printf("guided");
			break;
		case omp_sched_auto:
			printf("auto");
			break;
		default:
			printf("other (implementation specific)");
			break;
	}

	printf(" schedule is applied, with chunks made of %d iteration%s.\n", chunk_size, (chunk_size > 1) ? "s" : "");
    return 0;
}
*/

int lock(int numeral, int max_number, FILE *fd) {
#ifdef _OPENMP
    omp_lock_t writelock;
    omp_init_lock(&writelock);
    double start = omp_get_wtime();
#else
    double start = clock();
#endif // _OPENMP

    #pragma omp parallel for
    for (int i = 1; i < max_number; ++i)
    {
        for (int j = i; j < max_number; ++j) {
            int a = j;
            int b = i;

            while (a != b) {
                if (a > b) {
                    a -= b;
                } else {
                    b -= a;
                }
            }

            if (a % 10 == numeral) {
                #ifdef _OPENMP
                omp_set_lock(&writelock);
                #endif  // _OPENMP
                fprintf(fd, "%d and %d - %d\n", i, j, a);
                #ifdef _OPENMP
                omp_unset_lock(&writelock);
                #endif  // _OPENMP
            }
        }
    }

#ifdef _OPENMP
    double end = omp_get_wtime();
    float time = end - start;
    omp_destroy_lock(&writelock);
#else
    double end = clock();
    float time = ((double)(end - start)) / CLOCKS_PER_SEC;
#endif  // _OPENMP

    printf("time - %f\n", time);
    return 0;
}