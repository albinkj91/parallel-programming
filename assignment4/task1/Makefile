SEQ=sieve_sequential
PTH=sieve_pthreads
OMP=sieve_openmp
MPISR=sieve_mpi_send_receive
MPIBR=sieve_mpi_bcast_reduce


all: sequential pthreads openmp send_receive bcast_reduce

##############################
# Just to improve the workflow
sync:
	rsync -avz --exclude '.vscode' ./ olsp6250@tussilago.it.uu.se:~/itpp/parallel-programming/assignment4/task1

run_send:
	ssh olsp6250@tussilago.it.uu.se "cd itpp/parallel-programming/assignment4/task1 && mpic++ sieve_mpi.cpp -o sieve_mpi && mpiexec --hostfile hosts ./sieve_mpi 100000000"

run_bcast:
	ssh olsp6250@tussilago.it.uu.se "cd itpp/parallel-programming/assignment4/task1 && mpic++ sieve_mpi_bcast.cpp -o sieve_mpi_bcast && mpiexec --hostfile hosts ./sieve_mpi_bcast 100000000"
#
##############################

sequential:
	g++ $(SEQ).cpp -o $(SEQ) -std=c++11 -Wall -Wextra -pedantic

pthreads:
	g++ $(PTH).cpp -o $(PTH) -std=c++11 -pthread -Wall -Wextra -pedantic

openmp:
	g++ $(OMP).cpp -o $(OMP) -std=c++11 -Wall -Wextra -fopenmp -pedantic

send_receive:
	mpic++ $(MPISR).cpp -o $(MPISR)

bcast_reduce:
	mpic++ $(MPIBR).cpp -o $(MPIBR)

clean:
	$(RM) $(SEQ) $(PTH) $(OMP) $(MPISR) $(MPIBR)