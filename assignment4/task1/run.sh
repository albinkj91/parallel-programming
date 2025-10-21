make all

N=100000
delta=100000

for (( i=0; i<3; i++ )) ; {
    for (( j=0; j<9; j++ )) ; {
        echo "Primes from 1 to "$N":"
        echo -n "sequential:    "
        ./sieve_sequential $N
        echo -n "pthreads:      "
        ./sieve_pthreads 32 $N
        echo -n "openmp:        "
        ./sieve_openmp $N
        echo -n "mpi 1 server:  "
        mpiexec --hostfile one_host ./sieve_mpi_send_receive $N
        echo -n "mpi 3 servers: "
        mpiexec --hostfile hosts ./sieve_mpi_send_receive $N
        echo -n "mpi bcast:     "
        mpiexec --hostfile hosts ./sieve_mpi_bcast_reduce $N
        ((N+=delta))
        echo
    }

    echo
    ((delta*=10))
}