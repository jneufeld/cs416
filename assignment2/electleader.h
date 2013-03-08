#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mpi.h"

#define MPI_TAG 1234
#define MSG_LEN 64

void check_args(int num_args);
int extract_pnum(char *pnum_arg);
char *pack_elec(int id, int k, int d);
int num_size(int num);

