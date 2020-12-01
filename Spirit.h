#include <vector>
#include <string>
#include <iostream>

#define BOOST_SPIRIT_UNICODE
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>

namespace parsing
{
    namespace qi = boost::spirit::qi;
    using namespace boost::spirit::unicode;

    struct variable
    {
        std::string name;
    };
    struct binaryOperator
    {
        char operatorChar;
    };
    struct expression
    {
        variable leftSide, rightSide;
        binaryOperator op;
    };

    using namespace boost::spirit::unicode;
    namespace qi = boost::spirit::qi;

    template<typename Iterator>
    struct expressionParser : qi::grammar<Iterator, expression(), space_type>
    {
        expressionParser() : expressionParser::base_type(start)
        {
            using qi::lit;
            var %=
                +alnum;
            op %=
                '+' | '-' | '*' | '/' | lit("&&") | lit("||") | lit("==");
            start %=
                var
                >> op
                >> var;
        }

        qi::rule<Iterator, variable(), space_type> var;
        qi::rule<Iterator, binaryOperator(), space_type> op;
        qi::rule<Iterator, expression(), space_type> start;
    };

    template <typename Iterator>
    std::string parse(Iterator begin, Iterator end)
    {
        using boost::spirit::qi::phrase_parse;
        expression exp;
        expressionParser<std::string::const_iterator> expParser;
        auto parse_result = phrase_parse(begin, end, expParser, space, exp);
        if(!parse_result)
        {
            return "PARSING ERROR";
        }
        else return "I GUESS IT WORKS";

        //return "TEST STRING FORM PARSE FUNCTION";
    }
}

BOOST_FUSION_ADAPT_STRUCT(
    parsing::expression,
    (parsing::variable, leftSide)
    (parsing::variable, rightSide)
    (parsing::binaryOperator, op)
)

BOOST_FUSION_ADAPT_STRUCT(
    parsing::variable,
    (std::string, name)
)

BOOST_FUSION_ADAPT_STRUCT(
    parsing::binaryOperator,
    (char, operatorChar)
)
