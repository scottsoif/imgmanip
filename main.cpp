#include <iostream>
#include <armadillo>

import imgio;

// using namespace std;
using namespace arma;

int main(int argc, char const *argv[])
{
  /* code */
  hello_world();

  // SEGFAULT
  // std::cout << "Armadillo version: " << arma_version::as_string() << std::endl;

  // // construct a matrix according to given size and form of element initialisation
  mat A(2,3,fill::zeros);

  // .n_rows and .n_cols are read only
  std::cout << "A.n_rows: " << A.n_rows << std::endl;
  std::cout << "A.n_cols: " << A.n_cols << std::endl;

  A(1,2) = 456.0;  // access an element (indexing starts at 0)

  // SEGFAULT
  A.print("A:");

  A = 5.0;         // scalars are treated as a 1x1 matrix
  A.print("A:");

  A.set_size(4,5); // change the size (data is not preserved)

  A.fill(5.0);     // set all elements to a specific value
  // A.print("A:");

  A = { { 0.165300, 0.454037, 0.995795, 0.124098, 0.047084 },
        { 0.688782, 0.036549, 0.552848, 0.937664, 0.866401 },
        { 0.348740, 0.479388, 0.506228, 0.145673, 0.491547 },
        { 0.148678, 0.682258, 0.571154, 0.874724, 0.444632 },
        { 0.245726, 0.595218, 0.409327, 0.367827, 0.385736 } };

  // A.print("A:");

    return 0;
}
