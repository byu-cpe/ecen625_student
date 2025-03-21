
c_simulation: digitrec.cpp digitrec_test.cpp
	@echo "Compiling & executing the digit recogintion program ..."
	vitis-run --mode hls --csim --config hls_config.cfg --work_dir temp

c_synthesis: digitrec.cpp digitrec_test.cpp
	@echo "Synthesizing the digit recogintion program ..."
	v++ -c --mode hls --config hls_config.cfg --work_dir temp

package:
	@echo "Packaging the digit recogintion program ..."
	vitis-run --mode hls --package --config hls_config.cfg --work_dir temp

clean:
	@echo "Clean up output files"
# rm -rf digitrec vivado_hls.log *nn.prj result out.dat *~
