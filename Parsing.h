#include <array>
#include <iomanip>
#include <iostream>
#include <string>
#include <type_traits>

#include <tao/pegtl.hpp>
#include <tao/pegtl/contrib/parse_tree.hpp>
#include <tao/pegtl/contrib/parse_tree_to_dot.hpp>


using namespace TAO_PEGTL_NAMESPACE;

namespace language
{
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
   //struct lessEqual : pad< string< '<', '=' >, space > {};
   //struct moreEqual : pad< string< '>', '=' >, space > {};

   struct expressionOperator : sor < plus, minus, multiply, divide, modulo, less, more, equal, andOp, orOp > {};

   struct binaryExpression : seq <
        expressionValue,
        expressionOperator,
        expressionValue
   > {};

   struct simpleExpression : sor < binaryExpression, expressionValue > {};

   struct assignment : seq <
        sor < arrayVariable, variable >,
        one < '=' >,
        simpleExpression,
        one < ';' >
   > {};

   struct grammar : must< functionCall, eof > {};
   // clang-format on

   template< typename Rule >
   using selector = parse_tree::selector<
        Rule,
        parse_tree::store_content::on<
            integer,
            variable,
            arrayVariable,
            floatingPoint
         >,
         parse_tree::fold_one::on <
            expressionValue,
            simpleExpression,
            functionCall
        >,
         parse_tree::remove_content::on<
            assignment,
            binaryExpression,
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
