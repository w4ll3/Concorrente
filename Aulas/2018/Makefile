CC=gcc
MPICC=mpicc
IDIR=include
SRC=src
#LPAPI=/usr/local/lib/libpapi.a
#LBPA=-lpapi
LINC=-I/usr/include -I/usr/local/include $(LPAPI)
UTIL=$(SRC)/util.c
CFLAGS=-std=gnu99 -Wall -fopenmp -I$(IDIR) $(LINC) -O3 -lm -lpthread -pthread -lrt $(LBPA)
ODIR=obj
IPS=ips

dir:
	mkdir -p $(ODIR)

mm.s: dir
	$(CC) $(UTIL) $(SRC)/$@.c -o $(ODIR)/$@ $(CFLAGS) $(PAPI)

mm.t: dir
	$(CC) $(UTIL) $(SRC)/$@.c -o $(ODIR)/$@ $(CFLAGS) $(PAPI)
	
mpi.mm: dir
	$(MPICC) $(UTIL) $(SRC)/$@.c -o $(ODIR)/$@ $(CFLAGS) $(PAPI)
	./copy.sh $(IPS) $@

rbsor.t: dir
	$(CC) $(UTIL) $(SRC)/$@.c -o $(ODIR)/$@ $(CFLAGS) $(PAPI)
	
stack.t: dir
	$(CC) $(UTIL) $(SRC)/$@.c -o $(ODIR)/$@ $(CFLAGS) $(PAPI)

.PHONY: clean

clean:
	rm -f $(ODIR)/* $(ODIR)/* *~ core $(INCDIR)/*~
