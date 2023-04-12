#!/bin/sh

echo "DO NOT QUIT THIS SCRIPT UNTIL IT IS FINISHED"
echo "YOU HAVE BEEN WARNED"

judge/run_many.py maps/list_semifinal.txt bin/omegastay    bin/oh-my-blue               --times 20 --num-threads 10 >results/omegastay_vs_ohmyblue.txt
judge/run_many.py maps/list_semifinal.txt bin/omegastay    bin/granitestate             --times 20 --num-threads 10 >results/omegastay_vs_granitestate.txt
judge/run_many.py maps/list_semifinal.txt bin/omegastay    bin/babyblue                 --times 20 --num-threads 10 >results/omegastay_vs_babyblue.txt
judge/run_many.py maps/list_semifinal.txt bin/omegastay    bin/heisenberg               --times 20 --num-threads 10 >results/omegastay_vs_heisenberg.txt
judge/run_many.py maps/list_semifinal.txt bin/omegastay    bin/tahp-estimate-shield-val --times 20 --num-threads 10 >results/omegastay_vs_tahp.txt
