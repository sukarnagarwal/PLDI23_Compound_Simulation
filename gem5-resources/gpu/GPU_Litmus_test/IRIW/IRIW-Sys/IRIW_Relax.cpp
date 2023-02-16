/*
    m5threads, a pthread library for the M5 simulator
    Copyright (C) 2009, Stanford University

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
    
    
    
Litmus Test Developed by 

Sukarn Agarwal (Email: sagarwa2@ed.ac.uk)
Nicolai Oswald (Email: nicolai.oswald@ed.ac.uk)    
    
    
*/




#include <iostream>
#include <thread>
#include <condition_variable>
#include <cstdlib>
#include <vector>
#include <atomic>
#include "hip/hip_runtime.h"
#include "hip/hcc_detail/hip_atomic.h"


#define GPU_ACTIVE
#define BLOCK_PARALLELISM

#define RAND_RANGE 100
#define DEF_RUNS 2048   // Specifies the default number of runs

using namespace std;




struct s_var
{
    alignas(128) unsigned int a;
    alignas(128) unsigned int b;
};

struct r_gpu0
{
    alignas(128) unsigned int r0;
};

struct r_gpu1
{
    alignas(128) unsigned int r0;
};


struct r_gpu2
{
    alignas(128) unsigned int r0;
};

struct r_gpu3
{
    alignas(128) unsigned int r0;
};



template <typename T>
__device__ void 
seta(T* delay, struct s_var* var)
{
#pragma GCC push_options
#pragma GCC optimize ("O0")	
    // Variable delay
    for (int j = 0; j < (*delay); j++);
    var->a = 1;
#pragma GCC pop_options 

}

template <typename T>
__device__ void 
setb(T* delay, struct s_var* var)
{
#pragma GCC push_options
#pragma GCC optimize ("O0")	
    // Variable delay
    for (int j = 0; j < (*delay); j++);
    var->b = 1;
#pragma GCC pop_options 

}

template <typename T>
__device__ void 
loadab(T* delay, struct s_var* var, struct r_gpu0* res1, struct r_gpu1* res2)
{
#pragma GCC push_options
#pragma GCC optimize ("O0")
     res2->r0 = var->b;
     // Variable Delay	
    for (int j = 0; j < (*delay); j++);
     res1->r0 = var->a;						
     res2->r0 = var->b;
#pragma GCC pop_options 
}

template <typename T>
__device__ void 
loadba(T* delay, struct s_var* var, struct r_gpu2* res1, struct r_gpu3* res2)
{
#pragma GCC push_options
#pragma GCC optimize ("O0")
     res2->r0 = var->a;	
    // Variable Delay
    for (int j = 0; j < (*delay); j++);
     res1->r0 = var->b;      									
     res2->r0 = var->a;
#pragma GCC pop_options 

}



template <typename T>
__global__ void
IRIW(T* delay1, T* delay2, T* delay3, T* delay4,  struct s_var* var, struct r_gpu0* res1, struct r_gpu1* res2, struct r_gpu2* res3, struct r_gpu3* res4, unsigned N)
{
     
    int i = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);
#pragma GCC push_options
#pragma GCC optimize ("O0") 
    	if(i < 4*N) {
    		if((hipBlockIdx_x) % 4 == 0)
    			seta(&delay1[i/4], (var + (i/4)));
    		else if ((hipBlockIdx_x) % 4 == 1)
    			loadab(&delay2[i/4], (var + (i/4)), (res1 + (i/4)), (res2 + (i/4)));	
    		else if ((hipBlockIdx_x) % 4 == 2)	
    			loadba(&delay3[i/4], (var + (i/4)), (res3 + (i/4)), (res4 + i/4));
    		else 
    			setb(&delay4[i/4], (var + (i/4)));		
    	}
#pragma GCC pop_options        	
}




