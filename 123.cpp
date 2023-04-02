#include <iostream>

#define DEBUG_VALUE(X)                                      \
  std::cout << "At " << __FILE__ << " line " << __LINE__ << ": \'" << #X << "\' evaluates to " << X << '.' << std::endl;

// Notice the backslash ('\') at the end of each internal line of
// the macro to continue it to the next line.

// Look in the Tests/ directory to see how this macro will be used.