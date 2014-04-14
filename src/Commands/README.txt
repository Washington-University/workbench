Do not write new commands in this directory, that method is obsolete,
new commands should be made as either Operations or Algorithms, as follows:

The defining difference between Operations and Algorithms is whether they
contain a way for other code to use them easily.  Algorithms usually provide
a "constructor" that takes the inputs and outputs, but really executes the
thing the algorithm does (the reasoning for this is that it is the shortest
method to access without an instance, so we are abusing the class concept
for the purpose of associating several static methods).  On the other hand,
Operations only operate from arguments given to a parser of some kind.

As such, Operations should be used only for things that would not be useful
to other code (printing information to stdout or a text file, providing
command line access to something that is already trivial to do in code).

Once you know whether the command should be an Algorithm or Operation, open
either Algorithms/AlgorithmTemplate.h.txt or Operations/OperationTemplate.h.txt,
and follow the instructions in the block comment just below the license notice.
