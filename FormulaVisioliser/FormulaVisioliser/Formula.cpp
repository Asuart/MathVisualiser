#include "Formula.h"


Token::Token(const std::string& str, u8 _priority) {
	asString = str;
	priority = _priority;
	Define();
}
bool Token::Define() {
	val = 0xc0ffee;
	if (asString == "") {
		type = Type::UNDEFINED;
	}
	else if (asString == "x") {
		type = Type::VAR;
		val = 0xc0ffee;
		var = asString.c_str()[0];
		priority = 0;
	}
	else if (asString == "+") {
		type = Type::OP;
		op = Operation::SUM;
		priority += 1;
	}
	else if (asString == "-") {
		type = Type::OP;
		op = Operation::SUB;
		priority += 1;
	}
	else if (asString == "*") {
		type = Type::OP;
		op = Operation::MUL;
		priority += 2;
	}
	else if (asString == "/") {
		type = Type::OP;
		op = Operation::DIV;
		priority += 2;
	}
	else if (asString == "|") {
		type = Type::OP;
		op = Operation::SIN;
		priority += 3;
	}
	else if (asString == "&") {
		type = Type::OP;
		op = Operation::COS;
		priority += 3;
	}
	else if (asString == "^") {
		type = Type::OP;
		op = Operation::TG;
		priority += 3;
	}
	else if (asString == "%") {
		type = Type::OP;
		op = Operation::CTG;
		priority += 3;
	}
	else if (std::isdigit(asString.c_str()[0])) {
		type = Type::VAL;
		val = std::stof(asString);
		priority = 0;
	}
	return true;
}
	


bool Formula::GenerateExecutionSequence() {
	std::cout << "Formula: " << formula << std::endl;
	
	s32 index = formula.find('=', 0);
	if (index == -1){
		std::cout << "Formula doesn contain \'=\' symbol.";
		exit(2);
	}
	std::string left = formula.substr(0, index);
	std::string right = formula.substr(index + 1, formula.length() - index - 1);

	// replace functions with 1-symbol encoding
	// replace sin with |
	while (true) {
		index = right.find("sin");
		if (index == -1) break;
		right.erase(index, 3);
		right.insert(index, 1, '|');
	}
	// replace cos with &
	while (true) {
		index = right.find("cos");
		if (index == -1) break;
		right.erase(index, 3);
		right.insert(index, 1, '&');
	}
	// replace tg with ^
	while (true) {
		index = right.find("tg");
		if (index == -1) break;
		right.erase(index, 3);
		right.insert(index, 1, '^');
	}
	// replace ctg with %
	while (true) {
		index = right.find("ctg");
		if (index == -1) break;
		right.erase(index, 3);
		right.insert(index, 1, '%');
	}

	std::cout << "Refactored: " << left << " = " << right << std::endl;

	// В книге про алгоритмы С++ я читал про рекурсионный подход при решении этой задачи, но лично я предпочитаю итерации.
	u8 basePriority = 0;
	for (s32 i = 0; i < right.length(); i++) {
		s8 symbol = right.c_str()[i];

		if (symbol == ' ') continue;
		else if (std::isdigit(symbol)) {
			std::string value = "";
			bool integer = true;
			u32 nom = 1;
			while (true) {
				i++;
				value += symbol;
				symbol = right.c_str()[i];
				if (std::isdigit(symbol) || symbol == '.') { continue; }
				else if (symbol == 'x') {
					tokens.push_back(Token(value, 0));
					tokens.push_back(Token("*", basePriority));
					i--;
					break;
				}
				else {
					tokens.push_back(Token(value, 0));
					i--;
					break;
				}
			}
		}
		else if(symbol == '('){
			basePriority += 4;
		}
		else if (symbol == ')') {
			basePriority -= 4;
		}
		else {
			tokens.push_back(Token(right.substr(i, 1),basePriority));
		}
	}
	u32 j = 0;
	for (auto i = tokens.begin(); i != tokens.end(); i++, j++) {
		std::cout << j << ": " << i._Ptr->_Myval.asString;
		if (i->type == Token::Type::OP) std::cout << " (" << (u32)i->priority << ")";
		std::cout << std::endl;
	}
	return true;
}



