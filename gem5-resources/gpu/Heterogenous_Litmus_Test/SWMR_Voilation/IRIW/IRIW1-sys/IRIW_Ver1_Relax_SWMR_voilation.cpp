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



#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <atomic>
#include <sys/time.h>
#include <iostream>
#include "hip/hip_runtime.h"
#include "hip/hcc_detail/hip_atomic.h"


#define RAND_RANGE 50

#define DEF_RUNS 4096    // Specifies the default number of runs
#define DEF_THREADS 8



struct s_var
{
    alignas(128) unsigned int a;
    alignas(128) unsigned int b;
};

struct r_cpu0
{
    alignas(128) unsigned int r0;
};

struct r_gpu0
{
    alignas(128) unsigned int r0;
};

struct r_cpu1
{
    alignas(128) unsigned int r0;
};

struct r_gpu1
{
    alignas(128) unsigned int r0;
};


pthread_barrier_t barrier;
 struct s_var* var;
volatile struct r_cpu0* cpu_res1;
volatile struct r_gpu0* gpu_res1;
volatile struct r_cpu1* cpu_res2;
volatile struct r_gpu1* gpu_res2;
volatile unsigned* cpu_rand1;
volatile unsigned* cpu_rand2;
volatile unsigned* gpu_rand1;
volatile unsigned* gpu_rand2;


// without volatile, simulator test works even if __thread support is broken
__thread volatile int local = 7;


void* runIRIW(void* arg)
{
    long long int tid = (long long int)arg;
    local += tid;
   pthread_barrier_wait(&barrier);
   #pragma GCC push_options
#pragma GCC optimize ("O0")

if(tid / DEF_THREADS == 0)
{
	for (int i = tid; i < DEF_RUNS; i+=DEF_THREADS)
	{
		for (int j = 0; j < (*(cpu_rand1+i)); j++);
		auto tmp_b = (std::atomic<int>*) &((var+i)->b);
		(gpu_res1+i)->r0 = tmp_b->load(std::memory_order_relaxed);
		// Load A and Load B 
		for (int j = 0; j < (*(cpu_rand1+i)); j++);
		auto tmp_a = (std::atomic<int>*) &((var+i)->a);
    		(cpu_res1+i)->r0 = tmp_a->load(std::memory_order_relaxed);
    		for (int j = 0; j < (*(cpu_rand1+i)); j++);
		tmp_b = (std::atomic<int>*) &((var+i)->b);
		(gpu_res1+i)->r0 = tmp_b->load(std::memory_order_relaxed);
	}
}
else 
{
	for (int i = (tid % DEF_THREADS); i < DEF_RUNS; i+=DEF_THREADS)
	{
		for (int j = 0; j < (*(cpu_rand2+i)); j++);
		auto tmp_a = (std::atomic<int>*) &((var+i)->a);
    		(cpu_res2+i)->r0 = tmp_a->load(std::memory_order_relaxed);
		// Load B and Load A
		for (int j = 0; j < (*(cpu_rand2+i)); j++);
		auto tmp_b = (std::atomic<int>*) &((var+i)->b);
    		(gpu_res2+i)->r0 = tmp_b->load(std::memory_order_relaxed);
    		for (int j = 0; j < (*(cpu_rand2+i)); j++);
		tmp_a = (std::atomic<int>*) &((var+i)->a);
    		(cpu_res2+i)->r0 = tmp_a->load(std::memory_order_relaxed);	
	}

}
#pragma GCC pop_options 
    return (void*)local;
}

template <typename T>
__device__ void 
seta(T* delay, struct s_var* var)
{
#pragma GCC push_options
#pragma GCC optimize ("O0")	
            
            // Set A	
  for (int j = 0; j < (*delay); j++);
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
	// Set B
	for (int j = 0; j < (*delay); j++);
		for (int j = 0; j < (*delay); j++);
	var->b = 1;
	#pragma GCC pop_options
}




template <typename T>
__global__ void
IRIW1(T* gpu_rand1, T* gpu_rand2, struct s_var* var, unsigned N)
{
     
    int i = (hipBlockIdx_x * hipBlockDim_x + hipThreadIdx_x);
#pragma GCC push_options
#pragma GCC optimize ("O0") 
    	if(i < 2* N) {
    	       if((hipBlockIdx_x) % 2 == 0)
    			seta(&gpu_rand2[i/2], (var + (i/2)));
    	       else
    	       	setb(&gpu_rand1[i/2], (var + (i/2)));	
    	}
#pragma GCC pop_options        	
}


void* LaunchGPU(void * arg)
{
	long long int tid = (long long int)arg;
        local += tid;
	unsigned blocks = 8192;
        unsigned threadsPerBlock = 1;
         hipLaunchKernelGGL(IRIW1, dim3(blocks), dim3(threadsPerBlock), 0, 0,
            gpu_rand1, gpu_rand2, 
            var, DEF_RUNS);
            #pragma GCC push_options 	

            return (void*)local;        
}





