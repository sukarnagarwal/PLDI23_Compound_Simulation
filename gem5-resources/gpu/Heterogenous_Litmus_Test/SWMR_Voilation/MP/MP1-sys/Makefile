## Make File Framed by: Sukarn Agarwal (Email: sagarwa2@ed.ac.uk)

HIP_PATH?= /opt/rocm-4.0.1/hip
HIPCC=$(HIP_PATH)/bin/hipcc

BIN_DIR?= ./bin

MP_Ver1_Relax_SWMR_voilation: $(BIN_DIR)/MP_Ver1_Relax_SWMR_voilation

$(BIN_DIR)/MP_Ver1_Relax_SWMR_voilation: MP_Ver1_Relax_SWMR_voilation.cpp $(BIN_DIR)
	$(HIPCC) --amdgpu-target=gfx801,gfx803 $(CXXFLAGS) -O1 MP_Ver1_Relax_SWMR_voilation.cpp -o $(BIN_DIR)/MP_Ver1_Relax_SWMR_voilation
	
	
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

.PHONY: MP_Ver1_Relax_SWMR_voilation clean
