// Copyright 2019 Fuzhou Rockchip Electronics Co., Ltd. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <codecvt>
#include <locale>

#include <sys/prctl.h>

#include "color_table.h"
#include "dbus_dbserver_key.h"
#include "flow_db_protocol.h"
#include "flow_manager.h"
#include "osd_server.h"

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "osd_server.cpp"

namespace rockchip {
namespace aiserver {

static int CalcWcharLen(std::wstring wstr) {
  int len = 0;
  for (auto &iter : wstr)
    (iter > 0x7F) ? (len += 2) : (len++);
  return len;
}

static std::wstring String2Wstring(const std::string &str) {
  static std::wstring_convert<std::codecvt_utf8<wchar_t>> strCnv;
  return strCnv.from_bytes(str);
}

std::shared_ptr<OSDProducer> OSDServer::osd_producer_ =
    std::make_shared<OSDProducer>();

OSDServer::OSDServer(std::shared_ptr<easymedia::Flow> &flow, int w, int h)
    : delay_ms_(33), encoder_flow_(flow), view_width_(w), view_height_(h) {
  gradient_x_ = (float)view_width_ / WEB_VIEW_RECT_W;
  gradient_y_ = (float)view_height_ / WEB_VIEW_RECT_H;

  memset(&region_invade_, 0, sizeof(region_invade_s));

  easymedia::video_encoder_set_osd_plt(encoder_flow_,
                                       (uint32_t *)yuv444_palette_table);
  for (int i = OSD_REGION_ID_TIMEDATE; i <= OSD_REGION_ID_LAYER7; i++) {
    memset(&region_data_[i], 0, sizeof(OsdRegionData));
    region_data_[i].region_id = i;
    region_data_[i].enable = 0;
    easymedia::video_encoder_set_osd_region(encoder_flow_, &region_data_[i]);
  }
  ParseDbConfig();
  SyncDbConfig();
}

OSDServer::~OSDServer() { LOG_DEBUG("osd server uninstall\n"); }

void OSDServer::DumpOsdData() {
  for (int i = OSD_DB_REGION_ID_CHANNEL; i <= OSD_DB_REGION_ID_IMAGE; i++) {
    LOG_DEBUG("DumpOsdData region %d\n", i);
    LOG_DEBUG("DumpOsdData type     %d\n", osds_db_data_[i].type);
    LOG_DEBUG("DumpOsdData enable   %d\n", osds_db_data_[i].enable);
    LOG_DEBUG("DumpOsdData origin_x %d\n", osds_db_data_[i].origin_x);
    LOG_DEBUG("DumpOsdData origin_y %d\n", osds_db_data_[i].origin_y);
    if (!osds_db_data_[i].enable) {
      LOG_DEBUG("\n");
      continue;
    }
    if (osds_db_data_[i].type == OSD_TYPE_DATE ||
        osds_db_data_[i].type == OSD_TYPE_TEXT) {
      LOG_DEBUG("DumpOsdData text.format     %s\n",
                osds_db_data_[i].text.format);
      LOG_DEBUG("DumpOsdData text.font_path  %s\n",
                osds_db_data_[i].text.font_path);
      LOG_DEBUG("DumpOsdData text.font_size  %d\n",
                osds_db_data_[i].text.font_size);
      LOG_DEBUG("DumpOsdData text.font_color %x\n",
                osds_db_data_[i].text.font_color);
    }
    LOG_DEBUG("\n");
  }
}

void OSDServer::DefaultConfig() {
  for (int i = OSD_DB_REGION_ID_CHANNEL; i <= OSD_DB_REGION_ID_IMAGE; i++) {
    memset(&osds_db_data_[i], 0, sizeof(osd_data_s));
    osds_db_data_change_[i] = 0;
  }
  osd_data_s *draw_data = &osds_db_data_[OSD_DB_REGION_ID_DATETIME];
  draw_data->enable = true;
  draw_data->text.font_path = FONT_PATH;
  draw_data->type = OSD_TYPE_DATE;
  draw_data->text.font_size = 32;
  if (gradient_x_ > 2.0)
    draw_data->text.font_size *= 2;

  draw_data->text.font_color = 0xfba284;
  draw_data->origin_x = MULTI_UPALIGNTO16(gradient_x_, 16);
  draw_data->origin_y = MULTI_UPALIGNTO16(gradient_y_, 16);
  std::string format;
  format.append(OSD_FMT_TIME1).append(OSD_FMT_SPACE);
  format.append(OSD_FMT_WEEK0).append(OSD_FMT_SPACE);
  format.append(OSD_FMT_YMD0);
  strncpy(draw_data->text.format, format.c_str(),
          sizeof(draw_data->text.format));
  osds_db_data_change_[OSD_DB_REGION_ID_DATETIME] = 1;
}

void OSDServer::ParseDbConfig() {
  FlowManagerPtr &flow_manager = FlowManager::GetInstance();
  if (flow_manager->OsdParamsEmpty()) {
    DefaultConfig();
    return;
  }
  for (int i = OSD_DB_REGION_ID_CHANNEL; i <= OSD_DB_REGION_ID_IMAGE; i++) {
    memset(&osds_db_data_[i], 0, sizeof(osd_data_s));
    osds_db_data_change_[i] = 0;
    auto &config_map = flow_manager->GetOsdParams(i);
    if (!config_map.size())
      continue;
    if (config_map[DB_OSD_TYPE] == DB_OSD_TYPE_DATE) {
      SetOsdDateRegion(i, config_map);
    } else if (config_map[DB_OSD_TYPE] == DB_OSD_TYPE_CHANNLE ||
               config_map[DB_OSD_TYPE] == DB_OSD_TYPE_TEXT) {
      SetOsdTextRegion(i, config_map);
    } else if (config_map[DB_OSD_TYPE] == DB_OSD_TYPE_MASK) {
      SetOsdMaskRegion(i, config_map);
    } else if (config_map[DB_OSD_TYPE] == DB_OSD_TYPE_IMAGE) {
      SetOsdImageRegion(i, config_map);
    }
  }
}

void OSDServer::SyncDbConfig() {
  // TODO Fill region data
  osd_data_s *draw_data = &osds_db_data_[OSD_DB_REGION_ID_DATETIME];
  region_data_[OSD_REGION_ID_TIMEDATE].enable = draw_data->enable;
  region_data_[OSD_REGION_ID_TIMEDATE].pos_x = draw_data->origin_x;
  region_data_[OSD_REGION_ID_TIMEDATE].pos_y = draw_data->origin_y;
}

void OSDServer::SetOsdTextRegion(int region_id,
                                 std::map<std::string, std::string> map) {
  osd_data_s *draw_data;

  if (region_id >= OSD_DB_REGION_ID_TEXT2 &&
      region_id <= OSD_DB_REGION_ID_TEXT7)
    return;

  LOG_INFO("SetOsdTextRegion index %d\n", region_id);

  draw_data = &osds_db_data_[region_id];

  draw_data->type = OSD_TYPE_TEXT;
  draw_data->text.font_path = FONT_PATH;

  if (!map[DB_OSD_ENABLED].empty()) {
    draw_data->enable = atoi(map[DB_OSD_ENABLED].c_str());
  }

  if (!map[DB_OSD_POSITION_X].empty()) {
    draw_data->origin_x =
        MULTI_UPALIGNTO16(gradient_x_, atoi(map[DB_OSD_POSITION_X].c_str()));
  }

  if (!map[DB_OSD_POSITION_Y].empty()) {
    draw_data->origin_y =
        MULTI_UPALIGNTO16(gradient_y_, atoi(map[DB_OSD_POSITION_Y].c_str()));
  }

  if (!map[DB_OSD_DISPLAY_TEXT].empty()) {
    std::wstring wstr = String2Wstring(map[DB_OSD_DISPLAY_TEXT]);
    int len = CalcWcharLen(wstr);
    memset(draw_data->text.wch, 0, sizeof(draw_data->text.wch));
    memcpy(draw_data->text.wch, wstr.c_str(), len * sizeof(wchar_t));
  }

  if (!map[DB_OSD_FRONT_COLOR].empty()) {
    sscanf(map[DB_OSD_FRONT_COLOR].c_str(), "%x",
           &(draw_data->text.font_color));
  }

  if (!map[DB_OSD_FONT_SIZE].empty()) {
    draw_data->text.font_size = atoi(map[DB_OSD_FONT_SIZE].c_str());
    if (gradient_x_ > 1.5)
      draw_data->text.font_size *= 2;
  }

  osds_db_data_change_[region_id] = 1;
}

void OSDServer::SetOsdDateRegion(int region_id,
                                 std::map<std::string, std::string> map) {
  osd_data_s *draw_data;

  LOG_INFO("SetOsdDateRegion index %d\n", region_id);

  draw_data = &osds_db_data_[region_id];

  if (!map[DB_OSD_ENABLED].empty()) {
    draw_data->enable = atoi(map[DB_OSD_ENABLED].c_str());
  }

  draw_data->text.font_path = FONT_PATH;
  draw_data->type = OSD_TYPE_DATE;

  if (!map[DB_OSD_FONT_SIZE].empty()) {
    draw_data->text.font_size = atoi(map[DB_OSD_FONT_SIZE].c_str());
    if (gradient_x_ > 1.5)
      draw_data->text.font_size *= 2;
  }

  if (!map[DB_OSD_FRONT_COLOR].empty()) {
    sscanf(map[DB_OSD_FRONT_COLOR].c_str(), "%x",
           &(draw_data->text.font_color));
  }

  if (!map[DB_OSD_POSITION_X].empty()) {
    draw_data->origin_x =
        MULTI_UPALIGNTO16(gradient_x_, atoi(map[DB_OSD_POSITION_X].c_str()));
  }

  if (!map[DB_OSD_POSITION_Y].empty()) {
    draw_data->origin_y =
        MULTI_UPALIGNTO16(gradient_y_, atoi(map[DB_OSD_POSITION_Y].c_str()));
  }

  std::string format = "";
  if (!map[DB_OSD_TIME_STYLE].empty()) {
    if (map[DB_OSD_TIME_STYLE] == DB_OSD_TIME_STYLE_12) {
      format.append(OSD_FMT_TIME1).append(OSD_FMT_SPACE);
    } else {
      format.append(OSD_FMT_TIME0).append(OSD_FMT_SPACE);
    }
  }

  if (!map[DB_OSD_DISPLAY_WEEK_ENABLED].empty()) {
    if (map[DB_OSD_DISPLAY_WEEK_ENABLED] == DB_VALUE_ENABLE) {
      format.append(OSD_FMT_WEEK0).append(OSD_FMT_SPACE);
    }
  }

  if (!map[DB_OSD_DATE_STYLE].empty()) {
    format.append(map[DB_OSD_DATE_STYLE]);
  }

  if (!format.empty()) {
    strncpy(draw_data->text.format, format.c_str(),
            sizeof(draw_data->text.format));
  }

  if (!map[DB_OSD_FRONT_COLOR_MODE].empty()) {
    if (map[DB_OSD_FRONT_COLOR_MODE] == DB_VALUE_AUTO)
      draw_data->text.color_inverse = 1;
    else
      draw_data->text.color_inverse = 0;
  }

  osds_db_data_change_[region_id] = 1;
}

void OSDServer::SetOsdImageRegion(int region_id,
                                  std::map<std::string, std::string> map) {
  osd_data_s *draw_data;

  LOG_INFO("SetOsdImageRegion index %d\n", region_id);

  draw_data = &osds_db_data_[region_id];

  draw_data->type = OSD_TYPE_IMAGE;
  draw_data->image = IMAGE_PATH;

  if (!map[DB_OSD_ENABLED].empty()) {
    draw_data->enable = atoi(map[DB_OSD_ENABLED].c_str());
  }

  if (!map[DB_OSD_POSITION_X].empty()) {
    draw_data->origin_x =
        MULTI_UPALIGNTO16(gradient_x_, atoi(map[DB_OSD_POSITION_X].c_str()));
  }

  if (!map[DB_OSD_POSITION_Y].empty()) {
    draw_data->origin_y =
        MULTI_UPALIGNTO16(gradient_y_, atoi(map[DB_OSD_POSITION_Y].c_str()));
  }

  osd_producer_->GetImageInfo(draw_data);

  osds_db_data_change_[region_id] = 1;
}

void OSDServer::SetOsdMaskRegion(int region_id,
                                 std::map<std::string, std::string> map) {
  osd_data_s *draw_data;

  if (region_id >= OSD_DB_REGION_ID_MASK1 &&
      region_id <= OSD_DB_REGION_ID_MASK3)
    return;

  LOG_INFO("SetOsdMaskRegion index %d\n", region_id);

  draw_data = &osds_db_data_[region_id];

  draw_data->type = OSD_TYPE_IMAGE;
  draw_data->image = nullptr;

  if (!map[DB_OSD_ENABLED].empty()) {
    draw_data->enable = atoi(map[DB_OSD_ENABLED].c_str());
  }

  if (!map[DB_OSD_POSITION_X].empty()) {
    draw_data->origin_x = atoi(map[DB_OSD_POSITION_X].c_str());
    draw_data->origin_x = MULTI_UPALIGNTO16(gradient_x_, draw_data->origin_x);
  }

  if (!map[DB_OSD_POSITION_Y].empty()) {
    draw_data->origin_y = atoi(map[DB_OSD_POSITION_Y].c_str());
    draw_data->origin_y = MULTI_UPALIGNTO16(gradient_y_, draw_data->origin_y);
  }

  if (!map[DB_OSD_WIDTH].empty()) {
    draw_data->width = atoi(map[DB_OSD_WIDTH].c_str());
    draw_data->width = MULTI_UPALIGNTO16(gradient_x_, draw_data->width);
  }

  if (!map[DB_OSD_HTIGHT].empty()) {
    draw_data->height = atoi(map[DB_OSD_HTIGHT].c_str());
    draw_data->height = MULTI_UPALIGNTO16(gradient_y_, draw_data->height);
  }

  osds_db_data_change_[region_id] = 1;
}

void OSDServer::SetOsdRegion(int region_id,
                             std::map<std::string, std::string> map) {
  LOG_INFO("SetOsdRegion region_id %d\n", region_id);

  if (region_id == OSD_DB_REGION_ID_CHANNEL) {
    SetOsdTextRegion(region_id, map);
  } else if (region_id >= OSD_DB_REGION_ID_TEXT0 &&
             region_id <= OSD_DB_REGION_ID_TEXT7) {
    SetOsdTextRegion(region_id, map);
  } else if (region_id >= OSD_DB_REGION_ID_MASK0 &&
             region_id <= OSD_DB_REGION_ID_MASK3) {
    SetOsdMaskRegion(region_id, map);
  } else if (region_id == OSD_DB_REGION_ID_IMAGE) {
    SetOsdImageRegion(region_id, map);
  } else if (region_id == OSD_DB_REGION_ID_DATETIME) {
    SetOsdDateRegion(region_id, map);
  }
}

void OSDServer::SetOsdTip(int x, int y, std::string tip) {
  LOG_INFO("OSDServer SetOsdTip [%dx%d] %s\n", view_width_, view_height_,
           tip.c_str());
  int region_id = OSD_REGION_ID_LAYER7;
  osd_data_s osds_db_data;
  osds_db_data.enable = 1;
  osds_db_data.origin_x = MULTI_UPALIGNTO16(1, x);
  osds_db_data.origin_y = MULTI_UPALIGNTO16(1, y);
  osds_db_data.type = OSD_TYPE_TEXT;
  osds_db_data.text.font_path = FONT_PATH;
  osds_db_data.text.font_color = 0xFFFF80;
  osds_db_data.text.font_size = 32;
  if (gradient_x_ > 1.5)
    osds_db_data.text.font_size *= 2;

  std::wstring wstr = String2Wstring(tip);
  int len = CalcWcharLen(wstr);
  memset(osds_db_data.text.wch, 0, sizeof(osds_db_data.text.wch));
  memcpy(osds_db_data.text.wch, wstr.c_str(), len * sizeof(wchar_t));

  osd_data_s *draw_data = &osds_db_data;
  OsdRegionData *osd_region_data = &region_data_[region_id];
  draw_data->width = wcslen(draw_data->text.wch) * draw_data->text.font_size;
  draw_data->height = draw_data->text.font_size;
  draw_data->width = MULTI_UPALIGNTO16(1, draw_data->width);
  draw_data->height = MULTI_UPALIGNTO16(1, draw_data->height);
  draw_data->size = draw_data->width * draw_data->height;
  draw_data->buffer = static_cast<uint8_t *>(malloc(draw_data->size));
  if (gradient_x_ < 1) {
    int orign = MULTI_UPALIGNTO16(1, draw_data->width);
    int dest = MULTI_UPALIGNTO16(gradient_x_, draw_data->width);
    draw_data->origin_x -= (orign - dest);
  }
  memset(draw_data->buffer, 0xFF, draw_data->size);
  osd_producer_->FillYuvMap(draw_data);

  osd_region_data->enable = draw_data->enable;
  osd_region_data->region_id = region_id;
  osd_region_data->pos_x = draw_data->origin_x;
  osd_region_data->pos_y = draw_data->origin_y;
  osd_region_data->width = draw_data->width;
  osd_region_data->height = draw_data->height;
  osd_region_data->buffer = draw_data->buffer;
  easymedia::video_encoder_set_osd_region(encoder_flow_, osd_region_data);
  free(draw_data->buffer);
}

void OSDServer::BlinkRegionInvade() {
  if (!region_invade_.enable || !region_invade_.blink_cnt_)
    return;

  if (region_invade_.blink_interval_ms_ < delay_ms_)
    return;

  if ((region_invade_.blink_wait_cnt_ * delay_ms_) <
      region_invade_.blink_interval_ms_) {
    region_invade_.blink_wait_cnt_++;
    return;
  }

#if 1
  SetRegionInvade(region_invade_, BORDER_WATERFULL_LIGHT);
#else
  if (region_invade_.blink_status_) {
    SetRegionInvade(region_invade_, BORDER_DOTTED);
  } else {
    SetRegionInvade(region_invade_, BORDER_LINE);
  }
#endif
  region_invade_.blink_wait_cnt_ = 0;
  region_invade_.blink_cnt_--;
  region_invade_.blink_status_ = !region_invade_.blink_status_;
}

void OSDServer::SetRegionInvade(region_invade_s region_invade,
                                int display_style) {
  LOG_INFO("SetRegionInvade region_invade [%dx%d] display_style %d\n",
           view_width_, view_height_, display_style);
  int region_id = OSD_REGION_ID_LAYER6;
  int thick = 10;
  int color_index = 0x23;
  region_invade_ = region_invade;
  int x = MULTI_UPALIGNTO16(1, (int)(region_invade.position_x * gradient_x_));
  int y = MULTI_UPALIGNTO16(1, (int)(region_invade.position_y * gradient_y_));
  int w = MULTI_UPALIGNTO16(gradient_x_, region_invade.width);
  int h = MULTI_UPALIGNTO16(gradient_y_, region_invade.height);

  osd_data_s osds_db_data;
  osds_db_data.enable = region_invade.enable;
  osds_db_data.origin_x = x;
  osds_db_data.origin_y = y;
  osds_db_data.type = OSD_TYPE_BORDER;
  osd_data_s *draw_data = &osds_db_data;
  OsdRegionData *osd_region_data = &region_data_[region_id];
  draw_data->width = MULTI_UPALIGNTO16(1, w);
  draw_data->height = MULTI_UPALIGNTO16(1, h);
  draw_data->border.display_style = display_style;
  draw_data->border.thick = thick;
  draw_data->border.color_index = color_index;
  draw_data->border.color_key = 0xFF;
  draw_data->size = draw_data->width * draw_data->height;
  draw_data->buffer = static_cast<uint8_t *>(malloc(draw_data->size));
  memset(draw_data->buffer, 0xFF, draw_data->size);
  osd_producer_->FillYuvMap(draw_data);

  osd_region_data->enable = draw_data->enable;
  osd_region_data->region_id = region_id;
  osd_region_data->pos_x = draw_data->origin_x;
  osd_region_data->pos_y = draw_data->origin_y;
  osd_region_data->width = draw_data->width;
  osd_region_data->height = draw_data->height;
  osd_region_data->buffer = draw_data->buffer;
  easymedia::video_encoder_set_osd_region(encoder_flow_, osd_region_data);
  free(draw_data->buffer);
}

void OSDServer::SetRegionInvadeBlink(int blink_cnt, int interval_ms) {
  LOG_INFO("SetRegionInvadeBlink region_invade [%dx%d]\n", view_width_,
           view_height_);
  region_invade_.blink_cnt_ = blink_cnt;
  region_invade_.blink_interval_ms_ = interval_ms;
}

int OSDServer::UpdateBlink() {
  BlinkRegionInvade();
  return 0;
}

int OSDServer::UpdateText() {
  if (osds_db_data_change_[OSD_DB_REGION_ID_CHANNEL]) {
    LOG_INFO("UpdateText OSD_DB_REGION_ID_CHANNEL\n");
    int region_id = OSD_REGION_ID_LAYER1;
    OsdRegionData *osd_region_data = &region_data_[region_id];
    osd_data_s *draw_data = &osds_db_data_[OSD_DB_REGION_ID_CHANNEL];
    draw_data->width = wcslen(draw_data->text.wch) * draw_data->text.font_size;
    draw_data->height = draw_data->text.font_size;
    draw_data->width = MULTI_UPALIGNTO16(1, draw_data->width);
    draw_data->height = MULTI_UPALIGNTO16(1, draw_data->height);
    draw_data->size = draw_data->width * draw_data->height;
    draw_data->buffer = static_cast<uint8_t *>(malloc(draw_data->size));
    if (gradient_x_ < 1) {
      int orign = MULTI_UPALIGNTO16(1, draw_data->width);
      int dest = MULTI_UPALIGNTO16(gradient_x_, draw_data->width);
      draw_data->origin_x -= (orign - dest);
    }
    memset(draw_data->buffer, 0xFF, draw_data->size);
    osd_producer_->FillYuvMap(draw_data);
    osd_region_data->enable = draw_data->enable;
    osd_region_data->region_id = region_id;
    osd_region_data->pos_x = draw_data->origin_x;
    osd_region_data->pos_y = draw_data->origin_y;
    osd_region_data->width = draw_data->width;
    osd_region_data->height = draw_data->height;
    osd_region_data->buffer = draw_data->buffer;
    easymedia::video_encoder_set_osd_region(encoder_flow_, osd_region_data);
    free(draw_data->buffer);
    osds_db_data_change_[OSD_DB_REGION_ID_CHANNEL] = 0;
  }
  if (osds_db_data_change_[OSD_DB_REGION_ID_TEXT0]) {
    LOG_INFO("UpdateText OSD_DB_REGION_ID_TEXT0\n");
    int region_id = OSD_REGION_ID_LAYER3;
    OsdRegionData *osd_region_data = &region_data_[region_id];
    osd_data_s *draw_data = &osds_db_data_[OSD_DB_REGION_ID_TEXT0];
    draw_data->width = wcslen(draw_data->text.wch) * draw_data->text.font_size;
    draw_data->height = draw_data->text.font_size;
    draw_data->size = draw_data->width * draw_data->height;
    draw_data->buffer = static_cast<uint8_t *>(malloc(draw_data->size));
    memset(draw_data->buffer, 0xFF, draw_data->size);
    osd_producer_->FillYuvMap(draw_data);
    osd_region_data->enable = draw_data->enable;
    osd_region_data->region_id = region_id;
    osd_region_data->pos_x = draw_data->origin_x;
    osd_region_data->pos_y = draw_data->origin_y;
    osd_region_data->width = draw_data->width;
    osd_region_data->height = draw_data->height;
    osd_region_data->buffer = draw_data->buffer;
    easymedia::video_encoder_set_osd_region(encoder_flow_, osd_region_data);
    free(draw_data->buffer);
    osds_db_data_change_[OSD_DB_REGION_ID_TEXT0] = 0;
  }
  if (osds_db_data_change_[OSD_DB_REGION_ID_TEXT1]) {
    LOG_INFO("UpdateText OSD_DB_REGION_ID_TEXT1\n");
    int region_id = OSD_REGION_ID_LAYER5;
    OsdRegionData *osd_region_data = &region_data_[region_id];
    osd_data_s *draw_data = &osds_db_data_[OSD_DB_REGION_ID_TEXT1];
    draw_data->width = wcslen(draw_data->text.wch) * draw_data->text.font_size;
    draw_data->height = draw_data->text.font_size;
    draw_data->size = draw_data->width * draw_data->height;
    draw_data->buffer = static_cast<uint8_t *>(malloc(draw_data->size));
    memset(draw_data->buffer, 0xFF, draw_data->size);
    osd_producer_->FillYuvMap(draw_data);
    osd_region_data->enable = draw_data->enable;
    osd_region_data->region_id = region_id;
    osd_region_data->pos_x = draw_data->origin_x;
    osd_region_data->pos_y = draw_data->origin_y;
    osd_region_data->width = draw_data->width;
    osd_region_data->height = draw_data->height;
    osd_region_data->buffer = draw_data->buffer;
    easymedia::video_encoder_set_osd_region(encoder_flow_, osd_region_data);
    free(draw_data->buffer);
    osds_db_data_change_[OSD_DB_REGION_ID_TEXT1] = 0;
  }
  if (osds_db_data_change_[OSD_DB_REGION_ID_TEXT3] ||
      osds_db_data_change_[OSD_DB_REGION_ID_TEXT4]) {
    // OSD_REGION_ID_LAYER6
    // TODO
    LOG_INFO("UpdateText OSD_REGION_ID_LAYER6 TODO\n");
    osds_db_data_change_[OSD_DB_REGION_ID_TEXT3] = 0;
    osds_db_data_change_[OSD_DB_REGION_ID_TEXT4] = 0;
  }
  if (osds_db_data_change_[OSD_DB_REGION_ID_TEXT5] ||
      osds_db_data_change_[OSD_DB_REGION_ID_TEXT6] ||
      osds_db_data_change_[OSD_DB_REGION_ID_TEXT7]) {
    // OSD_REGION_ID_LAYER7
    // TODO
    LOG_INFO("UpdateText OSD_REGION_ID_LAYER7 TODO\n");
    osds_db_data_change_[OSD_DB_REGION_ID_TEXT5] = 0;
    osds_db_data_change_[OSD_DB_REGION_ID_TEXT6] = 0;
    osds_db_data_change_[OSD_DB_REGION_ID_TEXT7] = 0;
  }
  return 0;
}

int OSDServer::UpdateImage() {
  if (osds_db_data_change_[OSD_DB_REGION_ID_IMAGE]) {
    int region_id = OSD_REGION_ID_LAYER2;
    OsdRegionData *osd_region_data = &region_data_[region_id];
    osd_data_s *draw_data = &osds_db_data_[OSD_DB_REGION_ID_IMAGE];
    draw_data->size = draw_data->width * draw_data->height;
    draw_data->buffer = static_cast<uint8_t *>(malloc(draw_data->size));
    memset(draw_data->buffer, 0xFF, draw_data->size);
    osd_producer_->FillYuvMap(draw_data);
    osd_region_data->enable = draw_data->enable;
    osd_region_data->region_id = region_id;
    osd_region_data->pos_x = draw_data->origin_x;
    osd_region_data->pos_y = draw_data->origin_y;
    osd_region_data->width = draw_data->width;
    osd_region_data->height = draw_data->height;
    osd_region_data->buffer = draw_data->buffer;
    LOG_DEBUG("UpdateImage enable %d x %d y %d w %d h %d\n", draw_data->enable,
              draw_data->origin_x, draw_data->origin_y, draw_data->width,
              draw_data->height);
    easymedia::video_encoder_set_osd_region(encoder_flow_, osd_region_data);
    free(draw_data->buffer);
    osds_db_data_change_[OSD_DB_REGION_ID_IMAGE] = 0;
  }
  return 0;
}

int OSDServer::UpdateMask() {
  if (osds_db_data_change_[OSD_DB_REGION_ID_MASK0]) {
    int region_id = OSD_REGION_ID_LAYER4;
    OsdRegionData *osd_region_data = &region_data_[region_id];
    osd_data_s *draw_data = &osds_db_data_[OSD_DB_REGION_ID_MASK0];
    osds_db_data_change_[OSD_DB_REGION_ID_MASK0] = 0;
    if (draw_data->width && draw_data->height) {
      draw_data->size = draw_data->width * draw_data->height;
      draw_data->buffer = static_cast<uint8_t *>(malloc(draw_data->size));
      memset(draw_data->buffer, COLOR_BLACK_INDEX, draw_data->size);
      osd_region_data->enable = draw_data->enable;
      osd_region_data->region_id = region_id;
      osd_region_data->pos_x = draw_data->origin_x;
      osd_region_data->pos_y = draw_data->origin_y;
      osd_region_data->width = draw_data->width;
      osd_region_data->height = draw_data->height;
      osd_region_data->buffer = draw_data->buffer;
      LOG_DEBUG("UpdateMask enable %d x %d y %d w %d h %d\n", draw_data->enable,
                draw_data->origin_x, draw_data->origin_y, draw_data->width,
                draw_data->height);
      easymedia::video_encoder_set_osd_region(encoder_flow_, osd_region_data);
      free(draw_data->buffer);
    }
    osds_db_data_change_[OSD_DB_REGION_ID_IMAGE] = 0;
  }
  if (osds_db_data_change_[OSD_DB_REGION_ID_MASK1] ||
      osds_db_data_change_[OSD_DB_REGION_ID_MASK2] ||
      osds_db_data_change_[OSD_DB_REGION_ID_MASK3]) {
    // OSD_REGION_ID_LAYER4
    // TODO
    LOG_INFO("UpdateText OSD_REGION_ID_LAYER4 TODO\n");
    osds_db_data_change_[OSD_DB_REGION_ID_MASK1] = 0;
    osds_db_data_change_[OSD_DB_REGION_ID_MASK2] = 0;
    osds_db_data_change_[OSD_DB_REGION_ID_MASK3] = 0;
  }
  return 0;
}

void OSDServer::GenDataTime(const char *fmt, wchar_t *result, int r_size) {
  char year[8] = {0}, month[4] = {0}, day[4] = {0};
  char week[16] = {0}, hms[12] = {0};
  wchar_t w_ymd[16] = {0};
  wchar_t w_week[16] = {0};
  int wid = -1;
  int wchar_cnt = 0;

  time_t curtime;
  curtime = time(0);
  strftime(year, sizeof(year), "%Y", localtime(&curtime));
  strftime(month, sizeof(month), "%m", localtime(&curtime));
  strftime(day, sizeof(day), "%d", localtime(&curtime));

  if (strstr(fmt, OSD_FMT_TIME0)) {
    strftime(hms, sizeof(hms), "%H:%M:%S", localtime(&curtime));
  } else if (strstr(fmt, OSD_FMT_TIME1)) {
    strftime(hms, sizeof(hms), "%I:%M:%S %p", localtime(&curtime));
  }

  wchar_cnt = sizeof(w_week) / sizeof(wchar_t);
  if (strstr(fmt, OSD_FMT_WEEK0)) {
    strftime(week, sizeof(week), "%u", localtime(&curtime));
    wid = week[0] - '0';
    switch (wid) {
    case 1:
      swprintf(w_week, wchar_cnt, L" 星期一");
      break;
    case 2:
      swprintf(w_week, wchar_cnt, L" 星期二");
      break;
    case 3:
      swprintf(w_week, wchar_cnt, L" 星期三");
      break;
    case 4:
      swprintf(w_week, wchar_cnt, L" 星期四");
      break;
    case 5:
      swprintf(w_week, wchar_cnt, L" 星期五");
      break;
    case 6:
      swprintf(w_week, wchar_cnt, L" 星期六");
      break;
    case 7:
      swprintf(w_week, wchar_cnt, L" 星期日");
      break;
    default:
      LOG_ERROR("osd strftime week error\n");
      swprintf(w_week, wchar_cnt, L" 星期*");
      break;
    }
  } else if (strstr(fmt, OSD_FMT_WEEK1)) {
    strftime(week, sizeof(week), "%A", localtime(&curtime));
    swprintf(w_week, wchar_cnt, L" %s", week);
  }

  wchar_cnt = sizeof(w_ymd) / sizeof(wchar_t);
  if (strstr(fmt, OSD_FMT_CHR)) {
    if (strstr(fmt, OSD_FMT_YMD0))
      swprintf(w_ymd, wchar_cnt, L"%s-%s-%s", year, month, day);
    else if (strstr(fmt, OSD_FMT_YMD1))
      swprintf(w_ymd, wchar_cnt, L"%s-%s-%s", month, day, year);
    else if (strstr(fmt, OSD_FMT_YMD2))
      swprintf(w_ymd, wchar_cnt, L"%s-%s-%s", day, month, year);
    else if (strstr(fmt, OSD_FMT_YMD3))
      swprintf(w_ymd, wchar_cnt, L"%s/%s/%s", year, month, day);
    else if (strstr(fmt, OSD_FMT_YMD4))
      swprintf(w_ymd, wchar_cnt, L"%s/%s/%s", month, day, year);
    else if (strstr(fmt, OSD_FMT_YMD5))
      swprintf(w_ymd, wchar_cnt, L"%s/%s/%s", day, month, year);
  } else {
    if (strstr(fmt, OSD_FMT_YMD0))
      swprintf(w_ymd, wchar_cnt, L"%s年%s月%s日", year, month, day);
    else if (strstr(fmt, OSD_FMT_YMD1))
      swprintf(w_ymd, wchar_cnt, L"%s月%s日%s年", month, day, year);
    else if (strstr(fmt, OSD_FMT_YMD2))
      swprintf(w_ymd, wchar_cnt, L"%s日%s月%s年", day, month, year);
  }

  swprintf(result, r_size, L"%ls%ls %s", w_ymd, w_week, hms);
}

int OSDServer::UpdateTimeDate() {
  size_t wc_size = 128;
  wchar_t wc_str[128] = {0};
  osd_data_s *draw_data = &osds_db_data_[OSD_DB_REGION_ID_DATETIME];
  GenDataTime(draw_data->text.format, wc_str, wc_size);
  if (datetime_wstr_ != wc_str) {
    datetime_wstr_.clear();
    datetime_wstr_ = wc_str;
    size_t len = CalcWcharLen(datetime_wstr_);
    memset(draw_data->text.wch, 0, sizeof(draw_data->text.wch));
    wcsncpy(draw_data->text.wch, wc_str, len);
    draw_data->width = len * (draw_data->text.font_size >> 1);
    draw_data->height = draw_data->text.font_size;
    draw_data->width = MULTI_UPALIGNTO16(1, draw_data->width);
    draw_data->height = MULTI_UPALIGNTO16(1, draw_data->height);
    draw_data->size = draw_data->width * draw_data->height;
    draw_data->buffer = static_cast<uint8_t *>(malloc(draw_data->size));
    memset(draw_data->buffer, 0xFF, draw_data->size);
    osd_producer_->FillYuvMap(draw_data);
    region_data_[OSD_REGION_ID_TIMEDATE].enable = draw_data->enable;
    region_data_[OSD_REGION_ID_TIMEDATE].region_id = OSD_REGION_ID_TIMEDATE;
    region_data_[OSD_REGION_ID_TIMEDATE].pos_x = draw_data->origin_x;
    region_data_[OSD_REGION_ID_TIMEDATE].pos_y = draw_data->origin_y;
    region_data_[OSD_REGION_ID_TIMEDATE].width = draw_data->width;
    region_data_[OSD_REGION_ID_TIMEDATE].height = draw_data->height;
    region_data_[OSD_REGION_ID_TIMEDATE].buffer = draw_data->buffer;
    region_data_[OSD_REGION_ID_TIMEDATE].inverse =
        draw_data->text.color_inverse;
    easymedia::video_encoder_set_osd_region(
        encoder_flow_, &region_data_[OSD_REGION_ID_TIMEDATE]);
    free(draw_data->buffer);
  }
  return 0;
}

static void *ServerProcess(void *arg) {
  auto os = reinterpret_cast<OSDServer *>(arg);
  char thread_name[40];
  snprintf(thread_name, sizeof(thread_name), "OSDServer[%d]", os->GetWidth());
  prctl(PR_SET_NAME, thread_name);
  LOG_DEBUG("osd ServerProcess in\n");
  while (os->status() == kThreadRunning) {

    os->UpdateTimeDate();
    os->UpdateText();
    os->UpdateImage();
    os->UpdateMask();
    os->UpdateBlink();

    std::this_thread::sleep_for(std::chrono::milliseconds(os->GetDelayMs()));
  }
  LOG_DEBUG("osd ServerProcess out\n");
  return nullptr;
}

void OSDServer::start(void) {
  LOG_DEBUG("osd server start\n");
  service_thread_.reset(new Thread(ServerProcess, this));
  service_thread_->set_status(kThreadRunning);
}

void OSDServer::stop(void) {
  service_thread_->set_status(kThreadStopping);
  service_thread_->join();
  LOG_DEBUG("osd server stop\n");
}

ThreadStatus OSDServer::status(void) {
  if (service_thread_)
    return service_thread_->status();
  else
    return kThreadRunning;
}

} // namespace aiserver
} // namespace rockchip
