#!/usr/bin/env bash

# Generate curves that resemble idealized plectonemic supercoils.
# Use them to test whether "calc_writhe" is working correctly.
# If the supercoil is narrow enough, and the number of points along the curve
# is high enough, then the writhe should roughly match the "N_TW" parameter.

# (I probably shouldn't have written this in bash.)

N_POINTS=1000  # number of points in the curve (before adding linkers)
N_TW=5         # number of twists
R=2.0          # radius of cylinder.  spacing between points should be << R
LZ=500
DZ=`echo "2*$LZ/($N_POINTS)" | bc -l`   # should be << R
PI=3.141592653589793

# number of points in the linkers on either end connecting opposite
# sides of the plectonemic supercoils.  We want the spacing between these
# points to roughly equal the spacing between points elsewhere in the curve.
N_LINKER=`awk -v n=$N_POINTS -v ntw=$N_TW -v r=$R -v dz=$DZ 'BEGIN{dt=2*3.1415*r/(n*ntw); dr=sqrt(dt*dt+dz*dz); print int(0.5+2*r/dr)}'`

for ((i=0; i<$((N_POINTS/2)); i++)); do 
    XCOORD=`echo "$R*c(($i+0.5)*2*$PI*$N_TW/$N_POINTS)" | bc -l`
    YCOORD=`echo "$R*s(($i+0.5)*2*$PI*$N_TW/$N_POINTS)" | bc -l`
    ZCOORD=`echo "$i*$DZ" | bc -l`
    echo "$XCOORD $YCOORD $ZCOORD"
done


for ((j=0; j < $N_LINKER; j++)); do
    R_EFF=`echo "$R*($j-0.5*$N_LINKER)/(0.5*$N_LINKER)" | bc -l`
    XCOORD=`echo "-1*$R_EFF*c((($i+0.5)*$N_TW/$N_POINTS)*2*$PI)" | bc -l`
    YCOORD=`echo "-1*$R_EFF*s((($i+0.5)*$N_TW/$N_POINTS)*2*$PI)" | bc -l`
    ZCOORD=`echo "$DZ*$N_POINTS/2" | bc -l`
    echo "$XCOORD $YCOORD $ZCOORD"
done
   

for ((i=$((N_POINTS/2)); i<$N_POINTS; i++)); do 
    XCOORD=`echo "$R*c((($i+0.5)*$N_TW/$N_POINTS+0.5)*2*$PI)" | bc -l`
    YCOORD=`echo "-1*$R*s((($i+0.5)*$N_TW/$N_POINTS+0.5)*2*$PI)" | bc -l`
    ZCOORD=`echo "($N_POINTS - $i)*$DZ" | bc -l`
    echo "$XCOORD $YCOORD $ZCOORD"
done

for ((j=0; j < $N_LINKER; j++)); do
    R_EFF=`echo "$R*($j-0.5*$N_LINKER)/(0.5*$N_LINKER)" | bc -l`
    XCOORD=`echo "-1*$R_EFF*c((($i+0.5)*$N_TW/$N_POINTS+0.5)*2*$PI)" | bc -l`
    YCOORD=`echo "$R_EFF*s((($i+0.5)*$N_TW/$N_POINTS+0.5)*2*$PI)" | bc -l`
    ZCOORD="0"
    echo "$XCOORD $YCOORD $ZCOORD"
done
