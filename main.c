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
 * \version 0.0.1
 * 
 * \date 2025/06/23
 * 
 * \defgroup grs-fft GRS FFT
 * \{
 */

#include <math.h>

#include <fftw3.h>

#include "version.h"

#define GRS_FFT_DEFAULT_SIZE        16384

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

int main(int argc, char *argv[])
{
    const int N = GRS_FFT_DEFAULT_SIZE; /* Number of samples (power of 2 recommended) */
    fftw_complex *iq_samples = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * N);

    /* Fill iq_samples with your data (real part = I, imag part = Q) */
    int i = 0;
    for(i = 0; i < N; i++)
    {
        iq_samples[i][0] = /* Your I value */;
        iq_samples[i][1] = /* Your Q value */;
    }

    /* Compute windowed FFT */
    compute_fft(iq_samples, N);

    // Process FFT results here...

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

/** \} End of grs-fft group */
