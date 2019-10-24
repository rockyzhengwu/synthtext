//
// Created by zhengwu on 19-10-4.
//

#include "font_info.h"
#include <iostream>
#include <unicode/unistr.h>


std::string delete_not_converage(const std::string font_path, const std::string& language, const std::string content){
  PangoLanguage *lan = pango_language_from_string(language.c_str());
  PangoFontDescription *desc = pango_font_description_from_string(font_path.c_str());

  PangoFontMap *font_map = pango_cairo_font_map_new();
  PangoContext *context = pango_context_new();
  pango_context_set_font_map(context, font_map);
  pango_context_set_language(context, lan);

  PangoFont *font = pango_context_load_font(context, desc);
  PangoCoverage *coverage = pango_font_get_coverage(font, nullptr);

  icu::UnicodeString ucs = icu::UnicodeString::fromUTF8(icu::StringPiece(content.c_str()));
  std::string res = "";
  for (int i = 0; i < ucs.length(); ++i){
    int unicode = ucs[i];
    if (!pango_is_zero_width(unicode)){
      std::cout<< pango_coverage_get(coverage, unicode)  << std::endl;
      if (pango_coverage_get(coverage, unicode)!=PANGO_COVERAGE_EXACT){
        continue;
      }
    }
    icu::UnicodeString uni_str((UChar32)unicode);
    std::string  word;
    uni_str.toUTF8String(word);
    res += word;
  }

  pango_coverage_unref(coverage);
  g_object_unref(font);
  g_object_unref(context);
  return res;
}
PangoFontDescription *get_desc(const std::string font_path){
  PangoFontDescription *desc = pango_font_description_from_string(font_path.c_str());
  return desc;
}
