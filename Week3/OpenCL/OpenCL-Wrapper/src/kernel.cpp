#include "kernel.hpp" // note: unbalanced round brackets () are not allowed and string literals can't be arbitrarily long, so periodically interrupt with )+R(
string opencl_c_container() { return R( // ########################## begin of OpenCL C code ####################################################################



kernel void add_kernel(global float* A, global float* B, global float* C) { // equivalent to "for(uint n=0u; n<N; n++) {", but executed in parallel
	const uint n = get_global_id(0);
	C[n] = A[n]+B[n];
}

kernel void mul_kernel(global float* A, global float* B, global float* C) {
	// TASK 1 CODE BEGINS HERE
	const uint n = get_global_id(0);
	C[n] = A[n]*B[n];
	// TASK 1 CODE ENDS HERE
}

kernel void matMul (__global float* A, __global float *B, __global float *C, int aCol, int cRow, int cCol) {
	// TASK 2 CODE BEGINS HERE
    const uint index = get_global_id(0);
    int row = index / cCol;
    int col = index % cCol;

    // Check if the current thread's indices are within the bounds of matrix C
    if(row < cRow && col < cCol) {
        float4 sum = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
        for(int k = 0; k < aCol; k += 4) {
            float4 aVec = vload4(0, &A[row * aCol + k]);
            float4 bVec = (float4)(B[k * cCol + col], B[(k+1) * cCol + col], B[(k+2) * cCol + col], B[(k+3) * cCol + col]);
            sum += aVec * bVec;
        }
        // Reduce the sum vector to a single float value
        float finalSum = sum.s0 + sum.s1 + sum.s2 + sum.s3;
        C[row * cCol + col] = finalSum;
    }
	// TASK 2 CODE ENDS HERE
}


);} // ############################################################### end of OpenCL C code #####################################################################
