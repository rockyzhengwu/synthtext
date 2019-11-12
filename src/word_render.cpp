//
// Created by zhengwu on 19-10-7.
//

#include <boost/filesystem.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>

#include <cstdlib>
#include "word_render.h"
#include "pango_font_info.h"
#include <iostream>
#include <fstream>
#include <iostream>

double get_random_value() {
  int max_value = 100;
  double v = (std::rand() % max_value + 1) / (double) max_value;
  return v;
}

void WordRender::render_line_aug(const std::string &content, Pix *&pix, const std::string &background_image,
                                 std::vector<std::string> &words, std::vector<PangoRectangle> &positions) {

  cairo_surface_t *surface;
  cairo_t *cr;

  surface = cairo_image_surface_create_from_png(background_image.c_str());
  int w = cairo_image_surface_get_width(surface);
  int h = cairo_image_surface_get_height(surface);
  cr = cairo_create(surface);
  PangoLayout *layout = pango_cairo_create_layout(cr);
  int i = 0;

  for (i = 0; i < content.length(); i++)
  {
    int width, height;
    double angle = 2;
    double red;

    cairo_save (cr);
    // set cr 计算下一个字的位置,move to
    pango_cairo_update_layout (cr, layout);

    pango_layout_get_size (layout, &width, &height);


    pango_cairo_show_layout (cr, layout);

    cairo_restore (cr);
  }

}


void WordRender::render_line_no_background(const std::string &content, const std::string &font,
                                           Pix *&pix, std::vector<std::string> &words,
                                           std::vector<PangoRectangle> &positions) {
  // init cairo
  int max_width = 64 * content.size() / 3 + 20;
  int max_height = 64;
  cairo_surface_t *surface;
  cairo_t *cr;
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, max_width, max_height);
  cr = cairo_create(surface);

  PangoLayout *layout = pango_cairo_create_layout(cr);
  PangoFontDescription *desc = pango_font_description_from_string(font.c_str());
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);

  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);
  pango_cairo_context_set_resolution(pango_layout_get_context(layout), 150);

  cairo_move_to(cr, 0.0, 0.0);
  pango_layout_set_text(layout, content.c_str(), -1);
  // background white
  cairo_set_source_rgb(cr, get_random_value(), get_random_value(), get_random_value());
  pango_cairo_update_layout(cr, layout);
  pango_cairo_show_layout(cr, layout);

  crop_line_image(surface, layout, pix, words, positions);
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

void WordRender::render_line_background(const std::string &content, const std::string &font,
                                        const std::string &background_image, Pix *&pix,
                                        std::vector<std::string> &words,
                                        std::vector<PangoRectangle> &positions) {

  cairo_surface_t *surface;
  cairo_t *cr;

  surface = cairo_image_surface_create_from_png(background_image.c_str());
  int w = cairo_image_surface_get_width(surface);
  int h = cairo_image_surface_get_height(surface);
  cr = cairo_create(surface);
  // pango
  PangoLayout *layout = pango_cairo_create_layout(cr);
  PangoFontDescription *desc = pango_font_description_from_string(font.c_str());
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);
  pango_cairo_context_set_resolution(pango_layout_get_context(layout), 150);
  cairo_set_source_rgb(cr, get_random_value(), get_random_value(), get_random_value());

  double start_x = 0;
  double start_y = 0;
  double angle = 0.0;
  if (get_random_value() < 0.5){
    angle = get_random_value() ;
    if(get_random_value() < 0.5){
      angle = angle * -1;
      start_y = h - 32;
    }
    angle = 4 * angle * G_PI / 180;
  }
  cairo_move_to(cr, start_x, start_y);
  cairo_rotate(cr, angle);
  pango_layout_set_text(layout, content.c_str(), -1);
  pango_cairo_show_layout(cr, layout);

  crop_line_image(surface, layout, pix, words, positions, start_x, start_y, angle);
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

