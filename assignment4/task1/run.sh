MAX=1000000

./sequential $MAX

./pthreads 32 $MAX

./openmp $MAX

mpiexec --hostfile one_host ./sieve_mpi_send_receive $MAX

mpiexec --hostfile hosts ./sieve_mpi_send_receive $MAX

mpiexec --hostfile hosts ./sieve_mpi_bcast_reduce $MAX