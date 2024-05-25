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
    KeywordTokenCount
} KeywordType;

enum {
    SeparatorColon,
    SeparatorDDD,
    SeparatorTokenCount
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

    OperatorPower,
    OperatorTokenCount
} OperatorType;

enum {
    DoubleQuote,
    SingleQuote,
    CommentTokenCount,
};

extern const char* Keyword[KeywordTokenCount];
extern const char* Separator[SeparatorTokenCount];
extern const char* Operator[OperatorTokenCount];
extern const char* StringBegin[CommentTokenCount];
extern const char* SingleLineComment[1];
extern const char* MultiLineComment[2];

#endif
