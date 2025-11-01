/*
 * main.c
 * 
 * Copyright The GRS FFT Contributors.
 * 
 * This file is part of GRS FFT.
 * 
 * GRS FFT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * GRS FFT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GRS FFT. If not, see <http://www.gnu.org/licenses/>.
 * 
 */

/**
 * \brief GRS FFT calculator.
 * 
 * \author Gabriel Mariano Marcelino <gabriel.mm8@gmail.com>
 * 
 * \version 0.0.3
 * 
 * \date 2025/06/23
 * 
 * \defgroup grs-fft GRS FFT
 * \{
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <errno.h>

#include <zmq.h>
#include <fftw3.h>

#include "version.h"

#ifndef M_PI
#define M_PI (3.14159265358979323846264338327950288)
#endif

#define GRS_FFT_DEFAULT_BUF_LEN     300000
#define GRS_FFT_DEFAULT_SIZE        16384

bool verbose = false;

void *zmq_context;
void *zmq_subscriber;

/**
 * \brief IQ sample type.
 */
typedef struct
{
    float i;    /**< In-phase sample. */
    float q;    /**< Quadrature sample. */
} iq_t;

/**
 * \brief .
 *
 * \param[in,out] samples .
 *
 * \param[in] N .
 *
 * \return None.
 */
void apply_hann_window(fftw_complex *samples, int N);

/**
 * \brief .
 *
 * \param[in,out] iq_samples .
 *
 * \param[in] N .
 *
 * \return None.
 */
void compute_fft(fftw_complex *iq_samples, int N);

/**
 * \brief .
 *
 * \return None.
 */
void cleanup(void);

int main(int argc, char *argv[])
{
    /* Initialize ZMQ */
    zmq_context = zmq_ctx_new();
    if (!zmq_context)
    {
        if (verbose)
        {
            fprintf(stderr, "Failed to create ZMQ context!\n\r");
        }

        exit(EXIT_FAILURE);
    }

    /* Set IO threads to 1 (reduces context switching) */
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 1);

    zmq_subscriber = zmq_socket(zmq_context, ZMQ_SUB);
    if (!zmq_subscriber )
    {
        if (verbose)
        {
            fprintf(stderr, "Failed to create ZMQ publisher socket: %s\n\r", zmq_strerror(errno));
        }

        cleanup();

        exit(EXIT_FAILURE);
    }

    /* Enable high water mark at 1M messages */
    int hwm = 1000000;
    zmq_setsockopt(zmq_subscriber, ZMQ_RCVHWM, &hwm, sizeof(hwm));

    /* 2MB send buffer */
    int buf = 2097152;
    zmq_setsockopt(zmq_subscriber, ZMQ_RCVBUF, &buf, sizeof(buf));

    /* Connect to publisher (change to your publisher's address) */
    int rc = zmq_connect(zmq_subscriber, "tcp://localhost:5556");
    if (rc != 0)
    {
        fprintf(stderr, "Error connecting to ZMQ publisher: %s\n\r", zmq_strerror(errno));

        exit(EXIT_FAILURE);
    }

    /* Subscribe to all messages (empty filter) */
    zmq_setsockopt(zmq_subscriber, ZMQ_SUBSCRIBE, "", 0);

    const int N = GRS_FFT_DEFAULT_SIZE; /* Number of samples (power of 2 recommended) */
    fftw_complex *iq_samples = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    while(1)
    {
        iq_t iq_buf[GRS_FFT_DEFAULT_SIZE] = {0};

        /* Receive IQ samples */
        int iq_len = zmq_recv(zmq_subscriber, iq_buf, sizeof(iq_buf), 0);

        if (iq_len == -1)
        {
            fprintf(stderr, "Error receiving data: %s\n\r", zmq_strerror(errno));

            continue;
        }
        else
        {
            int num_samples = iq_len/sizeof(iq_t);
            printf("Samples: %d\n\r", num_samples);
            for(int i = 0; i < num_samples; i++)
            {
                printf("IQ = [ %f, %f ]\n\r", iq_buf[i].i, iq_buf[i].q);

                iq_samples[i][0] = iq_buf[i].i;
                iq_samples[i][1] = iq_buf[i].q;
            }

            /* Compute windowed FFT */
            compute_fft(iq_samples, N);
        }
    }

    fftw_free(iq_samples);

    return 0;
}

void apply_hann_window(fftw_complex *samples, int N)
{
    int i = 0;
    for(i = 0; i < N; i++)
    {
        /* Hann window coefficient calculation */
        double multiplier = 0.5 * (1 - cos(2 * M_PI * i / (N - 1)));

        /* Apply to both real (I) and imaginary (Q) components */
        samples[i][0] *= multiplier;    /* I component */
        samples[i][1] *= multiplier;    /* Q component */
    }
}

void compute_fft(fftw_complex *iq_samples, int N)
{
    /* Apply Hann window first */
    apply_hann_window(iq_samples, N);

    /* Create FFT plan */
    fftw_plan plan = fftw_plan_dft_1d(N, iq_samples, iq_samples, FFTW_FORWARD, FFTW_ESTIMATE);

    /* Execute FFT */
    fftw_execute(plan);

    /* Clean up */
    fftw_destroy_plan(plan);
}

void cleanup(void)
{
    if (zmq_subscriber)
    {
        zmq_close(zmq_subscriber);
    }

    if (zmq_context)
    {
        zmq_ctx_destroy(zmq_context);
    }
}

/** \} End of grs-fft group */
