
export CUDA_VERSION="11.1"; export CUDA_VISIBLE_DEVICES="0" ; export TRACES_FOLDER=/home/htran197/accel-sim-framework/hw_run/traces/device-0/11.1/heartwall-rodinia-2.0-ft/__data_test_avi_1___data_result_1_txt/traces; CUDA_INJECTION64_PATH=/home/htran197/accel-sim-framework/util/tracer_nvbit/tracer_tool/tracer_tool.so /home/htran197/gpu-app-collection/src/..//bin/11.1/release/heartwall-rodinia-2.0-ft ./data/test.avi 1 ./data/result-1.txt ; /home/htran197/accel-sim-framework/util/tracer_nvbit/tracer_tool/traces-processing/post-traces-processing /home/htran197/accel-sim-framework/hw_run/traces/device-0/11.1/heartwall-rodinia-2.0-ft/__data_test_avi_1___data_result_1_txt/traces/kernelslist ; rm -f /home/htran197/accel-sim-framework/hw_run/traces/device-0/11.1/heartwall-rodinia-2.0-ft/__data_test_avi_1___data_result_1_txt/traces/*.trace ; rm -f /home/htran197/accel-sim-framework/hw_run/traces/device-0/11.1/heartwall-rodinia-2.0-ft/__data_test_avi_1___data_result_1_txt/traces/kernelslist 