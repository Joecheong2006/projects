#include "keys_define.h"

const char* Keyword[] = {
    [KeywordIf] = "if",
    [KeywordWhile] = "while",
    [KeywordReturn] = "return",
    [KeywordConst] = "const",
    [KeywordInt] = "int", 
    [KeywordFloat] = "float", 
    [KeywordChar] = "char",
    [KeywordString] = "string",
    [KeywordBool] = "bool",
    [KeywordTrue] = "true", 
    [KeywordFlase] = "false", 
    [KeywordNull] = "null",
    [KeywordFunction] = "fn", 
    [KeywordEnd] = "end", 
};

const char* Separator[] = {
    [SeparatorColonColon] = "::",
    [SeparatorExtend] = "->",
    [SeparatorDDD] = "...",
};

const char* Operator[] = {
    [OperatorAssign] = "=",
    [OperatorPlus] = "+",
    [OperatorMinus] = "-",
    [OperatorMultiply] = "*",
    [OperatorDivision] = "/",
    [OperatorPlusEqual] = "+=",
    [OperatorMinusEqual] = "-=",
    [OperatorMultiplyEqual] = "*=",
    [OperatorDivisionEqual] = "/=",
    [OperatorBitwiseOr] = "|",
    [OperatorBitwiseAnd] = "&",
    [OperatorBitwiseXor] = "^",
    [OperatorBitwiseNot] = "~",
    [OperatorBitwiseOrEqual] = "|=",
    [OperatorBitwiseAndEqual] = "&=",
    [OperatorBitwiseXorEqual] = "^=",
    [OperatorLeftShift] = "<<",
    [OperatorRightShift] = ">>",
    [OperatorLeftShiftEqual] = "<<=",
    [OperatorRightShiftEqual] = ">>=",
    [OperatorRound] = "%",
    [OperatorRoundEqual] = "%=",
    [OperatorIncrement] = "++",
    [OperatorDecrement] = "--",
    [OperatorEqual] = "==",
    [OperatorNotEqual] = "!=",
    [OperatorGreaterThan] = ">",
    [OperatorLessThan] = "<",
    [OperatorInclusiveGreaterThan] = ">=",
    [OperatorInclusiveLessThan] = "<=",
    [OperatorOr] = "and",
    [OperatorAnd] = "or",
    [OperatorNot] = "!",
    [OperatorPower] = "**",
};

const char* SingleLineComment[] = {
    "#"
};

// must be a string pair
const char* MultiLineComment[] = {
    "/*", "*/"
};
