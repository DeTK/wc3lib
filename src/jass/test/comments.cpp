#define BOOST_TEST_MODULE CommentsTest
#include <boost/test/unit_test.hpp>

#include <fstream>
#include <iostream>
#include <iomanip>

//#define BOOST_SPIRIT_DEBUG // enable debugging for Qi

#include <boost/spirit/include/support_multi_pass.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp> // for more detailed error information

#include <boost/foreach.hpp>

#include "../../platform.hpp"
#include "../grammar.cpp"
#include "../ast.hpp"

#ifndef BOOST_TEST_DYN_LINK
#error Define BOOST_TEST_DYN_LINK for proper definition of main function.
#endif

using namespace wc3lib;
using namespace wc3lib::jass;

/*
 * This test parses lines and skips all comments starting with //
 * End of line characters should still be at the end of the strings to detect the lines.
 * All lines are stored in a vector.
 * 
 * https://stackoverflow.com/questions/22591094/boost-spirit-new-line-and-end-of-input
 */
BOOST_AUTO_TEST_CASE(CommentsTest) {
	ifstream in("comments.j");
	
	BOOST_REQUIRE(in);
	
	Grammar::traceLog.open("commentstraces.xml");
	
	BOOST_REQUIRE(Grammar::traceLog);
	
	typedef std::istreambuf_iterator<byte> IteratorType;
	typedef boost::spirit::multi_pass<IteratorType> ForwardIteratorType;

	ForwardIteratorType first = boost::spirit::make_default_multi_pass(IteratorType(in));
	ForwardIteratorType last;
	
	// used for backtracking and more detailed error output
	namespace classic = boost::spirit::classic;
	typedef classic::position_iterator2<ForwardIteratorType> PositionIteratorType;
	PositionIteratorType position_begin(first, last);
	PositionIteratorType position_end;
	jass_ast ast;
	bool valid = false;
	std::vector<std::string> result;
	
	try
	{
		namespace phoenix = boost::phoenix;
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		
		client::comment_skipper<PositionIteratorType> skipper;
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			//  | &qi::eoi
			*qi::eol >> (*(qi::char_ - (qi::eol)) % (qi::eol)),
			skipper,
			result
			);

		if (position_begin != position_end) // fail if we did not get a full match
		{
			std::cerr << "Begin is not equal to end!" << std::endl;
			valid = false;
		}
	}
	catch(const boost::spirit::qi::expectation_failure<PositionIteratorType> e)
	{
		const classic::file_position_base<std::string>& pos = e.first.get_position();
		std::stringstream msg;
		msg <<
		"parse error at file " << pos.file <<
		" line " << pos.line << " column " << pos.column << std::endl <<
		"'" << e.first.get_currentline() << "'" << std::endl <<
		std::setw(pos.column) << " " << "^- here";
		
		std::cerr << msg.str() << std::endl;
	}
	
	/*
	 * for debugging output
	BOOST_FOREACH(std::string &value, result) {
		std::cerr << value << std::endl;
	}
	*/
	
	BOOST_REQUIRE(valid);
	BOOST_REQUIRE(result.size() == 5); // number of parsed lines, lines with comments only contain the line break and therefore result in empty strings (eol is not stored in the string)
	//BOOST_REQUIRE(result[0] == "\n"); // NOTE The first line is ignored due to the grammar which starts with a non EOL character!
	BOOST_REQUIRE(result[0] == "globals");
	BOOST_REQUIRE(result[1] == "integerbla=10"); // all blanks have to be skipped
	BOOST_REQUIRE(result[2] == "endglobals");
	BOOST_REQUIRE(result[3] == ""); // NOTE Empty line for comment/blanks only line!
	BOOST_REQUIRE(result[4] == "typeblaextendsinteger"); // all blanks have to be skipped
}
