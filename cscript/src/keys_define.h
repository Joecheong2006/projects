#ifndef KEYS_DEFINE_H
#define KEYS_DEFINE_H

typedef enum {
    KeywordIf,
    KeywordWhile,
    KeywordReturn,
    KeywordConst,
    KeywordInt,
    KeywordFloat,
    KeywordBool,
    KeywordChar,
    KeywordString,
    KeywordTrue,
    KeywordFlase,
    KeywordNone,
    KeywordFunction,
    KeywordTokenCount
} KeywordType;

enum {
    SeparatorColon,
    SeparatorColonColon,
    SeparatorExtend,
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

extern const char* Keyword[KeywordTokenCount];
extern const char* Separator[SeparatorTokenCount];
extern const char* Operator[OperatorTokenCount];
extern const char* SingleLineComment[1];
extern const char* MultiLineComment[2];

#endif
