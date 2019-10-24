//
// Created by zhengwu on 19-10-4.
//

#ifndef SYNTHTEXT_FONT_INFO_H
#define SYNTHTEXT_FONT_INFO_H
#include <string>
#include <vector>

#include "pango/pango-font.h"
#include "pango/pango.h"
#include "pango/pangocairo.h"


std::string delete_not_converage(const std::string font_path, const std::string& language, const std::string content);
PangoFontDescription *get_desc(const std::string font_path);

#endif //SYNTHTEXT_FONT_INFO_H
