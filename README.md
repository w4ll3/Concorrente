# Concorrente
## Available programs

- Two matrix multiplication (2mm)
- Matrix Multiplication (mm)
- Red-Black SOR (rbsor)
- LU Decomposition (ludec)

## Building

To build any of the previous programs use `make program` where program is the desired one.
To modify the problem size you can pass to make the desired size by `make program SIZE=SIZE`, in the header _util.h_.
The available sizes are:
- DEBUG  (32)
- MINI   (512)
- SMALL  (1024)
- MEDIUM (2048)
- BIG    (4096)
- LARGE  (8192)
- GIANT  (16384)
- HUGE   (32768)
- WTF    (65536)

If you want to make them all use `make all`.

## Running

There is a bash script _speedup.sh_ which requires three arguments:
1. _Program name_
2. _Number of threads to start with_
3. _Size to run the program_

Once all defined use `./speedup.sh program threads size` and be happy.
