#include <bridge.hpp>

#include <string>
#include <vector>

#include <gtest/gtest.h>

TEST(test_analyzer, tokenize) {
  std::vector<std::string> tokens{"hello", "happy", "tax", "player"};

  auto tokenizer = bridge::analyzer::alphanumeric_tokenizer("hello happy tax player");

  
  for (auto token : tokenizer) {
    auto it = std::find(tokens.begin(), tokens.end(), token);
    EXPECT_NE(it, tokens.end());
    tokens.erase(it);  
    
  }

}