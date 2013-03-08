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

    int id = ((rank + 1) * pnum) % size;
    pack_elec(id, 15, 240);

    /* Send message left and right. */
    /*
    char *right_msg = "Sent from right\n";
    int right_len   = strlen(right_msg);
    int right       = (rank + 1) % size;

    char buffer[MSG_LEN];
    MPI_Status status;

    if(rank % 2)
    {
        MPI_Send(right_msg,
            right_len,
            MPI_CHAR,
            right,
            MPI_TAG,
            MPI_COMM_WORLD);

        MPI_Recv(buffer,
            MSG_LEN,
            MPI_CHAR,
            MPI_ANY_SOURCE,
            MPI_TAG,
            MPI_COMM_WORLD,
            &status); 

        printf("Received: %s\n", buffer);
    }
    else
    {
        MPI_Recv(buffer,
            MSG_LEN,
            MPI_CHAR,
            MPI_ANY_SOURCE,
            MPI_TAG,
            MPI_COMM_WORLD,
            &status); 

        printf("Received: %s\n", buffer);

        MPI_Send(right_msg,
            right_len,
            MPI_CHAR,
            right,
            MPI_TAG,
            MPI_COMM_WORLD);
    }
    */

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


/*
 * Package an election message.
 */
char *pack_elec(int id, int k, int d)
{
    int id_size = num_size(id);
    int k_size  = num_size(k);
    int d_size  = num_size(d);

    int total_length = strlen("elec...") + id_size + k_size + d_size;
    char *result = malloc(sizeof(char) * total_length);

    result[0] = 'e';
    result[1] = 'l';
    result[2] = 'e';
    result[3] = 'c';
    result[4] = '.';

    int location = strlen("elec.");

    if(id == 0)
    {
        result[location++] = '0';
    }
    else
    {
        while(id > 0)
        {
            int digit = id % 10;
            result[location++] = '0' + digit;
            id /= 10;
        }
    }

    result[location++] = '.';

    while(k > 0)
    {
        int digit = k % 10;
        result[location++] = '0' + digit;
        k /= 10;
    }

    result[location++] = '.';

    while(d > 0)
    {
        int digit = d % 10;
        result[location++] = '0' + digit;
        d /= 10;
    }

    printf("[DEBUG] msg = %s\n", result);

    return result;
}


/*
 * Determine the number of digits in an int.
 */
int num_size(int num)
{
    int size = 0;

    while(num > 0)
    {
        num /= 10;
        size++;
    }

    return size;
}
