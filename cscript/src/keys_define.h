#ifndef KEYS_DEFINE_H
#define KEYS_DEFINE_H

typedef enum {
    KeywordIf,
    KeywordWhile,
    KeywordReturn,
    KeywordConst,
    KeywordInt,
    KeywordFloat,
    KeywordString,
    KeywordChar,
    KeywordTrue,
    KeywordFlase,
    KeywordNone,
    KeywordFunction,
} KeywordType;

const char* Keyword[] = {
    [KeywordIf] = "if",
    [KeywordWhile] = "while",
    [KeywordReturn] = "return",
    [KeywordConst] = "const",
    [KeywordInt] = "int", 
    [KeywordFloat] = "float", 
    [KeywordChar] = "char",
    [KeywordString] = "string",
    [KeywordTrue] = "null", 
    [KeywordFlase] = "true", 
    [KeywordNone] = "false",
    [KeywordFunction] = "fn", 
};

enum {
    SeparatorColon,
    SeparatorDDD,
};

// must be in one character
const char* Separator[] = {
    [SeparatorColon] = ":",
    [SeparatorDDD] = "..."
};

typedef enum {
    OperatorMinus,
    OperatorPlus,
    OperatorMultiply,
    OperatorDivision,
    OperatorBitwiseOr,
    OperatorBitwiseAnd,
    OperatorBitwiseXor,
    OperatorBitwiseNot,
    OperatorLeftShift,
    OperatorRightShift,

    OperatorAssign,
    OperatorAssignementBegin = OperatorAssign,
    OperatorPlusEqual,
    OperatorMinusEqual,
    OperatorMultiplyEqual,
    OperatorDivisionEqual,
    OperatorBitwiseOrEqual,
    OperatorBitwiseAndEqual,
    OperatorBitwiseXorEqual,
    OperatorBitwiseNotEqual,
    OperatorLeftShiftEqual,
    OperatorRightShiftEqual,
    OperatorRoundEqual,
    OperatorAssignmentEnd = OperatorRoundEqual,

    OperatorRound,
    OperatorIncrement,
    OperatorDecrement,
    OperatorEqual,
    OperatorNotEqual,
    OperatorGreaterThan,
    OperatorLessThan,
    OperatorInclusiveGreaterThan,
    OperatorInclusiveLessThan,
    OperatorOr,
    OperatorAnd,
    OperatorNot,

    OperatorPower
} OperatorType;

// a = 1;
// a > 1;

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
    [OperatorBitwiseNotEqual] = "~=",
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

enum {
    DoubleQuote,
    SingleQuote,
};

const char* StringBegin[] = {
    [DoubleQuote] = "\"",
};

const char* SingleLineComment[] = {
    "#"
};

// must be a string pair
const char* MultiLineComment[] = {
    "/*", "*/"
};

#endif
