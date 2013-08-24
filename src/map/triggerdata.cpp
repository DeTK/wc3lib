/***************************************************************************
 *   Copyright (C) 2011 by Tamino Dauth                                    *
 *   tamino@cdauth.eu                                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "triggerdata.hpp"
#include "txt.hpp"

namespace wc3lib
{

namespace map
{

TriggerData::TriggerData()
{
	m_specialTypes.insert("nothing");
	m_specialTypes.insert("Null");
}

// Value 2: Optional flag (defaults to 0) indicating to disable display of category name
TriggerData::Category::Category() : m_displayName(true)
{
}

std::streamsize TriggerData::Category::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Category::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

TriggerData::Type::Type() : m_baseType(0)
{
}

std::streamsize TriggerData::Type::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Type::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Parameter::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Parameter::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Function::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Function::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Call::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::Call::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::DefaultTrigger::read(InputStream &istream) throw (Exception)
{
	return 0;
}

std::streamsize TriggerData::DefaultTrigger::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

void TriggerData::Function::fillTypes(TriggerData *triggerData, const SplitVector &values)
{
	for (std::size_t i = 0; i < values.size(); ++i) {
		if (triggerData->specialTypes().find(values[i]) != triggerData->specialTypes().end()) {
			this->types().push_back(values[i]);
		} else {
			TriggerData::Types::iterator iterator = triggerData->types().find(values[i]);
			
			if (iterator != triggerData->types().end()) {
				this->types().push_back(iterator->second);
			} else {
				this->types().push_back(values[i]);
				std::cerr << boost::format(_("Warning: Unknown type \"%1%\" at index %2% for trigger function \"%3%\".")) % values[i] % i % this->code() << std::endl;
			}
		}
	}
}

void TriggerData::Call::fillTypes(TriggerData *triggerData, const SplitVector &values)
{
	if (values.size() >= 1) {
		this->m_canBeUsedInEvents = boost::lexical_cast<bool>(values[0]);
		
		for (std::size_t i = 1; i < values.size(); ++i) {
			if (triggerData->specialTypes().find(values[i]) != triggerData->specialTypes().end()) {
				this->types().push_back(values[i]);
			} else {
				TriggerData::Types::iterator iterator = triggerData->types().find(values[i]);
				
				if (iterator != triggerData->types().end()) {
					this->types().push_back(iterator->second);
				} else {
					this->types().push_back(values[i]);
					std::cerr << boost::format(_("Warning: Unknown type \"%1%\" at index %2% for trigger function \"%3%\".")) % values[i] % i % this->code() << std::endl;
				}
			}
		}
	} else {
		std::cerr << boost::format(_("Missing \"canBeUsedInEvents\" value for trigger call \"%1%\".")) % this->code() << std::endl;
	}
}

template<class FunctionType>
void TriggerData::readFunction(const Txt::Pair& ref, boost::ptr_map<string, FunctionType> &functions)
{
	FunctionType *function = 0;
	string code = ref.first;
	
	if (!boost::starts_with(code, "_")) {
		std::auto_ptr<FunctionType> functionPtr(new FunctionType());
		function = functionPtr.get();
		function->setCode(code);
		std::cerr << "New function: \"" << code << "\"" << std::endl; // TEST
		functions.insert(code, functionPtr);
	} else {
		code = code.substr(1);
		
		string::size_type index = code.find("_");
		
		if (index != string::npos) {
			code = code.substr(0, index);
		} else {
			std::cerr << boost::format(_("Missing suffix for trigger function entry \"%1%\".")) % ref.first << std::endl;
		}
		
		typename boost::ptr_map<string, FunctionType>::iterator iterator = functions.find(code);
		
		if (iterator == functions.end()) {
			std::cerr << boost::format(_("Missing base trigger function \"%1%\" for trigger function entry \"%2\".")) % code % ref.first << std::endl;
			
			return;
		}
		
		function = iterator->second;
	}
	
	SplitVector values;
	boost::algorithm::split(values, ref.second, boost::is_any_of(","), boost::algorithm::token_compress_on);
	
	string::size_type index = ref.first.find_last_of("_");
	
	if (index == string::npos) {
		function->fillTypes(this, values); // since trigger calls have other values than events, conditions and actions, we call the virtual implementation!
	}
	else if (ref.first.substr(index + 1) == "Defaults") {
		if (!function->defaults().empty()) {
			function->defaults().clear();
			std::cerr << boost::format(_("Redefinition of defaults for trigger function \"%1%\".")) % code << std::endl;
		}
		
		for (std::size_t i = 0; i < values.size(); ++i) {
			Parameters::iterator iterator = this->parameters().find(values[i]);
			
			if (iterator != this->parameters().end()) {
				function->defaults().push_back(iterator->second);
			} else {
				try {
					function->defaults().push_back(boost::lexical_cast<int32>(values[i]));
				}
				catch (boost::bad_lexical_cast &e) {
					function->defaults().push_back(values[i]);
				}
			}
		}
	}
	else if (ref.first.substr(index + 1) == "Category") {
		if (values.size() >= 1) {
			Categories::iterator iterator = this->categories().find(values[0]);
			
			if (iterator != this->categories().end()) {
				function->setCategory(iterator->second);
			} else {
				std::cerr << boost::format(_("Invalid category \"%1%\" for trigger function \"%2\".")) % values[0] % code << std::endl;
			}
		} else {
			std::cerr << boost::format(_("Empty category for trigger function \"%1%\".")) % code << std::endl;
		}
	}
	else if (ref.first.substr(index + 1) == "Limits") {
		if (!function->limits().empty()) {
			function->limits().clear();
			std::cerr << boost::format(_("Redefinition of limits for trigger function \"%1%\".")) % code << std::endl;
		}
		
		for (std::size_t i = 0; i < values.size(); ++i) {
			Parameters::iterator iterator = this->parameters().find(values[i]);
			
			if (iterator != this->parameters().end()) {
				function->limits().push_back(iterator->second);
			} else {
				try {
					function->limits().push_back(boost::lexical_cast<int32>(values[i]));
				}
				catch (boost::bad_lexical_cast &e) {
					function->limits().push_back(values[i]);
				}
			}
		}
	}
}

std::size_t TriggerData::firstNonNumericChar(const string& value) const
{
	string::size_type index = string::npos;
	
	for (string::size_type i = 0; i < value.size(); ++i) {
		if (value[i] > '9' || value[i] < '0') { // not numeric
			index = i;
			
			break;
		}
	}
	
	return index;
}

std::streamsize TriggerData::read(InputStream &istream) throw (Exception)
{
	boost::scoped_ptr<Txt> txt(new Txt());
	std::streamsize size = txt->read(istream);
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("TriggerCategories"))
	{
		std::auto_ptr<Category> category(new Category());
		
		string name = ref.first;
		category->setName(name);
		
		SplitVector values;
		boost::algorithm::split(values, ref.second, boost::is_any_of(","), boost::algorithm::token_compress_on);
		
		if (values.size() >= 1) {
			category->setDisplayText(values[0]);
		}
		
		if (values.size() >= 2) {
			category->setIconImageFile(values[1]);
		}
		
		if (values.size() >= 3) {
			category->setDisplayName(!boost::lexical_cast<bool>(values[2]));
		}
		
		this->categories().insert(name, category);
	}
	
	std::map<string, string> baseTypes;
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("TriggerTypes"))
	{
		std::auto_ptr<Type> type(new Type());
		
		// Key: type name
		string name = ref.first;
		type->setName(name);
		
		SplitVector values;
		boost::algorithm::split(values, ref.second, boost::is_any_of(","), boost::algorithm::token_compress_on);
		
		// Value 0: flag (0 or 1) indicating if this type can be a global variable
		if (values.size() >= 1) {
			type->setCanBeGlobal(boost::lexical_cast<bool>(values[0]));
		}
		
		// Value 1: flag (0 or 1) indicating if this type can be used with comparison operators
		if (values.size() >= 2) {
			type->setCanBeCompared(boost::lexical_cast<bool>(values[1]));
		}
		
		// Value 2: string to display in the editor
		if (values.size() >= 3) {
			type->setDisplayText(values[2]);
		}
		
		// Value 3: base type, used only for custom types
		if (values.size() >= 4) {
			baseTypes[name] = values[3];
		} /*else {
			std::cerr << boost::format(_("Missing base type for type \"%1%\".")) % name << std::endl;
		}*/
		
		this->types().insert(name, type);
	}
	
	// set trigger types bases
	BOOST_FOREACH(Types::reference ref, this->types())
	{
		ref.second->setBaseType(this->types().find(baseTypes[ref.first])->second);
	}
	
	// set trigger type defaults which are stored in a separated category
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("TriggerTypeDefaults"))
	{
		this->types().find(ref.first)->second->setDefaultValue(ref.second);
	}
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("TriggerParams"))
	{
		std::auto_ptr<Parameter> parameter(new Parameter());
		string name = ref.first;
		
		parameter->setName(name);
		
		
		SplitVector values;
		boost::algorithm::split(values, ref.second, boost::is_any_of(","), boost::algorithm::token_compress_on);
		
		if (values.size() >= 1) {
			Types::iterator iterator = this->types().find(values[0]);
			
			if (iterator != this->types().end()) {
				parameter->setType(iterator->second);
			}
		}
		
		if (values.size() >= 2) {
			parameter->setCode(values[1]);
		}
		
		if (values.size() >= 3) {
			parameter->setDisplayText(values[2]);
		}
		
		this->parameters().insert(name, parameter);
	}
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("TriggerEvents")) {
		readFunction<Function>(ref, this->events());
	}
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("TriggerConditions")) {
		readFunction<Function>(ref, this->conditions());
	}
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("TriggerActions")) {
		readFunction<Function>(ref, this->actions());
	}
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("TriggerCalls")) {
		readFunction<Call>(ref, this->calls());
	}
	
	std::size_t numCategories = 0;
	std::size_t actualCategories = 0;
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("DefaultTriggerCategories")) {
		if (ref.first == "NumCategories") {
			numCategories = boost::lexical_cast<std::size_t>(ref.second);
			this->defaultTriggerCategories().resize(numCategories);
		} else if (boost::starts_with(ref.first, "Category")) {
			std::size_t index = boost::lexical_cast<std::size_t>(ref.first.substr(strlen("Category"))) - 1;
			
			if (index < this->defaultTriggerCategories().size()) {
				this->defaultTriggerCategories()[index] = ref.second;
			} else {
				std::cerr << boost::format(_("Invalid index %1% for default trigger category \"%2%\".")) % index % ref.second << std::endl;
			}
			
			++actualCategories;
		}
	}
	
	if (numCategories != actualCategories) {
		std::cerr << boost::format(_("Expected default trigger categories %1% are not equal to actual ones %2%.")) % numCategories % actualCategories << std::endl;
	}
	
	std::size_t numTriggers = 0;
	std::size_t actualTriggers = 0;
	
	BOOST_FOREACH(Txt::Pairs::const_reference ref, txt->entries("DefaultTriggers")) {
		if (ref.first == "NumTriggers") {
			this->defaultTriggers().resize(boost::lexical_cast<std::size_t>(ref.second));
		} else if (boost::starts_with(ref.first, "Trigger")) {
			string substr = ref.first.substr(strlen("Trigger"));
			string::size_type substrIndex = this->firstNonNumericChar(substr);
			
			if (substrIndex == string::npos) {
				std::cerr << boost::format(_("Default trigger entry \"%1%\" is missing type.")) % ref.first << std::endl;
				
				continue;
			}
			
			const std::size_t index = boost::lexical_cast<std::size_t>(substr.substr(0, substrIndex)) - 1;
			
			if (index < this->defaultTriggers().size()) {
				DefaultTrigger *trigger = 0;
				
				if (this->defaultTriggers().is_null(index)) { // first creation
					std::auto_ptr<DefaultTrigger> triggerPtr(new DefaultTrigger());
					trigger = triggerPtr.get();
					this->defaultTriggers().replace(index, triggerPtr);
					++actualTriggers;
				} else {
					trigger = &this->defaultTriggers()[index];
				}
				
				const string type = substr.substr(substrIndex);
				
				if (type == "Name") {
					trigger->setName(ref.second);
				} else if (type == "Comment") {
					trigger->setComment(ref.second);
				} else if (type == "Category") {
					const std::size_t category = boost::lexical_cast<std::size_t>(ref.second);
					trigger->setTriggerCategory(this->defaultTriggerCategories()[category]);
				} else if (type == "Events") {
					const std::size_t eventsCount = boost::lexical_cast<std::size_t>(ref.second) + 1;
					trigger->events().resize(eventsCount);
				} else if (type == "Conditions") {
					const std::size_t conditionsCount = boost::lexical_cast<std::size_t>(ref.second) + 1;
					trigger->conditions().resize(conditionsCount);
				} else if (type == "Actions") {
					const std::size_t actionsCount = boost::lexical_cast<std::size_t>(ref.second) + 1;
					trigger->actions().resize(actionsCount);
				} else if (boost::starts_with(type, "Event")) {
					const std::size_t eventIndex = boost::lexical_cast<std::size_t>(type.substr(strlen("Event"))) - 1;
					
					Functions::iterator iterator = this->events().find(ref.second);
					
					if (iterator != this->events().end()) {
						if (eventIndex < trigger->events().size()) {
							trigger->events()[eventIndex] = iterator->second;
						} else {
							std::cerr << boost::format(_("Invalid event index %1% for default trigger entry %2%.")) % eventIndex % index << std::endl;
						}
					} else {
						std::cerr << boost::format(_("Default trigger entry %1% has invalid event value \"%2%\".")) % index % ref.second << std::endl;
					}
				} else if (boost::starts_with(type, "Condition")) {
					const std::size_t conditionIndex = boost::lexical_cast<std::size_t>(type.substr(strlen("Condition"))) - 1;
					
					Functions::iterator iterator = this->conditions().find(ref.second);
					
					if (iterator != this->conditions().end()) {
						if (conditionIndex < trigger->conditions().size()) {
							trigger->conditions()[conditionIndex] = iterator->second;
						} else {
							std::cerr << boost::format(_("Invalid condition index %1% for default trigger entry %2%.")) % conditionIndex % index << std::endl;
						}
					} else {
						std::cerr << boost::format(_("Default trigger entry %1% has invalid condition value \"%2%\".")) % index % ref.second << std::endl;
					}
				} else if (boost::starts_with(type, "Action")) {
					const std::size_t actionIndex = boost::lexical_cast<std::size_t>(type.substr(strlen("Action"))) - 1;
					
					Functions::iterator iterator = this->actions().find(ref.second);
					
					if (iterator != this->actions().end()) {
						if (actionIndex < trigger->actions().size()) {
							trigger->actions()[actionIndex] = iterator->second;
						} else {
							std::cerr << boost::format(_("Invalid action index %1% for default trigger entry %2%.")) % actionIndex % index << std::endl;
						}
					} else {
						std::cerr << boost::format(_("Default trigger entry %1% has invalid action value \"%2%\".")) % index % ref.second << std::endl;
					}
				} else {
					std::cerr << boost::format(_("Default trigger entry \"%1%\" has unknown suffix \"%2%\".")) % type << std::endl;
				}
			} else {
				std::cerr << boost::format(_("Invalid index %1% for default trigger \"%2%\".")) % index % ref.first << std::endl;
			}
		}
	}
	
	if (numTriggers != actualTriggers) {
		std::cerr << boost::format(_("Expected default triggers %1% are not equal to actual ones %2%.")) % numTriggers % actualTriggers << std::endl;
	}

	return size;
}

std::streamsize TriggerData::write(OutputStream &ostream) const throw (Exception)
{
	return 0;
}

}

}