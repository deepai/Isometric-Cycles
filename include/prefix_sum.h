#ifndef _H_PREFIX_SUM
#define _H_PREFIX_SUM

template<typename T>
T prefixsum_inclusive(T *x, int N) {
    T *suma;
    T last_val = x[N - 1];
    #pragma omp parallel
    {
        const int ithread = omp_get_thread_num();
        const int nthreads = omp_get_num_threads();
        #pragma omp single
        {
            suma = new T[nthreads+1];
            suma[0] = 0;
        }
        T sum = 0;
        #pragma omp for schedule(static) nowait
        for (int i=0; i<N; i++) {
            sum += x[i];
            x[i] = sum;
        }
        suma[ithread+1] = sum;
        #pragma omp barrier
        T offset = 0;
        for(int i=0; i<(ithread+1); i++) {
            offset += suma[i];
        }
        #pragma omp for schedule(static)
        for (int i=0; i<N; i++) {
            x[i] += offset;
        }
    }
    delete[] suma;
    return (last_val + x[N - 1]);
}

#endif