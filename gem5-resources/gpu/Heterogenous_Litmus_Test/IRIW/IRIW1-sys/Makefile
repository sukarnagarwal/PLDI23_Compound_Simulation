## Make File Framed by: Sukarn Agarwal (Email: sagarwa2@ed.ac.uk)


HIP_PATH?= /opt/rocm-4.0.1/hip
HIPCC=$(HIP_PATH)/bin/hipcc

BIN_DIR?= ./bin

IRIW_Ver1_Relax : $(BIN_DIR)/IRIW_Ver1_Relax 

$(BIN_DIR)/IRIW_Ver1_Relax : IRIW_Ver1_Relax.cpp $(BIN_DIR)
	$(HIPCC) --amdgpu-target=gfx801,gfx803,gfx900 $(CXXFLAGS) IRIW_Ver1_Relax.cpp -o $(BIN_DIR)/IRIW_Ver1_Relax 
	
	
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BIN_DIR)

.PHONY: IRIW_Ver1_Relax  clean
