#pragma once
#include "string"
#include "fstream"
#include <list>
#include "iostream"

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef char s8;
typedef short s16;
typedef int s32;
typedef float f32;
typedef double f64;


struct Token {
	enum class Type { UNDEFINED = 0, VAL, VAR, OP };// float value, int value, variable, operation, function
	enum class Operation { none = 0, SUM, SUB, DIV, MUL, SIN, COS, TG, CTG };
	union {
		f32 val;
		s8 var;
		Operation op;
	};
	std::string asString;
	Type type;
	u8 priority;

	Token(const std::string& str = "", u8 _priority = 0);
private:
	bool Define();
};


class Formula
{
	std::string formula;
	std::list<Token> tokens;

	bool GenerateExecutionSequence();
public:
	Formula(const std::string& formula);
	~Formula();

	f32 DefineInX(f32 x);
	void PrintTokens();
};

