PROG=sieve_mpi

all:
	mpic++ $(PROG).cpp -o $(PROG)

sync:
	rsync -avz --exclude '.vscode' ./ olsp6250@tussilago.it.uu.se:~/itpp/parallel-programming/assignment4/task1
run_send:
	ssh olsp6250@tussilago.it.uu.se "cd itpp/parallel-programming/assignment4/task1 && mpic++ sieve_mpi.cpp -o sieve_mpi && mpiexec --hostfile hosts ./sieve_mpi 100000000"

run_bcast:
	ssh olsp6250@tussilago.it.uu.se "cd itpp/parallel-programming/assignment4/task1 && mpic++ sieve_mpi_bcast.cpp -o sieve_mpi_bcast && mpiexec --hostfile hosts ./sieve_mpi_bcast 100000000"

sequential: sieve.cpp
	g++ sieve.cpp -o sequential -std=c++11 -Wall -Wextra -pedantic

pthreads: sieve_pthreads.cpp
	g++ sieve_pthreads.cpp -o pthreads -std=c++11 -pthread -Wall -Wextra -pedantic

openmp: sieve_openmp.cpp
	g++ sieve_openmp.cpp -o openmp -std=c++11 -Wall -Wextra -fopenmp -pedantic

send_receive:
	mpic++ sieve_mpi_send_receive.cpp -o sieve_mpi_send_receive

bcast_reduce:
	mpic++ sieve_mpi_bcast_reduce.cpp -o sieve_mpi_bcast_reduce