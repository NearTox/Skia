/*
 * Copyright 2017 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */
/*****************************************************************************************
 ******************** This file was generated by sksllex. Do not edit. *******************
 *****************************************************************************************/
#ifndef SKSL_Lexer
#define SKSL_Lexer
#include <cstddef>
#include <cstdint>
namespace SkSL {

struct Token {
  enum class Kind {
    TK_END_OF_FILE,
    TK_FLOAT_LITERAL,
    TK_INT_LITERAL,
    TK_TRUE_LITERAL,
    TK_FALSE_LITERAL,
    TK_IF,
    TK_STATIC_IF,
    TK_ELSE,
    TK_FOR,
    TK_WHILE,
    TK_DO,
    TK_SWITCH,
    TK_STATIC_SWITCH,
    TK_CASE,
    TK_DEFAULT,
    TK_BREAK,
    TK_CONTINUE,
    TK_DISCARD,
    TK_RETURN,
    TK_NULL_LITERAL,
    TK_IN,
    TK_OUT,
    TK_INOUT,
    TK_UNIFORM,
    TK_CONST,
    TK_FLAT,
    TK_NOPERSPECTIVE,
    TK_READONLY,
    TK_WRITEONLY,
    TK_COHERENT,
    TK_VOLATILE,
    TK_RESTRICT,
    TK_BUFFER,
    TK_INLINE,
    TK_HASSIDEEFFECTS,
    TK_PLS,
    TK_PLSIN,
    TK_PLSOUT,
    TK_VARYING,
    TK_STRUCT,
    TK_LAYOUT,
    TK_PRECISION,
    TK_ENUM,
    TK_CLASS,
    TK_IDENTIFIER,
    TK_DIRECTIVE,
    TK_SECTION,
    TK_LPAREN,
    TK_RPAREN,
    TK_LBRACE,
    TK_RBRACE,
    TK_LBRACKET,
    TK_RBRACKET,
    TK_DOT,
    TK_COMMA,
    TK_PLUSPLUS,
    TK_MINUSMINUS,
    TK_PLUS,
    TK_MINUS,
    TK_STAR,
    TK_SLASH,
    TK_PERCENT,
    TK_SHL,
    TK_SHR,
    TK_BITWISEOR,
    TK_BITWISEXOR,
    TK_BITWISEAND,
    TK_BITWISENOT,
    TK_LOGICALOR,
    TK_LOGICALXOR,
    TK_LOGICALAND,
    TK_LOGICALNOT,
    TK_QUESTION,
    TK_COLONCOLON,
    TK_COLON,
    TK_EQ,
    TK_EQEQ,
    TK_NEQ,
    TK_GT,
    TK_LT,
    TK_GTEQ,
    TK_LTEQ,
    TK_PLUSEQ,
    TK_MINUSEQ,
    TK_STAREQ,
    TK_SLASHEQ,
    TK_PERCENTEQ,
    TK_SHLEQ,
    TK_SHREQ,
    TK_BITWISEOREQ,
    TK_BITWISEXOREQ,
    TK_BITWISEANDEQ,
    TK_LOGICALOREQ,
    TK_LOGICALXOREQ,
    TK_LOGICALANDEQ,
    TK_SEMICOLON,
    TK_ARROW,
    TK_WHITESPACE,
    TK_LINE_COMMENT,
    TK_BLOCK_COMMENT,
    TK_INVALID,
  };

  Token() : fKind(Kind::TK_INVALID), fOffset(-1), fLength(-1) {}

  Token(Kind kind, int32_t offset, int32_t length)
      : fKind(kind), fOffset(offset), fLength(length) {}

  Kind fKind;
  int fOffset;
  int fLength;
};

class Lexer {
 public:
  void start(const char* text, int32_t length) {
    fText = text;
    fLength = length;
    fOffset = 0;
  }

  Token next();

 private:
  const char* fText;
  int32_t fLength;
  int32_t fOffset;
};

}  // namespace SkSL
#endif
