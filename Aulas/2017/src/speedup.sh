#!/bin/bash

function show_help {
	clear
	printf "Available commands:\n\n"
	printf "\t-p, --program\tProgram to run\n"
	printf "\t-t, --threads\tNumber of threads to start with (decreases log2)\n"
	printf "\t-s, --size\tProblem size\n"
	printf "\t--sudo\tExecute with sudo\n"
	printf "\t--with-papi\tEnable PAPI\n\n"
	printf "Available sizes:\n\n\tMINI=512\n\tSMALL=1024\n\tMEDIUM=2048"
	printf " \n\tBIG=4096\n\tLARGE=8192\n\tGIANT=16384\n\tHUGE=32768\n\tWTF=65536\n\n"
	exit
}

function build {
	if [[ $USEPAPI -eq 1 ]]; then
		make PAPI="-DPAPI -lpapi" SIZE=$SSIZE $PROG
		printf "Using PAPI\n"
	else
		make SIZE=$SSIZE $PROG
	fi
}

while [[ $# -ge 1 ]]
do

KEY="$1"

case $KEY in
	-p|--program )
	PROG="$2"
	shift
	;;
	-s|--size )
		case "$2" in
		"MINI" )
		SIZE=512
		SSIZE="$2"
		shift
		;;
		"SMALL" )
		SIZE=1024
		SSIZE="$2"
		shift
		;;
		"MEDIUM" )
		SIZE=2048
		SSIZE="$2"
		shift
		;;
		"BIG" )
		SIZE=4096
		SSIZE="$2"
		shift
		;;
		"LARGE" )
		SIZE=8192
		SSIZE="$2"
		shift
		;;
		"GIANT" )
		SIZE=16384
		SSIZE="$2"
		shift
		;;
		"HUGE" )
		SIZE=32768
		SSIZE="$2"
		shift
		;;
		"WTF" )
		SIZE=65536
		SSIZE="$2"
		shift
		;;
		* )
		SSIZE="$2"
		shift
		;;
		esac
	;;
	--with-papi )
	USEPAPI=1
	;;
	-t|--threads )
	THREADS=$(( $2 + 0 ))
	shift
	;;
	--sudo )
	sudo su
	;;
	-h|--help|*)
	show_help
	exit
	;;
	esac
	shift
done

build

declare -a RESULTS

NOTSEQ=1

rm -rf results/$PROG.*

