#pragma once
#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include <mpir.h>
extern __global__ void CUDAFractalWorker(mpf_t leftX, mpf_t topI, mpf_t pixelCoordinateDelta, mpf_t SuperSampleCoordinateDelta, int frameWidth, int frameHeight, int MaxIterations, int SampleRate, int Precision, uint32_t *HostIterations, double *HostMagnitudes);