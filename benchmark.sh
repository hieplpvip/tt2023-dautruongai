#!/bin/sh

echo "DO NOT QUIT THIS SCRIPT UNTIL IT IS FINISHED"
echo "YOU HAVE BEEN WARNED"

mkdir -p results

judge/run_many.py maps/list_semifinal.txt bin/babyblue bin/heisenberg --no-log --times 10 >results/babyblue_vs_heisenberg.txt &
judge/run_many.py maps/list_semifinal.txt bin/babyblue bin/tahp-estimate-shield-val --no-log --times 10 >results/babyblue_vs_tahp.txt &
judge/run_many.py maps/list_semifinal.txt bin/heisenberg bin/tahp-estimate-shield-val --no-log --times 10 >results/heisenberg_vs_tahp.txt &

judge/run_many.py maps/list_semifinal.txt bin/granitestate bin/babyblue --no-log --times 10 >results/granitestate_vs_babyblue.txt &
judge/run_many.py maps/list_semifinal.txt bin/granitestate bin/heisenberg --no-log --times 10 >results/granitestate_vs_heisenberg.txt &
judge/run_many.py maps/list_semifinal.txt bin/granitestate bin/tahp-estimate-shield-val --no-log --times 10 >results/granitestate_vs_tahp.txt &

wait
