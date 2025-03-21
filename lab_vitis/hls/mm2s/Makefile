
c_synthesis: mm2s.cpp 
	@echo "Synthesizing the mm2s program ..."
	v++ -c --mode hls --config hls_config.cfg --work_dir temp

package:
	@echo "Packaging the mm2s program ..."
	vitis-run --mode hls --package --config hls_config.cfg --work_dir temp

clean:
	@echo "Clean up output files"
