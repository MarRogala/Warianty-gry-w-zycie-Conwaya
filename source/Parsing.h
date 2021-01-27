#include <array>
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

#include <memory>
#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>

using namespace TAO_PEGTL_NAMESPACE;

void print_node(const parse_tree::node& n, std::string program);

namespace language
{
    std::string nodeContent(const parse_tree::node& n, std::string program);
    std::string readFile(std::string path);
    std::unique_ptr<parse_tree::node> parseProgram(std::string program, std::string name);
    void printError(parse_error e, std::string program, std::string name);

   // clang-format off

   struct positiveInteger : plus < digit > {};

   struct integer : seq <
        opt<one<'-'>>,
        positiveInteger
    > {};

    struct floatingPoint : seq <
        opt < one < '-' > >,
        plus < digit >,
        one < '.' >,
        plus < digit >
    > {};

   struct variable : identifier{};

   struct arrayVariable : seq <
        variable,
        one< '[' >,
        sor<integer, variable>,
        one< ']' >
   > {};

   struct function : seq <
        variable,
        one< '(' >,
        list_must<
            sor<floatingPoint, integer>,
            one< ',' >
        >,
        one< ')' >
   > {};

   struct functionCall : seq <
        function,
        one< ';' >
   > {};

   struct expressionValue : sor< function, arrayVariable, variable, floatingPoint, integer > {};

   struct plus : pad< one< '+' >, space > {};
   struct minus : pad< one< '-' >, space > {};
   struct multiply : pad< one< '*' >, space > {};
   struct divide : pad< one< '/' >, space > {};
   struct modulo : pad< one< '%' >, space > {};
   struct less : pad< one< '<' >, space > {};
   struct more : pad< one< '>' >, space > {};
   struct equal : pad< string< '=', '=' >, space > {};
   struct andOp : pad< string< '&', '&' >, space > {};
   struct orOp : pad< string< '|', '|' >, space > {};
   struct lessEqual : pad< string< '<', '=' >, space > {};
   struct moreEqual : pad< string< '>', '=' >, space > {};

   struct expressionOperator : sor < plus, minus, multiply, divide, modulo, moreEqual, lessEqual, less, more, equal, andOp, orOp > {};

   struct binaryExpressionInBracket;

   struct binaryExpression : seq <
        sor < expressionValue, binaryExpressionInBracket >,
        expressionOperator,
        sor < expressionValue, binaryExpressionInBracket >
   > {};

   struct binaryExpressionInBracket : seq <
        one < '(' >,
        binaryExpression,
        one < ')' >
   > {};

   struct simpleExpression : sor < binaryExpression, expressionValue, binaryExpressionInBracket> {};

   struct assignment : seq <
        sor < arrayVariable, variable >,
        one < '=' >,
        simpleExpression,
        one < ';' >
   > {};

   struct ifStatement : seq <
        string< 'i', 'f' >,
        one< '(' >,
        simpleExpression,
        one< ')' >,
        one< '{' >,
        star< assignment >,
        one< '}' >
   > {};

   struct instruction : sor<
        functionCall,
        ifStatement,
        assignment
   > {};

   struct program : star < instruction > {};

   struct grammar : must< program, eof > {};
   // clang-format on

   template< typename Rule >
   using selector = parse_tree::selector<
        Rule,
        parse_tree::store_content::on<
            integer,
            variable,
            arrayVariable,
            expressionOperator,
            floatingPoint
         >,
         parse_tree::fold_one::on <
            expressionValue,
            simpleExpression,
            functionCall,
            binaryExpressionInBracket,
            ifStatement
        >,
         parse_tree::remove_content::on<
            assignment,
            binaryExpression,
            function,
            plus,
            minus,
            multiply,
            divide,
            modulo,
            less,
            more,
            equal,
            andOp,
            orOp >
    >;
}
