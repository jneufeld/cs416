/*
 * electleader.c
 *
 * CPSC 416 Assignment 1
 * March 8, 2013
 *
 * Student Name: Jonathan Neufeld
 * Student Number: 30671093
 * CS ID: p9d8
 */

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
#define MPI_TAG     1234
#define MSG_LEN     64
#define ELECTION    1
#define REPLY       2

/* -----------------------------------------------------------------------------
 * Function prototypes for general functions.
 * -------------------------------------------------------------------------- */
void check_args(int num_args);
int extract_pnum(char *pnum_arg);

/* -----------------------------------------------------------------------------
 * Function prototypes for the election.
 * -------------------------------------------------------------------------- */
void init_election(int size,
    int rank,
    int id,
    int *rcv_id,
    int *rcv_k,
    int *rcv_d,
    int *rcv_id2,
    int *rcv_k2,
    int *rcv_d2);

/* -----------------------------------------------------------------------------
 * Function prototypes for packing/unpacking messages.
 * -------------------------------------------------------------------------- */
char *pack_elec(int id, int k, int d);
char *pack_reply(int id, int k);
int unpack_msg_num(char *msg, int start);
int num_size(int num);
int unpack_elec_id(char *msg);
int unpack_elec_k(char *msg);
int unpack_elec_d(char *msg);
int unpack_reply_id(char *msg);
int unpack_reply_k(char *msg);
void print_rcv_elec(int rank, char *msg, int id, int k, int d, int sender);
void print_rcv_reply(int rank, char *msg, int id, int k, int sender);
int power(int k, int n);

