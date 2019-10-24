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


void load_word_data(const std::string & data_path, std::unordered_map<std::string, int>& word_map){
  std::string line;
  std::ifstream infile(data_path);
  int i = 0;
  while (std::getline(infile, line))
  {
    word_map[line] = i;
    ++i;
  }
}

int main(int argc, char **argv) {
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

  std::unordered_map<std::string, int> word_set;
  load_word_data(word_data_path, word_set);
  std::cout << word_set.size() << std::endl;
  std::unordered_map<std::string, int>::iterator word_iter = word_set.begin();
  for(; word_iter!=word_set.end(); ++word_iter) {
    if(word_iter->second < 21791){
      continue;
    }
    std::string word_out_dir = out_dir + "/" + std::to_string(word_iter->second);
    std::cout << word_out_dir << std::endl;
    boost::filesystem::create_directory(word_out_dir);
    std::cout << "render word: "<< word_iter->first << std::endl;
    render.render_all_fonts(word_iter->first, word_out_dir);
  }

  return 0;
}
