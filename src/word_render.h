//
// Created by zhengwu on 19-10-7.
//

#ifndef SYNTHTEXT_WORD_RENDER_H
#define SYNTHTEXT_WORD_RENDER_H

#include <vector>
#include <string>
#include <map>
#include <allheaders.h>
#include <pango/pangocairo.h>

class WordRender {
public:
  WordRender() = default;
  ~WordRender() = default;

  void init(
          const std::string &background_dir,
          const std::string &font_dir ,
          double image_backgound_rate);

  void render_line_no_background(
          const std::string &content,
          const std::string &font,
          Pix *&pix,
          std::vector<std::string> &words,
          std::vector<PangoRectangle> &positions);

  void render_line_aug(
      const std::string &content,
      Pix *&pix,
      const std::string &background_image,
      std::vector<std::string> &words,
      std::vector<PangoRectangle> &positions);

  void render_line_background(
          const std::string &content,
          const std::string &font,
          const std::string &background_image,
          Pix *&pix,
          std::vector<std::string> &words,
          std::vector<PangoRectangle> &positions
          );


  void render_word_with_background(
          const std::string &word,
          const std::string &font,
          const std::string &background_image,
          Pix *&pix);

  void render_word_no_background(
          const std::string &word,
          const std::string &font,
          Pix *& pix);
  void render_line_all_fonts(const std::string &content, const std::string &out_image_dir, int font_num);

  void render_word_all_fonts(const std::string & word, const std::string & out_image_dir);

  void crop_text_image(cairo_surface_t *surface, PangoLayout *layout, Pix *&pix, double start_x=0, double start_y=0);

  void crop_line_image(cairo_surface_t *surface, PangoLayout *layout, Pix *&pix,
          std::vector<std::string> &words, std::vector<PangoRectangle> &positions, double start_x = 0, double start_y=0, double angle=0.0);


private:
  // font
  std::vector<std::string> fonts_;
  // background
  std::vector<std::string> background_images_;
  double image_background_rate_ = 0.0;

  void init_font(const std::string font_dir);

  WordRender(const WordRender&);
  void operator=(const WordRender&);

};


#endif //SYNTHTEXT_WORD_RENDER_H