int check_output(struct r_gpu0* v_cpu0, struct r_gpu1* v_gpu0, struct r_gpu2* v_cpu1, struct r_gpu3* v_gpu1, unsigned t_range)
{
    unsigned res_cpu0_gpu0 = 0;
    unsigned res_cpu1_gpu0 = 0;
    unsigned res_cpu0_gpu1 = 0;
    unsigned res_cpu1_gpu1 = 0;
    unsigned res_cpu2_gpu2 = 0;
    unsigned rst_cpu0_gpu0 = 0;
    unsigned rst_cpu1_gpu0 = 0;
    unsigned rst_cpu0_gpu1 = 0;
    unsigned rst_cpu1_gpu1 = 0;
    unsigned rst_cpu2_gpu2 = 0;
    int pass = 0;
    int failure = 0;

    for (auto i = 0; i < t_range; ++i) {
        if ((v_cpu0[i].r0 == 0) && (v_gpu0[i].r0 == 0)) {
            res_cpu0_gpu0++;
        } else if ((v_cpu0[i].r0 == 1) && (v_gpu0[i].r0 == 0)) {
            res_cpu1_gpu0++;
        } else if ((v_cpu0[i].r0 == 0) && (v_gpu0[i].r0 == 1)) {
            res_cpu0_gpu1++;
        } else if ((v_cpu0[i].r0 == 1) && (v_gpu0[i].r0 == 1)) {
            res_cpu1_gpu1++;
        }
        else
        {
        	res_cpu2_gpu2++;
        }
        
        if ((v_cpu1[i].r0 == 0) && (v_gpu1[i].r0 == 0)) {
            rst_cpu0_gpu0++;
        } else if ((v_cpu1[i].r0 == 1) && (v_gpu1[i].r0 == 0)) {
            rst_cpu0_gpu1++;
        } else if ((v_cpu1[i].r0 == 0) && (v_gpu1[i].r0 == 1)) {
            rst_cpu1_gpu0++;
        } else if ((v_cpu1[i].r0 == 1) && (v_gpu1[i].r0 == 1)) {
            rst_cpu1_gpu1++;
        }
        else
        {
        	rst_cpu2_gpu2++;
        }
        
        if ((v_cpu0[i].r0 == 1) && (v_gpu0[i].r0 == 0) && (v_cpu1[i].r0 == 1) && (v_gpu1[i].r0 == 0)) {
    		failure++;
   	 }
        else
        {
        	pass++;
        }
    }
        cout << "Pass Test: " << pass << endl;
	cout << "Failure Test: " << failure << endl;
    
    if ((!res_cpu1_gpu0) && (!rst_cpu0_gpu1)) {
        cout << "Success!" << endl;
        cout << "Count (a:0 and b:0): " << res_cpu0_gpu0 << endl;
        cout << "Count (a:1 and b:0): " << res_cpu1_gpu0 << endl;
        cout << "Count (a:0 and b:1): " << res_cpu0_gpu1 << endl;
        cout << "Count (a:1 and b:1): " << res_cpu1_gpu1 << endl;
        cout << "Count (r0:2 and r1:2): " << res_cpu2_gpu2 << endl; 
        cout << "Count (a:0 and b:0): " << rst_cpu0_gpu0 << endl;
        cout << "Count (a:1 and b:0): " << rst_cpu1_gpu0 << endl;
        cout << "Count (a:0 and b:1): " << rst_cpu0_gpu1 << endl;
        cout << "Count (a:1 and b:1): " << rst_cpu1_gpu1 << endl;
        cout << "Count (r0:2 and r1:2): " << rst_cpu2_gpu2 << endl;
        //cout << "Num Valid test: " << (t_range - res_cpu0_gpu1) << endl;
        //cout << "Num Invalid test: " << res_cpu0_gpu1 << endl;
        return 0;
    } else {
        cout << "Fail!" << endl;
        cout << "Count (a:0 and b:0): " << res_cpu0_gpu0 << endl;
        cout << "Count (a:1 and b:0): " << res_cpu1_gpu0 << endl;
        cout << "Count (a:0 and b:1): " << res_cpu0_gpu1 << endl;
        cout << "Count (a:1 and b:1): " << res_cpu1_gpu1 << endl;
        cout << "Count (r0:2 and r1:2): " << res_cpu2_gpu2 << endl;
        cout << "Count (a:0 and b:0): " << rst_cpu0_gpu0 << endl;
        cout << "Count (a:1 and b:0): " << rst_cpu1_gpu0 << endl;
        cout << "Count (a:0 and b:1): " << rst_cpu0_gpu1 << endl;
        cout << "Count (a:1 and b:1): " << rst_cpu1_gpu1 << endl;
        cout << "Count (r0:2 and r1:2): " << rst_cpu2_gpu2 << endl;
        //cout << "Num Valid test: " << (t_range - res_cpu0_gpu1) << endl;
        //scout << "Num Invalid test: " << res_cpu0_gpu1 << endl;
        return 2;
    }
	
}


void check_ab(struct s_var* var, unsigned t_range)
{
    unsigned res_cpu0_gpu0 = 0;
    unsigned res_cpu1_gpu0 = 0;
    unsigned res_cpu0_gpu1 = 0;
    unsigned res_cpu1_gpu1 = 0;

    for (auto i = 0; i < t_range; ++i) {
        if ((var[i].a == 0) && (var[i].b == 0)) {
            res_cpu0_gpu0++;
        } else if ((var[i].a == 1) && (var[i].b == 0)) {
            res_cpu1_gpu0++;
        } else if ((var[i].a == 0) && (var[i].b == 1)) {
            res_cpu0_gpu1++;
        } else {
            res_cpu1_gpu1++;
        }
    }
    cout << "Count (a:0 and b:0): " << res_cpu0_gpu0 << endl;
        cout << "Count (a:1 and b:0): " << res_cpu1_gpu0 << endl;
        cout << "Count (a:0 and b:1): " << res_cpu0_gpu1 << endl;
        cout << "Count (a:1 and b:1): " << res_cpu1_gpu1 << endl;

}


