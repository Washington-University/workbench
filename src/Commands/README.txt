Do not write new commands in this directory, that method is obsolete,
new commands should be made as either Operations or Algorithms, as follows:

The defining difference between Operations and Algorithms is that Algorithms
provide a way for other workbench code to use their code easily.  Operations,
by contrast, often merely expose something already available internally to the
command line.  For instance, setting the name of a map in a file is a simple
function call, so to expose that to the command line, an Operation is used.
On the other hand, computing gradients is not built into any type of file object,
so it is implemented inside an Algorithm class (which provides a way for other
code to call it).

Operations are also generally the right solution when the output does not fit
into a common data file type (volume file, cifti file, etc) and the
implementation is simple - for instance, doing a spatial reduction operation on
a file results in one number per map, so the operations for this (-*-stats)
print the numbers to the terminal rather than making an output file.

Once you know whether the command should be an Algorithm or Operation, open
either Algorithms/AlgorithmTemplate.h.txt or Operations/OperationTemplate.h.txt,
and follow the instructions in the block comment just below the license notice.