void WordRender::crop_line_image(cairo_surface_t *surface, PangoLayout *layout, Pix *&pix,
                                 std::vector<std::string> &words, std::vector<PangoRectangle> &positions,
                                 double start_x, double start_y, double angle) {
  const char *text = pango_layout_get_text(layout);
  PangoLayoutIter *cluster_iter = pango_layout_get_iter(layout);
  std::vector<int> cluster_start_indices;
  do {
    cluster_start_indices.push_back(pango_layout_iter_get_index(cluster_iter));
  } while (pango_layout_iter_next_cluster(cluster_iter));

  cluster_start_indices.push_back(strlen(text));
  pango_layout_iter_free(cluster_iter);

  std::sort(cluster_start_indices.begin(), cluster_start_indices.end());
  std::map<int, int> cluster_start_to_end_index;
  for (size_t i = 0; i + 1 < cluster_start_indices.size(); ++i) {
    cluster_start_to_end_index[cluster_start_indices[i]] = cluster_start_indices[i + 1];
  }
  cluster_iter = pango_layout_get_iter(layout);
  int total_height = 0;

  int w = cairo_image_surface_get_width(surface);
  do {
    PangoRectangle cluster_rect;
    pango_layout_iter_get_cluster_extents(cluster_iter, &cluster_rect, nullptr);
    pango_extents_to_pixels(&cluster_rect, nullptr);
    if (cluster_rect.x + cluster_rect.width > w) {
      continue;
    }
    const int start_byte_index = pango_layout_iter_get_index(cluster_iter);
    const int end_byte_index = cluster_start_to_end_index[start_byte_index];
    std::string cluster_text = std::string(text + start_byte_index, end_byte_index - start_byte_index);
    words.push_back(cluster_text);
    positions.push_back(cluster_rect);
    cluster_rect.x = cluster_rect.x - start_x;
    cluster_rect.y = cluster_rect.y - start_y;
    if (cluster_rect.height > total_height) {
      total_height = cluster_rect.height;
    }
  } while (pango_layout_iter_next_cluster(cluster_iter));
  pango_layout_iter_free(cluster_iter);
  const int width = cairo_image_surface_get_width(surface);
  const int height = cairo_image_surface_get_height(surface);
  Pix *image_pix = pixCreate(width, height, 32);
  int byte_stride = cairo_image_surface_get_stride(surface);
  for (int i = 0; i < height; ++i) {
    memcpy(reinterpret_cast<unsigned char *>(image_pix->data + i * image_pix->wpl) + 1,
           cairo_image_surface_get_data(surface) + i * byte_stride,
           byte_stride - ((i == height - 1) ? 1 : 0));
  }
  // delete last white space word
  int j = words.size() - 1;
  for (; j > 0; j--) {
    if (words[j] != " ") {
      break;
    }
  }
  PangoRectangle lastrect = positions[positions.size()-1];
  int total_width = lastrect.x + lastrect.width ;

  if (angle < 0){
    start_y = (height - 32) - (-1 *  total_width * tan(angle)) ;
    total_height = 32 +(-1 * total_width * tan(angle));
    total_width += 32;
//    start_y += total_width * tan(angle);
  } else{
    total_height = lastrect.y + lastrect.height;
    total_height += total_width * tan(angle);
  }

  BOX *box = boxCreate(start_x, start_y, total_width, total_height);
  pix = pixClipRectangle(image_pix, box, NULL);

  // free tmp image
  boxDestroy(&box);
  pixDestroy(&image_pix);
}

void WordRender::render_line_all_fonts(const std::string &content, const std::string &out_image_dir, int font_num) {
  Pix *pix = nullptr;
  int font_size=  fonts_.size();
  std::vector<int> selected_font_indexs ;

  while(selected_font_indexs.size() < font_num + 1){
    int font_index =  rand() % static_cast<int>(font_size);
    selected_font_indexs.push_back(font_index);
  }

  for (int font_index : selected_font_indexs) {
    std::string font = fonts_[font_index];
    std::string can_render_content = std::string(content.c_str());
    PangoFontInfo pango_font_info(font);
    int dn = pango_font_info.DropUncoveredChars(can_render_content);

    if(can_render_content.length() < 1){
      continue;
    }

//    if (1.0 * can_render_content.length() / content.length() < 0.5) {
//      continue;
//    }

    std::vector<std::string> words;
    std::vector<PangoRectangle> positions;
    if (background_images_.size() == 0 or image_background_rate_ == 0) {
      render_line_no_background(can_render_content, font, pix, words, positions);
    } else {
      if (get_random_value() < image_background_rate_) {
        int index = (int) (get_random_value() * (background_images_.size() - 1));
        render_line_background(can_render_content, font, background_images_[index], pix, words, positions);
      } else {
        render_line_no_background(can_render_content, font, pix, words, positions);
      }
    }
    if (pix != nullptr) {
      boost::uuids::uuid uuid = boost::uuids::random_generator()();
      const std::string image_name = boost::lexical_cast<std::string>(uuid);
      std::string image_path = out_image_dir + "/" + image_name+ ".png";
      pixWritePng(image_path.c_str(), pix, 0);
      pixDestroy(&pix);
      pix = nullptr;
      std::string label_path = image_path.substr(0, image_path.length() - 3) + "txt";
      std::ofstream label_file(label_path);
      if (label_file.is_open()) {
        // todo fix this ugly code  find last not white space
        int j = words.size() - 1;
        for (; j > 0; j--) {
          if (words[j] != " ") {
            break;
          }
        }
        for (int i = 0; i < j + 1; i++) {
          label_file << words[i] << " " << positions[i].x << " " << positions[i].y << " "
                     << positions[i].width << " " << positions[i].height << "\n";
        }
        label_file.close();
      }
    }
  }

  if (pix != nullptr) {
    pixDestroy(&pix);
  }
}