int check_output(unsigned t_range)
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
    
    unsigned pass = 0;
    unsigned failure = 0;
    
    for (auto i = 0; i < t_range; ++i) {
    
    		// Load A and Load B
        if ((cpu_res1[i].r0 == 0) && (gpu_res1[i].r0 == 0)) {
            res_cpu0_gpu0++;
        } else if ((cpu_res1[i].r0 == 1) && (gpu_res1[i].r0 == 0)) {
            res_cpu1_gpu0++;
        } else if ((cpu_res1[i].r0 == 0) && (gpu_res1[i].r0 == 1)) {
            res_cpu0_gpu1++;
        } else if ((cpu_res1[i].r0 == 1) && (gpu_res1[i].r0 == 1)) {
            res_cpu1_gpu1++;
        }
        else{
            res_cpu2_gpu2++;
        }
        // Load B and Load A
        if ((cpu_res2[i].r0 == 0) && (gpu_res2[i].r0 == 0)) {
            rst_cpu0_gpu0++;
        } else if ((cpu_res2[i].r0 == 1) && (gpu_res2[i].r0 == 0)) {
            rst_cpu1_gpu0++;
        } else if ((cpu_res2[i].r0 == 0) && (gpu_res2[i].r0 == 1)) {
            rst_cpu0_gpu1++;
        } else if ((cpu_res2[i].r0 == 1) && (gpu_res2[i].r0 == 1)) {
            rst_cpu1_gpu1++;
        }
        else{
            rst_cpu2_gpu2++;
        }
        
        if ((cpu_res1[i].r0 == 1) && (gpu_res1[i].r0 == 0) && (cpu_res2[i].r0 == 0) && (gpu_res2[i].r0 == 1)) {
        	failure++;
        }
        else
        {
        	pass++;
        }
    }
    
    std::cout << "Pass Test: " << pass << std::endl;
	std::cout << "Failure Test: " << failure << std::endl;
if (failure != 0)
    {
		std::cout << "=========================================================================" << std::endl;
        	std::cout << "	\t Compound IRIW1-sys with SWMR voilation Allowed  \t " << std::endl;						
        	std::cout << "=========================================================================" << std::endl;  
    }
    else
    {
		std::cout << "=========================================================================" << std::endl;
        	std::cout << "	\t Compound IRIW1-sys with SWMR voilation Disallowed  \t " << std::endl;						
        	std::cout << "=========================================================================" << std::endl;  
    }
    if (!res_cpu1_gpu0 && !rst_cpu0_gpu1) {
        std::cout << "Success!" << std::endl;
        
        std::cout << "Count (a:0 and b:0): " << res_cpu0_gpu0 << std::endl;
        std::cout << "Count (a:1 and b:0): " << res_cpu1_gpu0 << std::endl;
        std::cout << "Count (a:0 and b:1): " << res_cpu0_gpu1 << std::endl;
        std::cout << "Count (a:1 and b:1): " << res_cpu1_gpu1 << std::endl;
        std::cout << "Count (r0:2 and r1:2): " << res_cpu2_gpu2 << std::endl;
       
        std::cout << "Count (a:0 and b:0): " << rst_cpu0_gpu0 << std::endl;
        std::cout << "Count (a:1 and b:0): " << rst_cpu1_gpu0 << std::endl;
        std::cout << "Count (a:0 and b:1): " << rst_cpu0_gpu1 << std::endl;
        std::cout << "Count (a:1 and b:1): " << rst_cpu1_gpu1 << std::endl;
        std::cout << "Count (r0:2 and r1:2): " << rst_cpu2_gpu2 << std::endl; 
             
        //std::cout << "Num Valid test: " << (t_range - res_cpu1_gpu1) << std::endl;
        //std::cout << "Num Invalid test: " << res_cpu1_gpu1 << std::endl;
        return 0;
    } else {
        std::cout << "Fail!" << std::endl;
        
        std::cout << "Count (a:0 and b:0): " << res_cpu0_gpu0 << std::endl;
        std::cout << "Count (a:1 and b:0): " << res_cpu1_gpu0 << std::endl;
        std::cout << "Count (a:0 and b:1): " << res_cpu0_gpu1 << std::endl;
        std::cout << "Count (a:1 and b:1): " << res_cpu1_gpu1 << std::endl;
        std::cout << "Count (r0:2 and r1:2): " << res_cpu2_gpu2 << std::endl;
        
        std::cout << "Count (a:0 and b:0): " << rst_cpu0_gpu0 << std::endl;
        std::cout << "Count (a:1 and b:0): " << rst_cpu1_gpu0 << std::endl;
        std::cout << "Count (a:0 and b:1): " << rst_cpu0_gpu1 << std::endl;
        std::cout << "Count (a:1 and b:1): " << rst_cpu1_gpu1 << std::endl;
        std::cout << "Count (r0:2 and r1:2): " << rst_cpu2_gpu2 << std::endl;
        
        //std::cout << "Num Valid test: " << (t_range - res_cpu1_gpu1) << std::endl;
        //std::cout << "Num Invalid test: " << res_cpu1_gpu1 << std::endl;
        return 2;
    }

}



