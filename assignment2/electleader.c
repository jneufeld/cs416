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

    /* Send initial election message. */
    char buffer[MSG_LEN];
    MPI_Status status;

    char *right_msg = pack_elec(id, 0, 0);
    int right_len   = strlen(right_msg) + 1;
    int right       = (rank + 1) % size;

    char *left_msg = pack_elec(id, 0, 0);
    int left_len   = strlen(left_msg) + 1;
    int left       = (rank + 1) % size;

    if(rank % 2)
    {
        /* Send left and right. */
        printf("[PROC-%d] Sending: msg(%s)\n", rank, right_msg);
        MPI_Send(right_msg, right_len, MPI_CHAR, right, MPI_TAG, MPI_COMM_WORLD);
        printf("[PROC-%d] Sending: msg(%s)\n", rank, left_msg);
        MPI_Send(left_msg, left_len, MPI_CHAR, left, MPI_TAG, MPI_COMM_WORLD);

        /* Receive right. */
        MPI_Recv(buffer, MSG_LEN, MPI_CHAR, MPI_ANY_SOURCE, MPI_TAG, MPI_COMM_WORLD, &status); 

        int rcv_id = unpack_elec_id(buffer);
        int rcv_k  = unpack_elec_k(buffer);
        int rcv_d  = unpack_elec_d(buffer);
        print_rcv_elec(rank, buffer, rcv_id, rcv_k, rcv_d);

        /* Receive left. */
        MPI_Recv(buffer, MSG_LEN, MPI_CHAR, MPI_ANY_SOURCE, MPI_TAG, MPI_COMM_WORLD, &status); 

        rcv_id = unpack_elec_id(buffer);
        rcv_k  = unpack_elec_k(buffer);
        rcv_d  = unpack_elec_d(buffer);
        print_rcv_elec(rank, buffer, rcv_id, rcv_k, rcv_d);
    }
    else
    {
        /* Receive right. */
        MPI_Recv(buffer, MSG_LEN, MPI_CHAR, MPI_ANY_SOURCE, MPI_TAG, MPI_COMM_WORLD, &status); 

        int rcv_id = unpack_elec_id(buffer);
        int rcv_k  = unpack_elec_k(buffer);
        int rcv_d  = unpack_elec_d(buffer);
        print_rcv_elec(rank, buffer, rcv_id, rcv_k, rcv_d);

        /* Receive left. */
        MPI_Recv(buffer, MSG_LEN, MPI_CHAR, MPI_ANY_SOURCE, MPI_TAG, MPI_COMM_WORLD, &status); 

        rcv_id = unpack_elec_id(buffer);
        rcv_k  = unpack_elec_k(buffer);
        rcv_d  = unpack_elec_d(buffer);
        print_rcv_elec(rank, buffer, rcv_id, rcv_k, rcv_d);

        /* Send left and right. */
        printf("[PROC-%d] Sending: msg(%s)\n", rank, right_msg);
        MPI_Send(right_msg, right_len, MPI_CHAR, right, MPI_TAG, MPI_COMM_WORLD);
        printf("[PROC-%d] Sending: msg(%s)\n", rank, left_msg);
        MPI_Send(left_msg, left_len, MPI_CHAR, left, MPI_TAG, MPI_COMM_WORLD);
    }

    /* Exit cleanly. */
    MPI_Finalize();
    return 0;
}


/*
 * Debugging to print a received election message.
 */
void print_rcv_elec(int rank, char *msg, int id, int k, int d)
{
    printf("[PROC-%d] Received: msg(%s), id(%d), k(%d), d(%d)\n", rank, msg, id, k, d);
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
int unpack_msg_num(char *msg, int start)
{
    int result = 0;
    int end = start + 1;

    while(msg[end] != '.' && msg[end] != '\0' && msg[end] != '\n')
    {
        end++;
    }

    int len = end - start;

    while(msg[start] != '.' && msg[start] != '\0' && msg[end] != '\n')
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
    return unpack_msg_num(msg, id_start);
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

    return unpack_msg_num(msg, k_start + 1);
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

    return unpack_msg_num(msg, d_start + 1);
}


/*
 * Package a reply message.
 */
char *pack_reply(int id, int k)
{
    int id_size = num_size(id);
    int k_size  = num_size(k);

    int total_length = strlen("reply..") + id_size + k_size;
    char *result = malloc((sizeof(char) * total_length) + 1);

    result[0] = 'r';
    result[1] = 'e';
    result[2] = 'p';
    result[3] = 'l';
    result[4] = 'y';
    result[5] = '.';

    int location = strlen("reply.");
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

    result[location] = '\0';

    return result;
}


/*
 * Unpack and return the ID from a reply message.
 */
int unpack_reply_id(char *msg)
{
    int id_start = strlen("reply.");
    return unpack_msg_num(msg, id_start);
}


/*
 * Unpack and return the k value from a reply message.
 */
int unpack_reply_k(char *msg)
{
    int id_start = strlen("reply.");
    int k_start  = id_start + 1;

    while(msg[k_start] != '.')
    {
        k_start++;
    }

    return unpack_msg_num(msg, k_start + 1);
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