while [[ "$THREADS" -ge "2" ]]; do
	for i in $(seq 0 2);	do
		if [[ "$NOTSEQ" -eq "0" ]]; then
			if [[ "$i" -eq "0" ]]; then
				echo -e "\n\x1B[35m$THREADS threads:"
				continue
			fi
		fi
		SUM=0
		ALL=()
		RESULTS=()
		PAPI_L1_TCM=()
		PAPI_L2_TCM=()
		PAPI_L3_TCM=()
		PAPI_FP_INS=()
		PAPI_TOT_CYC=()
		PAPI_TOT_INS=()
		if [[ $USEPAPI -eq 1 ]]; then
			TPAPI_L1_TCM=0
			TPAPI_L2_TCM=0
			TPAPI_L3_TCM=0
			TPAPI_FP_INS=0
			TPAPI_TOT_CYC=0
			TPAPI_TOT_INS=0
		fi
		for j in $(seq 1 13); do
			ID=$(( 14 - j ))
			if [ "$j" -eq "1" ]; then
				if [[ $USEPAPI -ne 1 ]]; then
					MIN=$(./obj/$PROG $THREADS $i $j)
					MIN=$(( MIN + 0 ))
					MAX=$(( MIN + 0 ))
				else
					MIN=($(./obj/$PROG $THREADS $i $j))
					MIN=$(( ${MIN[-1]} + 0 ))
					MAX=$(( ${MIN[-1]} + 0 ))
				fi
			else
				if [[ $USEPAPI -ne 1 ]]; then
					TIMES[ID]=$(./obj/$PROG $THREADS $i $j)
					TIMES[ID]=$(( ${TIMES[$ID]} + 0 ))
					if [ "${TIMES[$ID]}" -le "$MIN" ]; then
						MIN=$ID
					fi
					if [ "${TIMES[$ID]}" -ge "$MAX" ]; then
						MAX=$ID
					fi
				else
					AUX=$(./obj/$PROG $THREADS $i $j)
					ALL=( $AUX )
					PAPI_L1_TCM[j]=$(( ${ALL[1]} + 0 ))
					PAPI_L2_TCM[j]=$(( ${ALL[2]} + 0 ))
					PAPI_L3_TCM[j]=$(( ${ALL[3]} + 0 ))
					PAPI_FP_INS[j]=$(( ${ALL[4]} + 0 ))
					PAPI_TOT_CYC[j]=$(( ${ALL[5]} + 0 ))
					PAPI_TOT_INS[j]=$(( ${ALL[6]} + 0 ))
					TIMES[ID]=$(( ${ALL[-1]} + 0 ))
					if [ "${TIMES[$ID]}" -le "$MIN" ]; then
						MIN=$ID
					fi
					if [ "${TIMES[$ID]}" -ge "$MAX" ]; then
						MAX=$ID
					fi
				fi
			fi
		done

		for j in $(seq 1 12); do
			if [ "${TIMES[$j]}" -ne "$MIN" ]; then
				if [ "${TIMES[$j]}" -ne "$MAX" ]; then
					SUM=$(( SUM + ${TIMES[$j]} + 0 ))
					if [[ $USEPAPI -eq 1 ]]; then
						for k in $(seq 1 ${#PAPI_L1_TCM[@]}); do
							TPAPI_L1_TCM=$(( ${PAPI_L1_TCM[$k]} + $TPAPI_L1_TCM + 0 ))
							TPAPI_L2_TCM=$(( ${PAPI_L2_TCM[$k]} + $TPAPI_L2_TCM + 0 ))
							TPAPI_L3_TCM=$(( ${PAPI_L3_TCM[$k]} + $TPAPI_L3_TCM + 0 ))
							TPAPI_FP_INS=$(( ${PAPI_FP_INS[$k]} + $TPAPI_FP_INS + 0 ))
							TPAPI_TOT_CYC=$(( ${PAPI_TOT_CYC[$k]} + $TPAPI_TOT_CYC + 0 ))
							TPAPI_TOT_INS=$(( ${PAPI_TOT_INS[$k]} + $TPAPI_TOT_INS + 0 ))
						done
						TPAPI_L1_TCM=$(( ${PAPI_L1_TCM[$k]} / 10 ))
						TPAPI_L2_TCM=$(( ${PAPI_L2_TCM[$k]} / 10 ))
						TPAPI_L3_TCM=$(( ${PAPI_L3_TCM[$k]} / 10 ))
						TPAPI_FP_INS=$(( ${PAPI_FP_INS[$k]} / 10 ))
						TPAPI_TOT_CYC=$(( ${PAPI_TOT_CYC[$k]} / 10 ))
						TPAPI_TOT_INS=$(( ${PAPI_TOT_INS[$k]} / 10 ))
					fi
				fi
			fi
		done

		DEC=$(bc -l <<< "$SUM / 100000000000")
		if [ "$i" -eq "0" ]; then
			SEQ=$DEC
			DEC=$(bc -l <<< "$DEC * 10")
			echo -e "\n\n\t\t\x1B[33mSequential:\t\t\t\x1B[31mTime: $DEC"
			echo -e "\t\t\x1B[36mTotal Cycles: \t\t\t\x1B[31m$TPAPI_TOT_CYC\x1B[36m"
			echo -e "\t\tTotal Instructions: \t\t\x1B[31m$TPAPI_TOT_INS\x1B[36m"
			echo -e "\t\tL1 Total Cache Miss: \t\t\x1B[31m$TPAPI_L1_TCM\x1B[36m"
			echo -e "\t\tL2 Total Cache Miss: \t\t\x1B[31m$TPAPI_L2_TCM\x1B[36m"
			echo -e "\t\tL3 Total Cache Miss: \t\t\x1B[31m$TPAPI_L3_TCM\x1B[36m"
			echo -e "\t\tFloat Point Instruction: \t\x1B[31m$TPAPI_FP_INS\x1B[0m"
			echo -e "\n\n\x1B[35m$THREADS threads:\n"
		else
			SPD=$(bc -l <<< "$SEQ / $DEC")
			DEC=$(bc -l <<< "$DEC * 10")
			if [ "$i" -eq "1" ]; then
				echo -e "\t\x1B[33mPthread:\t\t\t\x1B[31mTime: $DEC\t\x1B[36mSpeedup: $SPD"
				echo -e "\t\x1B[36mTotal Cycles: \t\t\t\x1B[31m$TPAPI_TOT_CYC\x1B[36m"
				echo -e "\tTotal Instructions: \t\t\x1B[31m$TPAPI_TOT_INS\x1B[36m"
				echo -e "\tL1 Total Cache Miss: \t\t\x1B[31m$TPAPI_L1_TCM\x1B[36m"
				echo -e "\tL2 Total Cache Miss: \t\t\x1B[31m$TPAPI_L2_TCM\x1B[36m"
				echo -e "\tL3 Total Cache Miss: \t\t\x1B[31m$TPAPI_L3_TCM\x1B[36m"
				echo -e "\tFloat Point Instruction: \t\x1B[31m$TPAPI_FP_INS\x1B[0m"
				printf "%s\t%s\t" "$THREADS" "$SPD" >> results/$PROG.dat
				printf "%s\t%s\t%s\t%s\n" "$TPAPI_L1_TCM" "$TPAPI_L2_TCM" "$TPAPI_L3_TCM" >> results/$PROG.pt.tcm
			else
				echo -e "\n\t\x1B[33mOpenMP:\t\t\t\t\x1B[31mTime: $DEC\t\x1B[36mSpeedup: $SPD"
				echo -e "\t\x1B[36mTotal Cycles: \t\t\t\x1B[31m$TPAPI_TOT_CYC\x1B[36m"
				echo -e "\tTotal Instructions: \t\t\x1B[31m$TPAPI_TOT_INS\x1B[36m"
				echo -e "\tL1 Total Cache Miss: \t\t\x1B[31m$TPAPI_L1_TCM\x1B[36m"
				echo -e "\tL2 Total Cache Miss: \t\t\x1B[31m$TPAPI_L2_TCM\x1B[36m"
				echo -e "\tL3 Total Cache Miss: \t\t\x1B[31m$TPAPI_L3_TCM\x1B[36m"
				echo -e "\tFloat Point Instruction: \t\x1B[31m$TPAPI_FP_INS\x1B[0m"
				printf "%s\n" "$SPD" >> results/$PROG.dat
				printf "%s\t%s\t%s\t%s\n" "$TPAPI_L1_TCM" "$TPAPI_L2_TCM" "$TPAPI_L3_TCM" >> results/$PROG.om.tcm
			fi
		fi
	done
	NOTSEQ=0
	THREADS=$(( $THREADS / 2 ))
done
echo -e "\x1B[0m\n"
printf "1\t1\t1\n0\t0\t0\n" >> results/$PROG.dat

gnuplot -c plot.gp $PROG $SIZE