Formula::Formula(const std::string& _formula = "y = x") {
	formula = _formula;
	GenerateExecutionSequence();
}

Formula::~Formula() {

}



f32 Formula::DefineInX(f32 x) {
	auto tokens_copy = tokens;

	// replace variables with constant value
	for (auto i = tokens_copy.begin(); i != tokens_copy.end(); i++) {
		if (i._Ptr->_Myval.type == Token::Type::VAR) {
			i._Ptr->_Myval.val = x;
		}
	}

	while (true) {
		auto cur = tokens_copy.begin();
		for (auto i = tokens_copy.begin(); i != tokens_copy.end(); i++) { // search for operation with greates priority.
			if (cur->priority < i->priority) cur = i;
		}
		if (cur->type == Token::Type::VAL || cur->type == Token::Type::VAR) return cur->val;

		f32 val;
		auto it = cur;
		switch (cur->op) { // check, which operation is it, and execute it;
		case Token::Operation::SUM:
			cur._Ptr->_Prev->_Myval.val += cur._Ptr->_Next->_Myval.val;
			cur._Ptr->_Prev->_Myval.asString = std::to_string(cur._Ptr->_Prev->_Myval.val);
			it = cur;
			it++;
			tokens_copy.erase(cur);
			tokens_copy.erase(it);
			break;
		case Token::Operation::SUB:
			cur._Ptr->_Prev->_Myval.val -= cur._Ptr->_Next->_Myval.val;
			cur._Ptr->_Prev->_Myval.asString = std::to_string(cur._Ptr->_Prev->_Myval.val);
			it = cur;
			it++;
			tokens_copy.erase(cur);
			tokens_copy.erase(it);
			break;
		case Token::Operation::MUL:
			cur._Ptr->_Prev->_Myval.val *= cur._Ptr->_Next->_Myval.val;
			cur._Ptr->_Prev->_Myval.asString = std::to_string(cur._Ptr->_Prev->_Myval.val);
			it = cur;
			it++;
			tokens_copy.erase(cur);
			tokens_copy.erase(it);
			break;
		case Token::Operation::DIV:
			cur._Ptr->_Prev->_Myval.val /= cur._Ptr->_Next->_Myval.val;
			cur._Ptr->_Prev->_Myval.asString = std::to_string(cur._Ptr->_Prev->_Myval.val);
			it = cur;
			it++;
			tokens_copy.erase(cur);
			tokens_copy.erase(it);
			break;
		case Token::Operation::SIN:
			val = std::sin(cur._Ptr->_Next->_Myval.val);
			cur->type = Token::Type::VAL;
			cur->val = val;
			cur->priority = 0;
			cur->asString = std::to_string(val);
			tokens_copy.erase(++cur);
			break;
		case Token::Operation::COS:
			val = std::cos(cur._Ptr->_Next->_Myval.val);
			cur->type = Token::Type::VAL;
			cur->val = val;
			cur->priority = 0;
			cur->asString = std::to_string(val);
			tokens_copy.erase(++cur);
			break;
		case Token::Operation::TG:
			val = std::tan(cur._Ptr->_Next->_Myval.val);
			cur->type = Token::Type::VAL;
			cur->val = val;
			cur->priority = 0;
			cur->asString = std::to_string(val);
			tokens_copy.erase(++cur);
			break;
		case Token::Operation::CTG:
			val = 1 / std::tan(cur._Ptr->_Next->_Myval.val);
			cur->type = Token::Type::VAL;
			cur->val = val;
			cur->priority = 0;
			cur->asString = std::to_string(val);
			tokens_copy.erase(++cur);
			break;
		default:
			std::cout << "Wrong tokens order: ";
			PrintTokens();
			system("pause");
		}
	}
}

void Formula::PrintTokens() {
	for (auto i = tokens.begin(); i != tokens.end(); i++) {
		std::cout << i._Ptr->_Myval.asString << " ";
	}
	std::cout << std::endl;
}