int main(int argc, char* argv[])
{
    unsigned num_iterations;
    if (argc == 1) {
        num_iterations = DEF_RUNS; 
    }
    else if (argc == 2) {
        num_iterations = atoi(argv[1]);
        if (num_iterations <= 0) {
            cerr << "Usage: " << argv[0] << " [num_values]" << endl;
            return 1;
        }
    }
    else {
        cerr << "Usage: " << argv[0] << " [num_values]" << endl;
        return 1;
    }
    
    
    struct s_var* var = (struct s_var*)malloc(sizeof(struct s_var) * num_iterations);
    // Initialize shared vars
    for (auto i = 0; i < num_iterations; ++i) {
        var[i].a = 0;
        var[i].b = 0;
    }

    // Allocate output variables for cpu
    struct r_gpu0* gpu_res1 = (struct r_gpu0*)malloc(sizeof(struct r_gpu0) * num_iterations);
    // Initialize cpu result
    for (auto i = 0; i < num_iterations; ++i) {
        gpu_res1[i].r0 = 2;
    }

    // Allocate output variables for gpu
     struct r_gpu1* gpu_res2 = (struct r_gpu1*)malloc(sizeof(struct r_gpu1) * num_iterations);
    // Initialize gpu result
    for (auto i = 0; i < num_iterations; ++i) {
        gpu_res2[i].r0 = 2;
    }

    struct r_gpu2* gpu_res3 = (struct r_gpu2*)malloc(sizeof(struct r_gpu2) * num_iterations);
    // Initialize cpu result
    for (auto i = 0; i < num_iterations; ++i) {
        gpu_res3[i].r0 = 2;
    }

    // Allocate output variables for gpu
     struct r_gpu3* gpu_res4 = (struct r_gpu3*)malloc(sizeof(struct r_gpu3) * num_iterations);
    // Initialize gpu result
    for (auto i = 0; i < num_iterations; ++i) {
        gpu_res4[i].r0 = 2;
    }





    // Number of CPU threads == GPU threads
    // Allocate vectors for random numbers
    unsigned* gpu_rand1 = (unsigned*)malloc(sizeof(unsigned) * num_iterations);
    unsigned* gpu_rand2 = (unsigned*)malloc(sizeof(unsigned) * num_iterations);
    unsigned* gpu_rand3 = (unsigned*)malloc(sizeof(unsigned) * num_iterations);
    unsigned* gpu_rand4 = (unsigned*)malloc(sizeof(unsigned) * num_iterations);
    //=======================================================================
    //	Generate random numbers
    //======================================================================= 
    // Number of CPU threads == GPU threads
    // Allocate vectors for random numbers
    for (int iter = 0; iter < num_iterations; iter++) {
    	     unsigned rand_val = std::rand() % RAND_RANGE;
   		gpu_rand1[iter] = rand_val;
            // Generate random time offset value
             rand_val = std::rand() % RAND_RANGE;
            //cout << "GPU_T:" << i << " Rand: " << rand_val << endl;
            gpu_rand2[iter] = rand_val;
            
            rand_val = std::rand() % RAND_RANGE;
   		gpu_rand3[iter] = rand_val;
   		
   	     rand_val = std::rand() % RAND_RANGE;
   		gpu_rand4[iter] = rand_val;	
   		
    }

    std::cout << "Random thread launch delays generated" << endl;

    //=======================================================================
    //	Execute Runs
    //=======================================================================  
        unsigned blocks = 8192;
        unsigned threadsPerBlock = 1;
        cout << "Launching kernel: 1 "<< endl;
       hipLaunchKernelGGL(IRIW, dim3(blocks), dim3(threadsPerBlock), 0, 0,
            gpu_rand1, gpu_rand2, gpu_rand3, gpu_rand4,
            var,
            gpu_res1, gpu_res2, gpu_res3, gpu_res4, num_iterations);             
        cout << "Waiting for other threads to complete: "<< endl; 
    //=======================================================================
     //	Start concurrent execution
     //=======================================================================
     
        std::cout << "All threads executing" << endl;
        // Extra Delay
#pragma GCC push_options
#pragma GCC optimize ("O0")	   
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
        for (long unsigned int j = 0; j < 1000000; j++);
#pragma GCC pop_options 
	 hipDeviceSynchronize();
	 
	 //=======================================================================
    // free sync and random memory
    //=======================================================================
    std::free(gpu_rand1);
    std::free(gpu_rand2);
    std::free(gpu_rand3);
    std::free(gpu_rand4);
    

    //=======================================================================
    // Check the results
    //=======================================================================
    cout << "Validating..." << flush;
    int res = check_output(gpu_res1, gpu_res2, gpu_res3, gpu_res4, num_iterations);
    check_ab(var, num_iterations);
    free(gpu_res1);
    free(gpu_res2);
    free(gpu_res3);
    free(gpu_res4);
    free(var);

    return res;
    
    
}    