void WordRender::render_word_all_fonts(const std::string &word, const std::string &out_image_dir) {
  Pix *pix = nullptr;
  for (std::string font: fonts_) {
    PangoFontInfo pango_font_info(font);
    if (!pango_font_info.CoversUTF8Text(word.c_str())) {
      continue;
    }
    if (background_images_.size() == 0 or image_background_rate_ == 0) {
      render_word_no_background(word, font, pix);
    } else {
      if (get_random_value() < image_background_rate_) {
        int index = (int) (get_random_value() * (background_images_.size() - 1));
        render_word_with_background(word, font, background_images_[index], pix);
      } else {
        render_word_no_background(word, font, pix);
      }
    }
    if (pix != nullptr) {
      std::string image_path = out_image_dir + "/" + font + ".png";
      pixWritePng(image_path.c_str(), pix, 0);
      pixDestroy(&pix);
      pix = nullptr;
    }
  }
  if (pix != nullptr) {
    pixDestroy(&pix);
  }
}

void WordRender::render_word_no_background(const std::string &word, const std::string &font, Pix *&pix) {
  // init cairo
  cairo_surface_t *surface;
  cairo_t *cr;
  surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 64, 64);
  cr = cairo_create(surface);

  PangoLayout *layout = pango_cairo_create_layout(cr);
  PangoFontDescription *desc = pango_font_description_from_string(font.c_str());
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
  cairo_paint(cr);

  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);
  pango_cairo_context_set_resolution(pango_layout_get_context(layout), 300);

  cairo_move_to(cr, 0.0, 0.0);
  pango_layout_set_text(layout, word.c_str(), -1);
  // background white
  cairo_set_source_rgb(cr, get_random_value(), get_random_value(), get_random_value());
  pango_cairo_update_layout(cr, layout);
  pango_cairo_show_layout(cr, layout);

  crop_text_image(surface, layout, pix);
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

void WordRender::render_word_with_background(const std::string &word, const std::string &font,
                                        const std::string &background_image,
                                        Pix *&pix) {
  // init cairo
  cairo_surface_t *surface;
  cairo_t *cr;
  surface = cairo_image_surface_create_from_png(background_image.c_str());
  int w = cairo_image_surface_get_width(surface);
  int h = cairo_image_surface_get_height(surface);

  cr = cairo_create(surface);
  // pango
  PangoLayout *layout = pango_cairo_create_layout(cr);
  PangoFontDescription *desc = pango_font_description_from_string(font.c_str());
  pango_layout_set_font_description(layout, desc);
  pango_font_description_free(desc);
  pango_cairo_context_set_resolution(pango_layout_get_context(layout), 300);
  cairo_set_source_rgb(cr, get_random_value(), get_random_value(), get_random_value());

  double start_x = 0;
  double start_y = 0;
  if (w > 64) {
    start_x = get_random_value() * (w - 64);
  }
  if (h > 64) {
    start_y = get_random_value() * (h - 64);
  }

  cairo_move_to(cr, start_x, start_y);
  pango_layout_set_text(layout, word.c_str(), -1);
  pango_cairo_show_layout(cr, layout);

  crop_text_image(surface, layout, pix, start_x, start_y);
  // get text rect

  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

void
WordRender::crop_text_image(cairo_surface_t *surface, PangoLayout *layout, Pix *&pix, double start_x, double start_y) {
  // get word text box
  PangoLayoutIter *cluster_iter = pango_layout_get_iter(layout);
  PangoRectangle cluster_rect;
  pango_layout_iter_get_cluster_extents(cluster_iter, &cluster_rect, nullptr);
  pango_layout_iter_free(cluster_iter);
  pango_extents_to_pixels(&cluster_rect, nullptr);

  int w = cairo_image_surface_get_width(surface);
  int h = cairo_image_surface_get_height(surface);
  // pango to pix
  Pix *image_pix = pixCreate(w, h, 32);
  int byte_stride = cairo_image_surface_get_stride(surface);
  for (int i = 0; i < h; ++i) {
    memcpy(reinterpret_cast<unsigned char *>(image_pix->data + i * image_pix->wpl) + 1,
           cairo_image_surface_get_data(surface) + i * byte_stride,
           byte_stride - ((i == h - 1) ? 1 : 0));
  }
  BOX *box = boxCreate(start_x + cluster_rect.x, start_y + cluster_rect.y, cluster_rect.width, cluster_rect.height);
  pix = pixClipRectangle(image_pix, box, NULL);
  // free tmp image
  boxDestroy(&box);
  pixDestroy(&image_pix);
}

void WordRender::init(const std::string &background_dir, const std::string &font_dir, double image_background_rate) {
  image_background_rate_ = image_background_rate;
  init_font(font_dir);
  if (background_dir.empty()) {
    image_background_rate_ = 0.0;
    return;
  }
  boost::filesystem::path boost_dir(background_dir);
  boost::filesystem::directory_iterator boost_dir_iterator(boost_dir);
  for (auto iterator = boost_dir_iterator; iterator != boost::filesystem::directory_iterator(); ++iterator) {
    if (boost::filesystem::is_regular_file(iterator->path())) {
      std::string path_str = iterator->path().string();
      if (path_str.compare(path_str.length() - 3, 3, "png") == 0) {
        background_images_.push_back(iterator->path().string());
      }
    }
  }
}


void WordRender::init_font(const std::string font_dir) {
  std::string font_cache_dir = "/tmp";
  PangoFontInfo::HardInitFontConfig(font_dir, font_cache_dir);
  fonts_ = FontUtils::ListAvailableFonts();
}