int main (int argc, char** argv)
{
    if (argc != 2) { 
        printf("usage: %s <thread_count>\n", argv[0]);
        exit(1);
    }
    int thread_count = atoi(argv[1]);

    printf("Starting %d threads...\n", thread_count);

auto num_iterations = DEF_RUNS; 

auto t_range = num_iterations;
    // Allocate shared variables
   var = (struct s_var*)malloc(sizeof(struct s_var) * t_range);
    // Initialize shared vars
    for (auto i = 0; i < t_range; ++i) {
        var[i].a = 0;
        var[i].b = 0;
    }

    // Allocate output variables for cpu
   cpu_res1 = (struct r_cpu0*)malloc(sizeof(struct r_cpu0) * t_range);
   cpu_res2 = (struct r_cpu1*)malloc(sizeof(struct r_cpu1) * t_range);
    // Initialize cpu result
    for (auto i = 0; i < t_range; ++i) {
        cpu_res1[i].r0 = 2;
        cpu_res2[i].r0 = 2;
    }

    // Allocate output variables for gpu
    gpu_res1 = (struct r_gpu0*)malloc(sizeof(struct r_gpu0) * t_range);
    gpu_res2 = (struct r_gpu1*)malloc(sizeof(struct r_gpu1) * t_range);
    // Initialize gpu result
    for (auto i = 0; i < t_range; ++i) {
        gpu_res1[i].r0 = 2;
        gpu_res2[i].r0 = 2;
    }

    // Number of CPU threads == GPU threads
    // Allocate vectors for random numbers
   cpu_rand1 = (unsigned*)malloc(sizeof(unsigned) * t_range);
   cpu_rand2 = (unsigned*)malloc(sizeof(unsigned) * t_range);
   gpu_rand1 = (unsigned*)malloc(sizeof(unsigned) * t_range);
   gpu_rand2 = (unsigned*)malloc(sizeof(unsigned) * t_range);


for (int iter = 0; iter < num_iterations; iter++) {
           // Generate random time offset value
            auto g_tid = iter;

            // CPU
            unsigned rand_val = std::rand() % RAND_RANGE;
            //std::cout << "CPU_T:" << i << " Rand: " << rand_val << std::endl;
            cpu_rand1[g_tid] = rand_val;
            
            rand_val = std::rand() % RAND_RANGE;
            cpu_rand2[g_tid] = rand_val;

            //GPU
            rand_val = std::rand() % RAND_RANGE;
            //std::cout << "GPU_T:" << i << " Rand: " << rand_val << std::endl;
            gpu_rand1[g_tid] = rand_val;
            
            rand_val = std::rand() % RAND_RANGE;
            gpu_rand2[g_tid] = rand_val;
    }

    std::cout << "Random thread launch delays generated" << std::endl;


    int i;
    pthread_t* threads = (pthread_t*)calloc(thread_count, sizeof(pthread_t));
    
    
    //=============================================================
    //		Intialize the barrier
    //============================================================
        pthread_barrier_init(&barrier, NULL, (thread_count)); 
        
    //=============================================================    
    assert(threads != NULL);




/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
///			GPU Section 						    ///
////////////////////////////////////////////////////////////////////////////////////
    long long int gpu_local = (long long int) LaunchGPU((void*)100);
    printf("local[0] = %lld\n", gpu_local);
    std::cout << "GPU Threads Launched" << std::endl;
 /////////////////////////////////////////////////////////////////////////////////////   


/////////////////////////////////////////////////////////////////////////////////////
///			CPU Section 						    ///
////////////////////////////////////////////////////////////////////////////////////
    for (i = 0 ; i < thread_count; i++) {
        int createResult = pthread_create(&threads[i], 
                                          NULL,
                                          runIRIW,
                                          (void*)i);
        assert(createResult == 0);
    }


   std::cout << "CPU Threads Launched" << std::endl;
                            

	std::cout << "All threads executing" << std::endl;
        #pragma GCC push_options
#pragma GCC optimize ("O0")	   
        for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                 for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                 for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
                for (long unsigned j = 0; j < 500000; j++);
		#pragma GCC pop_options
		

    

////////////////////////////////////////////////////////////////////////
///		Join GPU Threads				       ///
///////////////////////////////////////////////////////////////////////

    hipDeviceSynchronize();
///////////////////////////////////////////////////////////////////////    
////////////////////////////////////////////////////////////////////////
///		Join CPU Threads				       ///
///////////////////////////////////////////////////////////////////////	
	
    for (i = 0 ; i < thread_count; i++) {
        int joinResult = pthread_join(threads[i], 
                                      (void**)&local);
        assert(joinResult == 0);
        printf("local[%d] = %lld\n", i, local);
    }
////////////////////////////////////////////////////////////////////////        
    
    std::cout << "Validating..." << std::flush;
    int res = check_output(t_range);
   
        free(threads);
   
    return 0;
}
