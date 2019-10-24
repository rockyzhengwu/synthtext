/**********************************************************************
 * File:        normstrngs.cpp
 * Description: Utilities to normalize and manipulate UTF-32 and
 *              UTF-8 strings.
 * Author:      Ranjith Unnikrishnan
 * Created:     Thu July 4 2013
 *
 * (C) Copyright 2013, Google Inc.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * http://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 **********************************************************************/

#include "normstrngs.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>

#include "unicode/normalizer2.h"  // From libicu
#include "unicode/translit.h"     // From libicu
#include "unicode/uchar.h"        // From libicu
#include "unicode/unorm2.h"       // From libicu
#include "unicode/uscript.h"      // From libicu


static bool is_hyphen_punc(const char32 ch) {
  static const int kNumHyphenPuncUnicodes = 13;
  static const char32 kHyphenPuncUnicodes[kNumHyphenPuncUnicodes] = {
      '-',    0x2010, 0x2011, 0x2012,
      0x2013, 0x2014, 0x2015,  // hyphen..horizontal bar
      0x207b,                  // superscript minus
      0x208b,                  // subscript minus
      0x2212,                  // minus sign
      0xfe58,                  // small em dash
      0xfe63,                  // small hyphen-minus
      0xff0d,                  // fullwidth hyphen-minus
  };
  for (int i = 0; i < kNumHyphenPuncUnicodes; ++i) {
    if (kHyphenPuncUnicodes[i] == ch) return true;
  }
  return false;
}

static bool is_single_quote(const char32 ch) {
  static const int kNumSingleQuoteUnicodes = 8;
  static const char32 kSingleQuoteUnicodes[kNumSingleQuoteUnicodes] = {
      '\'', '`',
      0x2018,  // left single quotation mark (English, others)
      0x2019,  // right single quotation mark (Danish, Finnish, Swedish, Norw.)
               // We may have to introduce a comma set with 0x201a
      0x201B,  // single high-reveresed-9 quotation mark (PropList.txt)
      0x2032,  // prime
      0x300C,  // left corner bracket (East Asian languages)
      0xFF07,  // fullwidth apostrophe
  };
  for (int i = 0; i < kNumSingleQuoteUnicodes; ++i) {
    if (kSingleQuoteUnicodes[i] == ch) return true;
  }
  return false;
}

static bool is_double_quote(const char32 ch) {
  static const int kNumDoubleQuoteUnicodes = 8;
  static const char32 kDoubleQuoteUnicodes[kNumDoubleQuoteUnicodes] = {
      '"',
      0x201C,  // left double quotation mark (English, others)
      0x201D,  // right double quotation mark (Danish, Finnish, Swedish, Norw.)
      0x201F,  // double high-reversed-9 quotation mark (PropList.txt)
      0x2033,  // double prime
      0x301D,  // reversed double prime quotation mark (East Asian langs,
               // horiz.)
      0x301E,  // close double prime (East Asian languages written horizontally)
      0xFF02,  // fullwidth quotation mark
  };
  for (int i = 0; i < kNumDoubleQuoteUnicodes; ++i) {
    if (kDoubleQuoteUnicodes[i] == ch) return true;
  }
  return false;
}



// Apply just the OCR-specific normalizations and return the normalized char.
char32 OCRNormalize(char32 ch) {
  if (is_hyphen_punc(ch))
    return '-';
  else if (is_single_quote(ch))
    return '\'';
  else if (is_double_quote(ch))
    return '"';
  return ch;
}

bool IsOCREquivalent(char32 ch1, char32 ch2) {
  return OCRNormalize(ch1) == OCRNormalize(ch2);
}

bool IsValidCodepoint(const char32 ch) {
  // In the range [0, 0xD800) or [0xE000, 0x10FFFF]
  return (static_cast<uint32_t>(ch) < 0xD800) || (ch >= 0xE000 && ch <= 0x10FFFF);
}

bool IsWhitespace(const char32 ch) {
  return u_isUWhiteSpace(static_cast<UChar32>(ch));
}
//
//bool IsUTF8Whitespace(const char* text) {
//  return SpanUTF8Whitespace(text) == strlen(text);
//}
//
//unsigned int SpanUTF8Whitespace(const char* text) {
//  int n_white = 0;
////  icu::UnicodeString ucs =
//  for (UNICHAR::const_iterator it = UNICHAR::begin(text, strlen(text));
//       it != UNICHAR::end(text, strlen(text)); ++it) {
//    if (!IsWhitespace(*it)) break;
//    n_white += it.utf8_len();
//  }
//  return n_white;
//}

bool IsInterchangeValid(const char32 ch) {
  return IsValidCodepoint(ch) &&
         !(ch >= 0xFDD0 && ch <= 0xFDEF) &&  // Noncharacters.
         !(ch >= 0xFFFE && ch <= 0xFFFF) && !(ch >= 0x1FFFE && ch <= 0x1FFFF) &&
         !(ch >= 0x2FFFE && ch <= 0x2FFFF) &&
         !(ch >= 0x3FFFE && ch <= 0x3FFFF) &&
         !(ch >= 0x4FFFE && ch <= 0x4FFFF) &&
         !(ch >= 0x5FFFE && ch <= 0x5FFFF) &&
         !(ch >= 0x6FFFE && ch <= 0x6FFFF) &&
         !(ch >= 0x7FFFE && ch <= 0x7FFFF) &&
         !(ch >= 0x8FFFE && ch <= 0x8FFFF) &&
         !(ch >= 0x9FFFE && ch <= 0x9FFFF) &&
         !(ch >= 0xAFFFE && ch <= 0xAFFFF) &&
         !(ch >= 0xBFFFE && ch <= 0xBFFFF) &&
         !(ch >= 0xCFFFE && ch <= 0xCFFFF) &&
         !(ch >= 0xDFFFE && ch <= 0xDFFFF) &&
         !(ch >= 0xEFFFE && ch <= 0xEFFFF) &&
         !(ch >= 0xFFFFE && ch <= 0xFFFFF) &&
         !(ch >= 0x10FFFE && ch <= 0x10FFFF) &&
         (!u_isISOControl(static_cast<UChar32>(ch)) || ch == '\n' ||
          ch == '\f' || ch == '\t' || ch == '\r');
}

bool IsInterchangeValid7BitAscii(const char32 ch) {
  return IsValidCodepoint(ch) && ch <= 128 &&
         (!u_isISOControl(static_cast<UChar32>(ch)) || ch == '\n' ||
          ch == '\f' || ch == '\t' || ch == '\r');
}

icu::UnicodeString stringToUnicodeString(const std::string & content){
  icu::UnicodeString ucs = icu::UnicodeString::fromUTF8(icu::StringPiece(content.c_str()));
}