## Make File Framed by: Sukarn Agarwal (Email: sagarwa2@ed.ac.uk)

HIP_PATH?= /opt/rocm-4.0.1/hip
HIPCC=$(HIP_PATH)/bin/hipcc

BIN_DIR?= ./bin

SB_RelAcq_WO_Atomic_Fence: $(BIN_DIR)/SB_RelAcq_WO_Atomic_Fence

$(BIN_DIR)/SB_RelAcq_WO_Atomic_Fence: SB_RelAcq_WO_Atomic_Fence.cpp $(BIN_DIR)
	$(HIPCC) --amdgpu-target=gfx801,gfx803 $(CXXFLAGS) SB_RelAcq_WO_Atomic_Fence.cpp -o $(BIN_DIR)/SB_RelAcq_WO_Atomic_Fence
		
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

.PHONY: SB_RelAcq_WO_Atomic_Fence clean


