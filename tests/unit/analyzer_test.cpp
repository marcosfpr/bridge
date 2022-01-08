#include <bridge.hpp>

#include <string>
#include <vector>

#include <gtest/gtest.h>

TEST(test_analyzer, tokenize) {
  std::vector<std::string> tokens{"hello", "happy", "tax", "player"};

  auto token_iter = bridge::analyzer::tokenize("hello happy tax player");

  for (auto token : token_iter) {
    auto it = std::find(tokens.begin(), tokens.end(), token);
    EXPECT_NE(it, tokens.end());
    tokens.erase(it);  
    
  }

}