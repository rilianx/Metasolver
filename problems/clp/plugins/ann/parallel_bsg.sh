#!/bin/bash
echo "Hola Mundo"


for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data01.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data01.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data01.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data01.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done



for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data03.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data03.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data03.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data03.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done



for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data05.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data05.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data05.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data05.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data08.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data08.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data08.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data08.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data10.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data10.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data10.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data10.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data12.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data12.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data12.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data12.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data15.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data15.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data15.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data15.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data20.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data20.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data20.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data20.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data30.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data30.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data30.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data30.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data40.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data40.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data40.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data40.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data50.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data50.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data50.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data50.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data60.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data60.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data60.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data60.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data70.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data70.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data70.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data70.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data80.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data80.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data80.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data80.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data90.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data90.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data90.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data90.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

for ((i=0;i<96;i=i+4))
do
./BSG_CLP_ANN problems/clp/benchs/ann_a/data100.txt $i 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data100.txt $((i+1)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data100.txt $((i+2)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
./BSG_CLP_ANN problems/clp/benchs/ann_a/data100.txt $((i+3)) 0.98 500 4.0 1.0 0.2 0.04 1.0 0.0 0.0 1 0 &
wait
done

