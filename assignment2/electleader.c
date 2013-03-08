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
    char *send_msg = pack_elec(id, 0, 1);
    printf("[PROC-%d] send_msg=%s\n", rank, send_msg);
    printf("[PROC-%d] unpack_id=%d\n", rank, unpack_elec_id(send_msg));
    printf("[PROC-%d] unpack_k=%d\n", rank, unpack_elec_k(send_msg));
    printf("[PROC-%d] unpack_d=%d\n", rank, unpack_elec_d(send_msg));

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
    char *result = malloc((sizeof(char) * total_length) + 1);

    result[0] = 'e';
    result[1] = 'l';
    result[2] = 'e';
    result[3] = 'c';
    result[4] = '.';

    int location = strlen("elec.");
    int pos = location + id_size - 1;

    if(id == 0)
    {
        result[location++] = '0';
    }
    else
    {
        while(id > 0)
        {
            int digit = id % 10;
            result[pos] = '0' + digit;
            id /= 10;
            location++;
            pos--;
        }
    }

    result[location++] = '.';

    pos = location + k_size - 1;
    if(k == 0)
    {
        result[location++] = '0';
    }
    else
    {
        while(k > 0)
        {
            int digit = k % 10;
            result[pos] = '0' + digit;
            k /= 10;
            location++;
            pos--;
        }
    }

    result[location++] = '.';

    pos = location + d_size - 1;
    if(d == 0)
    {
        result[location++] = '0';
    }
    else
    {
        while(d > 0)
        {
            int digit = d % 10;
            result[pos] = '0' + digit;
            d /= 10;
            location++;
            pos--;
        }
    }

    result[location] = '\0';

    return result;
}


/*
 * Unpack and return a number from an election message.
 */
int unpack_elec_num(char *msg, int start)
{
    int result = 0;
    int end = start + 1;

    while(msg[end] != '.' && msg[end] != '\0')
    {
        end++;
    }

    int len = end - start;

    while(msg[start] != '.' && msg[start] != '\0')
    {
        int digit = msg[start] - '0';
        digit = digit * pow(10, len - 1);
        result += digit;
        start++;
        len--;
    }

    return result;
}


/*
 * Unpack and return the ID from an election message.
 */
int unpack_elec_id(char *msg)
{
    int id_start = strlen("elec.");
    return unpack_elec_num(msg, id_start);
}


/*
 * Unpack and return the k value from an election message.
 */
int unpack_elec_k(char *msg)
{
    int id_start = strlen("elec.");
    int k_start  = id_start + 1;

    while(msg[k_start] != '.')
    {
        k_start++;
    }

    return unpack_elec_num(msg, k_start + 1);
}


/*
 * Unpack and return the d value from an election message.
 */
int unpack_elec_d(char *msg)
{
    int id_start = strlen("elec.");
    int k_start  = id_start + 1;

    while(msg[k_start] != '.')
    {
        k_start++;
    }

    int d_start = k_start + 1;

    while(msg[d_start] != '.')
    {
        d_start++;
    }

    return unpack_elec_num(msg, d_start + 1);
}


/*
 * Determine the number of digits in an int.
 */
int num_size(int num)
{
    int size = 0;

    if(num == 0)
    {
        size = 1;
    }
    else
    {
        while(num > 0)
        {
            num /= 10;
            size++;
        }
    }

    return size;
}
