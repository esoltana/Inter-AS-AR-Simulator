Date: Aug. 12 2014
Created by: Zhe Song

If you just run the executable, it will print what input parameters are needed.

how to run the executable:
./test_CallGenerator AS_num  call_arrival_rate  number_of_calls_generated  output_file_name  AR_whole_time  lead_time single_slot_time 



Example run:
./test_CallGenerator 1 8 1000 output.txt 1024 3 1

The file output.txt in this folder was generated using these parameters. AS_num: number of the AS. But the CallGenerator.cpp file is hardcoded with the directories and file names that have the AS
