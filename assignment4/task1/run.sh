MAX=100000

./sieve_sequential $MAX

./sieve_pthreads 32 $MAX

./sieve_openmp $MAX

mpiexec --hostfile one_host ./sieve_mpi_send_receive $MAX

mpiexec --hostfile hosts ./sieve_mpi_send_receive $MAX

mpiexec --hostfile hosts ./sieve_mpi_bcast_reduce $MAX