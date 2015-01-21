Jan 14, 2014
Created by Elahe

Important Points about the scenarios:

1) The header definitions and selected parameter values are explained at the begining of each file.
2) 6 conditions are tested for USST/EST, with single/multiple AR options, and for one/multiple links(topology)
3) The values are selected in a way to be able to check all these conditions:

  1. produced options are in AR window size range
  2. the lead-time value is considered correctly
  3. the paramaters of produced calls are correct based on the input file
  4. The link reservation for different TS is true
  5. The number of blocked calls is true
  6. The link is correctly reserved in both directions
  7. for multiple options, all options produced correctly
  8. the selected option is correct
  9. the selected path is correct, 
  10.the alternative paths are also considered.
  11. the earliest path algorithm works fine

4) all 6 scenarios were correct based one the above check list.
5) by default, for USST calls, the  reserved rate for both directions are equal.
6) AR option values in the tables are absolute values.
7) The selected AR option for the accepted calls is the index value (0 means the first AR option, 1 means the second AR option is selected)
8) for blocked calls the "selectedoption", "path Length", and "path" values are -1 or empty.
9) for EST calls, the input parameters are changed to minute (they are included in the corresponding files). The reason was that the input files were based on minutes. But, the problem condition is the same as USST calls when values are converted to Time Slot.
10) in EST files, two output headers are changed: "sch-win" which shows the scheduling window. (1,3) means that the start times can be (1,2, or 3). The other header is "selectedop" which shows the selected start time. It has an absolute value. For example is selectedop value is 3 with duration 2, it means that the third time slot is selected and the 3th and 4th time slots will be reserved(as the duration is 2TS).
11) the 7th scenarios has the same parameters as 6th file. the only difference is that the arrival rate is larger in order to produce more calls and see if the shortest path is full, the alternative paths will be considered or not. The result shows that the code works well.




