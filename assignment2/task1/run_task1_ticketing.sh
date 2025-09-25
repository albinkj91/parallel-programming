#! /bin/bash

for (( n=8; n<=64; n*=2)) ; {
   echo "With " $n " threads:"
   for (( t=1; t<=67108864; t*=2)) ; {
      echo -n "Segment size: " $t " "
      ./numerical_integration $n 67108864 -t $t
   }
}