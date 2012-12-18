# Succincter
## Computer Science 222 Fall 2012

### Project Members: ###
* Saagar Deshpande (sdeshpande@college, saagar@hcs.harvard.edu)
* Dan Bradley (dbradley@college)

### Introduction: ###
Using Mihai Patrascu's 2008 paper "Succincter", we implement a way to store trits (trinary values) within 1.01% of the ideal space of n*log_2(3) while having lookup in O(t) time, where t is the depth of our data structure. We use recursion to improve redundancy in the data structure, which allows us to accurately compute single decodes quickly without needing to decode the entire compressed block first. We find that this is both a fast and space efficient robust data structure for encoding and decoding operations with room for extension past simply storing trits.

### Source Code: ###

[Project on Github](https://github.com/raysaagar/succincterr)

### Usage: ###

* `encoder.c` and `decoder.c` are the Succincter implementations.
* `nencode.c` and `ndecode.c` are the naive implementation.
* `aencode.c` and `adecode.c` are simple arithmetic encoder/decoder implementations.

### Results: ###

[PDF](https://github.com/raysaagar/succincter/blob/master/tex/succinctur.pdf)

### Sources: ###
* Original Paper: Patrascu, Mihai. Succincter. 49th IEEE Symposium on Foundations of Computer Science. 2008.
* [Succincter](http://people.csail.mit.edu/mip/papers/succinct/succinct.pdf)

### Acknowledgements: ###
* [Michael Mitzenmacher](http://www.eecs.harvard.edu/~michaelm/) - for introducing us to Mihai Patrascu's work and inspiring this project 
* [Mihai Patrasc](http://people.csail.mit.edu/mip/) - for writing Succincter
