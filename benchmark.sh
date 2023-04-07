#!/bin/sh

echo "DO NOT QUIT THIS SCRIPT UNTIL IT IS FINISHED"
echo "YOU HAVE BEEN WARNED"

mkdir -p results

judge/run_many.py maps/list_semifinal.txt bin/babyblue bin/heisenberg --times 10 --num-threads 5 >results/babyblue_vs_heisenberg.txt
judge/run_many.py maps/list_semifinal.txt bin/babyblue bin/tahp-estimate-shield-val --times 10 --num-threads 5 >results/babyblue_vs_tahp.txt
judge/run_many.py maps/list_semifinal.txt bin/heisenberg bin/tahp-estimate-shield-val --times 10 --num-threads 5 >results/heisenberg_vs_tahp.txt

judge/run_many.py maps/list_semifinal.txt bin/granitestate bin/babyblue --times 10 --num-threads 5 >results/granitestate_vs_babyblue.txt
judge/run_many.py maps/list_semifinal.txt bin/granitestate bin/heisenberg --times 10 --num-threads 5 >results/granitestate_vs_heisenberg.txt
judge/run_many.py maps/list_semifinal.txt bin/granitestate bin/tahp-estimate-shield-val --times 10 --num-threads 5 >results/granitestate_vs_tahp.txt

judge/run_many.py maps/list_semifinal.txt bin/omegastay bin/granitestate --num-threads 5 --times 10 >results/omegastay_vs_granitestate.txt

wait
