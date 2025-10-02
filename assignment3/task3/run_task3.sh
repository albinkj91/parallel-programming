#! /bin/bash

size=200

echo "1. Small quadratic matrices of size "$size"x"$size":"
echo
./matrix_multiplication 8 $size $size $size 1
./matrix_multiplication 8 $size $size $size 2
./matrix_multiplication 8 $size $size $size 3
echo

########################################

size=800

echo "2. Large quadratic matrices of size "$size"x"$size":"
echo
for (( c=1; c<=2; c++ )) ; {
   echo "Running with collapse($c):"
   for (( t=1; t<=256; t*=2 )) ; {
      ./matrix_multiplication $t $size $size $size $c
   }
   echo
}
echo

########################################

sizeI=1
sizeJ=160000
sizeK=1000

echo "3. Large rectangular matrices of size "$sizeI"x"$sizeK" and "$sizeK"x"$sizeJ":"
echo
for (( c=1; c<=2; c++ )) ; {
   echo "Running with collapse($c):"
   for (( t=1; t<=256; t*=2 )) ; {
      ./matrix_multiplication $t $sizeI $sizeJ $sizeK $c
   }
   echo
}
