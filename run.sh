## Build
#make clean
#make all

## Project 1
#mpirun -np 8 --oversubscribe --prefix /usr/local/openmpi --host h01:2,h02:2,h03:2,h04:2 out/hello_world
#mpirun -np 8 --oversubscribe --prefix /usr/local/openmpi --host h01,h02,h03,h04 -nolocal out/hello_world
#mpirun -np 8 --oversubscribe --prefix /usr/local/openmpi --host h01,h02,h03 --map-by node out/hello_world
#mpirun -np 16 --oversubscribe --prefix /usr/local/openmpi --host h01,h02,h03,h04 --map-by node out/calculate_pi
#mpirun -np 12 --oversubscribe --prefix /usr/local/openmpi --host h01,h02 --map-by node out/calculate_pi

## Project 2
#mpirun -np 4 --oversubscribe --prefix /usr/local/openmpi --host h01,h02,h03,h04 --map-by node out/prime_number 10000

## Project 3
mpirun -np 4 --oversubscribe --prefix /usr/local/openmpi --host h01,h02,h03,h04 --map-by node out/matrix_multiplication 60
