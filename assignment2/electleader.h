/* -----------------------------------------------------------------------------
 * Includes.
 * -------------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mpi.h"

/* -----------------------------------------------------------------------------
 * Definitions.
 * -------------------------------------------------------------------------- */
#define MPI_TAG 1234
#define MSG_LEN 64

/* -----------------------------------------------------------------------------
 * Function prototypes for general functions.
 * -------------------------------------------------------------------------- */
void check_args(int num_args);
int extract_pnum(char *pnum_arg);

/* -----------------------------------------------------------------------------
 * Function prototypes for packing/unpacking messages.
 * -------------------------------------------------------------------------- */
char *pack_elec(int id, int k, int d);
int num_size(int num);
int unpack_elec_id(char *msg);
int unpack_elec_num(char *msg, int start);
int unpack_elec_id(char *msg);
int unpack_elec_k(char *msg);
int unpack_elec_d(char *msg);

