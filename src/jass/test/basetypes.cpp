#define BOOST_TEST_MODULE BaseTypesTest
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
 * This test tries to find all six base types of JASS from the input file.
 * The symbol table should add them in the first place so that they are available for ALL JASS inputs.
 */
BOOST_AUTO_TEST_CASE(BaseTypesTest) {
	const char* jassFile = "basetypes.j";
	const char* traceFile = "basetypestrace.xml";
	
	ifstream in(jassFile);
	
	BOOST_REQUIRE(in);
	
	Grammar::traceLog.open(traceFile);
	
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
	
	jass_file current_file;
	current_file.path = jassFile;
	
	ast.files.push_back(current_file);
	
	BOOST_REQUIRE(ast.files.size() == 1);
	
	bool valid = false;
	std::vector<jass_type*> result;
	
	try
	{
		namespace phoenix = boost::phoenix;
		namespace qi = boost::spirit::qi;
		namespace ascii = boost::spirit::ascii;
		
		client::comment_skipper<PositionIteratorType> skipper;
		client::jass_grammar<PositionIteratorType> grammar(ast, current_file);
	
		valid = boost::spirit::qi::phrase_parse(
			position_begin,
			position_end,
			(grammar.type_identifier % qi::eol), // parse all base types separated by eol
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
	BOOST_REQUIRE(result.size() == 6); // there are 6 different base types which all should be found in the symbol table by default
	//BOOST_REQUIRE(result[4] == "typeblaextendsinteger"); // all blanks have to be skipped
}
