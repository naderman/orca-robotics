#ifndef ORCAMISC_TOKENISE_H
#define ORCAMISC_TOKENISE_H

#include <string>
#include <vector>

namespace orcamisc {

//! Takes a string containing tokens separated by a delimiter
//! Fills in the vector of tokens
void tokenise(const std::string        &str, 
              std::vector<std::string> &tokens, 
              const std::string        &delimiter);

}

#endif
