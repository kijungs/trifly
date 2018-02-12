=================================================================================

 Tri-Fly: Distributed Estimation of Global and Local Triangle Counts in Graph Streams
 Authors: Anonymized

 Version: 1.0
 Date: Nov 10, 2017
 Main Contact: Kijung Shin, Mohammad Hammoud, Euiwoong Lee, Jinoh Oh, and Christos Faloutsos

 This software is free of charge under research purposes.
 For commercial purposes, please contact the author.

 =================================================================================

Tri-Fly is a distributed streaming algorithm for global and local triangle counting in graph streams. 
Tri-Fly has the following advantages compared to baselines: 
 - Accurate: Tri-Fly produces up to 4.5X smaller estimation error
 - Fast: Tri-Fly runs in linear time up to 8.8X faster
 - Theoretically Sound: Tri-Fly gives unbiased estimates with variances inversely proportional to the number of machines

For detailed information, see 'user_guide.pdf'

For demo, type 'make demo'