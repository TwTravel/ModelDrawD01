lcov --capture --directory ./  --output-file coverage.info
genhtml coverage.info --output-directory output
gprof Draw3D gmon.out|gprof2dot -n0 -e0|dot -Tpng -o output.png


