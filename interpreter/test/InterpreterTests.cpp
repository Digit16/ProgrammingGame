#include <gtest/gtest.h>

#include "../src/Ast.h"
#include "../src/Interpreter.h"
#include "../src/Lexer.h"
#include "../src/Token.h"

using namespace testing;

class InterpreterTests : public Test {
public:
  void SetUp() override {}
};

TEST_F(InterpreterTests, SimpleAdditionTest) {
  std::string input = "5+7";

  Lexer lexer(input);
  Parser parser(lexer);
  Interpreter interpreter(parser);
  std::shared_ptr<AstNode> result = interpreter.interpret();

  int numberResult = -1;
  auto number = std::dynamic_pointer_cast<Number>(result);
  if (!number) {
    throw std::runtime_error("Casting failed");
  }

  numberResult = number->value();

  ASSERT_EQ(numberResult, 12);
}

TEST_F(InterpreterTests, SimpleNestedExpressionTest) {
  std::string input = "(5+7)*2";

  Lexer lexer(input);
  Parser parser(lexer);
  Interpreter interpreter(parser);
  std::shared_ptr<AstNode> result = interpreter.interpret();

  int numberResult = -1;
  auto number = std::dynamic_pointer_cast<Number>(result);
  if (!number) {
    throw std::runtime_error("Casting failed");
  }

  numberResult = number->value();

  ASSERT_EQ(numberResult, 24);
}

TEST_F(InterpreterTests, ProperExecutionOrder) {
  std::string input = "5+7*5";

  Lexer lexer(input);
  Parser parser(lexer);
  Interpreter interpreter(parser);
  std::shared_ptr<AstNode> result = interpreter.interpret();

  int numberResult = -1;
  auto number = std::dynamic_pointer_cast<Number>(result);
  if (!number) {
    throw std::runtime_error("Casting failed");
  }

  numberResult = number->value();

  ASSERT_EQ(numberResult, 40);
}
