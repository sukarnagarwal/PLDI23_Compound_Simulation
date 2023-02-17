## Make File Framed by: Sukarn Agarwal (Email: sagarwa2@ed.ac.uk)

HIP_PATH?= /opt/rocm-4.0.1/hip
HIPCC=$(HIP_PATH)/bin/hipcc

BIN_DIR?= ./bin

MP_Ver1_Rel_Acq_GPU_WO_ATOMIC: $(BIN_DIR)/MP_Ver1_Rel_Acq_GPU_WO_ATOMIC

$(BIN_DIR)/MP_Ver1_Rel_Acq_GPU_WO_ATOMIC: MP_Ver1_Rel_Acq_GPU_WO_ATOMIC.cpp $(BIN_DIR)
	$(HIPCC) --amdgpu-target=gfx801,gfx803 $(CXXFLAGS) -O1 MP_Ver1_Rel_Acq_GPU_WO_ATOMIC.cpp -o $(BIN_DIR)/MP_Ver1_Rel_Acq_GPU_WO_ATOMIC
	
	
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

.PHONY: MP_Ver1_Rel_Acq_GPU_WO_ATOMIC clean
