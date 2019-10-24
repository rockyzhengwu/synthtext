//
// Created by zhengwu on 19-10-19.
//
#include <iostream>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>
#include <pango/pangocairo.h>
#include <cairo.h>
#include <fontconfig/fontconfig.h>
#include <fstream>
#include <unordered_map>
// from leptonica
#include "allheaders.h"
#include "font_info.h"
#include "pango_font_info.h"
#include "word_render.h"
#include "word_render.h"



void load_line_data(const std::string & data_path, std::vector<std::string >& lines){
  std::string line;
  std::ifstream infile(data_path);
  while (std::getline(infile, line))
  {
    lines.push_back(line);
  }
}

int main(int argc, char** argv){
  std::string word_data_path = "";
  std::string font_dir = "";
  std::string font_cache_dir = "/tmp";
  std::string out_dir = "";
  std::string background_image_dir = "";

  if(argc < 4){
    std::cout<< "Usage: ./render_word <word_data_path> <font_dir> <out_image_dir>";
    return 0;
  }

  word_data_path = argv[1];
  font_dir = argv[2];
  out_dir = argv[3];
  background_image_dir=argv[4];

  WordRender render;
  render.init(background_image_dir, font_dir, 0.8 );
  std::vector<std::string> line_list;
  load_line_data(word_data_path, line_list);
  std::cout << "total line size: " << line_list.size() << std::endl;
  std::vector<std::string>::iterator line_iter = line_list.begin();
  int counter = 0;
  int font_num = 3;
  for(; line_iter!=line_list.end(); ++line_iter) {
    std::cout << counter << std::endl;
    std::string word_out_dir = out_dir + "/" + std::to_string(counter / 10000 );
    std::cout << word_out_dir << std::endl;
    boost::filesystem::create_directory(word_out_dir);
    //std::cout << "render word: "<< line_iter->data() << std::endl;
    render.render_line_all_fonts(line_iter->data(), word_out_dir, font_num);
    counter++;
  }
}
