apps - full programs to see...
core - libraries to maltiply matrix
test - benchmarks and unit tests// not works now

extern folder - place extern libs:
    google benchmark - subfolder benchmark : need benchmark.lib,
    benchmark_main.lib and benchmark headers
    CM - subfolder CM 
    eigen - if you want to check with eigen it may be placed here, no currently 

# example of command for run benchmarks
Release\speed_test.exe --benchmark_repetitions=5 --benchmark_out=Release\result.csv --benchmark_out_format=csv --benchmark_counters_tabular=true --benchmark_report_aggregates_only=true
