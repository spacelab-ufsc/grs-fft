#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <zmq.h>

typedef struct {
    float i;
    float q;
} iq_t;

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <samples_file_name> <num_bytes_to_send>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];
    long NBYTES = atol(argv[2]);

    if (NBYTES <= 0) {
        fprintf(stderr, "num_bytes_to_send must be positive.\n");
        return 1;
    }
    if (NBYTES % sizeof(iq_t) != 0) {
        fprintf(stderr, "NBYTES must be a multiple of %lu (sizeof(iq_t)) structure.\n",
                sizeof(iq_t));
        return 1;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("fopen");
        return 1;
    }

    void *ctx = zmq_ctx_new();
    void *pub = zmq_socket(ctx, ZMQ_PUB);

    if (zmq_bind(pub, "tcp://127.0.0.1:5556") != 0) {
        perror("zmq_bind");
        fclose(fp);
        zmq_close(pub);
        zmq_ctx_term(ctx);
        return 1;
    }

    uint8_t *buffer = malloc(NBYTES);
    if (!buffer) {
        fprintf(stderr, "malloc failed\n");
        fclose(fp);
        zmq_close(pub);
        zmq_ctx_term(ctx);
        return 1;
    }
    zmq_sleep(0.2); /* Gives time to the publisher connect with the subscriber before broadcasting data */


    while (1) {
        size_t n = fread(buffer, 1, NBYTES, fp);

        if (n == 0)
            break;

        n -= (n % sizeof(iq_t));

        if (n == 0)
            break;

        zmq_send(pub, buffer, n, 0);
    }

    free(buffer);
    fclose(fp);
    zmq_close(pub);
    zmq_ctx_term(ctx);

    return 0;
}
