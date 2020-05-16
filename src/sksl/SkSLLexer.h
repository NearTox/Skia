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
  enum Kind {
#undef END_OF_FILE
    END_OF_FILE,
#undef FLOAT_LITERAL
    FLOAT_LITERAL,
#undef INT_LITERAL
    INT_LITERAL,
#undef TRUE_LITERAL
    TRUE_LITERAL,
#undef FALSE_LITERAL
    FALSE_LITERAL,
#undef IF
    IF,
#undef STATIC_IF
    STATIC_IF,
#undef ELSE
    ELSE,
#undef FOR
    FOR,
#undef WHILE
    WHILE,
#undef DO
    DO,
#undef SWITCH
    SWITCH,
#undef STATIC_SWITCH
    STATIC_SWITCH,
#undef CASE
    CASE,
#undef DEFAULT
    DEFAULT,
#undef BREAK
    BREAK,
#undef CONTINUE
    CONTINUE,
#undef DISCARD
    DISCARD,
#undef RETURN
    RETURN,
#undef NULL_LITERAL
    NULL_LITERAL,
#undef IN
    IN,
#undef OUT
    OUT,
#undef INOUT
    INOUT,
#undef UNIFORM
    UNIFORM,
#undef CONST
    CONST,
#undef FLAT
    FLAT,
#undef NOPERSPECTIVE
    NOPERSPECTIVE,
#undef READONLY
    READONLY,
#undef WRITEONLY
    WRITEONLY,
#undef COHERENT
    COHERENT,
#undef VOLATILE
    VOLATILE,
#undef RESTRICT
    RESTRICT,
#undef BUFFER
    BUFFER,
#undef HASSIDEEFFECTS
    HASSIDEEFFECTS,
#undef PLS
    PLS,
#undef PLSIN
    PLSIN,
#undef PLSOUT
    PLSOUT,
#undef VARYING
    VARYING,
#undef STRUCT
    STRUCT,
#undef LAYOUT
    LAYOUT,
#undef PRECISION
    PRECISION,
#undef ENUM
    ENUM,
#undef CLASS
    CLASS,
#undef IDENTIFIER
    IDENTIFIER,
#undef DIRECTIVE
    DIRECTIVE,
#undef SECTION
    SECTION,
#undef LPAREN
    LPAREN,
#undef RPAREN
    RPAREN,
#undef LBRACE
    LBRACE,
#undef RBRACE
    RBRACE,
#undef LBRACKET
    LBRACKET,
#undef RBRACKET
    RBRACKET,
#undef DOT
    DOT,
#undef COMMA
    COMMA,
#undef PLUSPLUS
    PLUSPLUS,
#undef MINUSMINUS
    MINUSMINUS,
#undef PLUS
    PLUS,
#undef MINUS
    MINUS,
#undef STAR
    STAR,
#undef SLASH
    SLASH,
#undef PERCENT
    PERCENT,
#undef SHL
    SHL,
#undef SHR
    SHR,
#undef BITWISEOR
    BITWISEOR,
#undef BITWISEXOR
    BITWISEXOR,
#undef BITWISEAND
    BITWISEAND,
#undef BITWISENOT
    BITWISENOT,
#undef LOGICALOR
    LOGICALOR,
#undef LOGICALXOR
    LOGICALXOR,
#undef LOGICALAND
    LOGICALAND,
#undef LOGICALNOT
    LOGICALNOT,
#undef QUESTION
    QUESTION,
#undef COLONCOLON
    COLONCOLON,
#undef COLON
    COLON,
#undef EQ
    EQ,
#undef EQEQ
    EQEQ,
#undef NEQ
    NEQ,
#undef GT
    GT,
#undef LT
    LT,
#undef GTEQ
    GTEQ,
#undef LTEQ
    LTEQ,
#undef PLUSEQ
    PLUSEQ,
#undef MINUSEQ
    MINUSEQ,
#undef STAREQ
    STAREQ,
#undef SLASHEQ
    SLASHEQ,
#undef PERCENTEQ
    PERCENTEQ,
#undef SHLEQ
    SHLEQ,
#undef SHREQ
    SHREQ,
#undef BITWISEOREQ
    BITWISEOREQ,
#undef BITWISEXOREQ
    BITWISEXOREQ,
#undef BITWISEANDEQ
    BITWISEANDEQ,
#undef LOGICALOREQ
    LOGICALOREQ,
#undef LOGICALXOREQ
    LOGICALXOREQ,
#undef LOGICALANDEQ
    LOGICALANDEQ,
#undef SEMICOLON
    SEMICOLON,
#undef ARROW
    ARROW,
#undef WHITESPACE
    WHITESPACE,
#undef LINE_COMMENT
    LINE_COMMENT,
#undef BLOCK_COMMENT
    BLOCK_COMMENT,
#undef INVALID
    INVALID,
  };

  constexpr Token() noexcept : fKind(Kind::INVALID), fOffset(-1), fLength(-1) {}

  Token(Kind kind, int32_t offset, int32_t length) noexcept
      : fKind(kind), fOffset(offset), fLength(length) {}

  Kind fKind;
  int fOffset;
  int fLength;
};

class Lexer {
 public:
  void start(const char* text, int32_t length) noexcept {
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
