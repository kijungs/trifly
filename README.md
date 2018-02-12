Tri-Fly: Distributed Estimation of Global and Local Triangle Counts in Graph Streams
========================
**Tri-Fly** is a distributed streaming algorithm for global and local triangle counting in graph streams. 
**Tri-Fly** has the following advantages compared to baselines: 
 * Accurate: Tri-Fly produces up to 4.5X smaller estimation error
 * Fast: Tri-Fly runs in linear time up to 8.8X faster
 * Theoretically Sound: Tri-Fly gives unbiased estimates with variances inversely proportional to the number of machines


Datasets
========================
The download links for the datasets used in the paper are [here](http://www.cs.cmu.edu/~kijungs/codes/trifly/)

Building and Running CoreScope
========================
Please see [User Guide](user_guide.pdf)

Running Demo
========================
For demo, please type 'make'

Reference
========================
If you use this code as part of any published research, please acknowledge the following paper.
```
@inproceedings{shin2018trifly,
  author    = {Kijung Shin and Mohammad Hammoud and Euiwoong Lee and Jinoh Oh and Christos Faloutsos},
  title     = {Tri-Fly: Distributed Estimation of Global and Local Triangle Counts in Graph Streams},
  booktitle = {PAKDD},
  year      = {2018}
}
```
