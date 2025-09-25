#! /bin/bash

for (( n=1; n<1000000000; n*=2 )) ; {
   echo "n: " $n
   for (( t=1; t<128; t*=2)) ; {
      echo -n "Threads: " $t " "
      ./numerical_integration $t $n
   }
}
