#include "electleader.h"

/*
 * Run electleader.
 */
int main(int argc, char *argv[])
{
    check_args(argc);
    int pnum = extract_pnum(argv[1]);
    int rank, size;

    /* Initialize our MPI stuffs. */
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("I am %d of %d\n", rank, size);

    /* Exit cleanly. */
    MPI_Finalize();
    return 0;
}


/*
 * Ensure arguments to program are valid. Exit if not given a single int.
 */
void check_args(int num_args)
{
    const char *usage = "mpiexec -n NUM ./electleader PNUM";

    if(num_args != 2)
    {
        printf("ERROR: Please provide the right number of arguments.\n");
        printf("Usage: %s\n", usage);
        exit(1);
    }
}


/*
 * Create an int from argument given to program from command line.
 */
int extract_pnum(char *pnum_arg)
{
    int pnum = atoi(pnum_arg);
    return pnum;
}
