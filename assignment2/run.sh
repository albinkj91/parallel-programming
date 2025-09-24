#! /bin/bash

for (( n=1; n<100000000; n*=2 )) ; {
   echo "n: " $n
   for (( t=1; t<128; t*=2)) ; {
      ./numerical_integration $t $n
   }
}
