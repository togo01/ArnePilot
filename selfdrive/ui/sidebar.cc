#include <stdio.h>
#include <string.h>
#include <math.h>
#include "ui.hpp"

static void ui_draw_sidebar_background(UIState *s) {
  int sbr_x = !s->scene.uilayout_sidebarcollapsed ? 0 : -(sbr_w) + bdr_s * 2;

  nvgBeginPath(s->vg);
  nvgRect(s->vg, sbr_x, 0, sbr_w, vwp_h);
  nvgFillColor(s->vg, COLOR_BLACK_ALPHA(85));
  nvgFill(s->vg);
}

static void ui_draw_sidebar_settings_button(UIState *s) {
  bool settingsActive = s->active_app == cereal_UiLayoutState_App_settings;
  const int settings_btn_xr = !s->scene.uilayout_sidebarcollapsed ? settings_btn_x : -(sbr_w);

  ui_draw_image(s->vg, settings_btn_xr, settings_btn_y, settings_btn_w, settings_btn_h, s->img_button_settings, settingsActive ? 1.0f : 0.65f);
}

static void ui_draw_sidebar_home_button(UIState *s) {
  bool homeActive = s->active_app == cereal_UiLayoutState_App_home;
  const int home_btn_xr = !s->scene.uilayout_sidebarcollapsed ? home_btn_x : -(sbr_w);

  ui_draw_image(s->vg, home_btn_xr, home_btn_y, home_btn_w, home_btn_h, s->img_button_home, homeActive ? 1.0f : 0.65f);
}

static void ui_draw_sidebar_network_strength(UIState *s) {
  const int network_img_h = 27;
  const int network_img_w = 176;
  const int network_img_x = !s->scene.uilayout_sidebarcollapsed ? 58 : -(sbr_w);
  const int network_img_y = 196;
  const int network_img = s->scene.networkType == cereal_ThermalData_NetworkType_none ?
                          s->img_network[0] : s->img_network[s->scene.networkStrength + 1];

  ui_draw_image(s->vg, network_img_x, network_img_y, network_img_w, network_img_h, network_img, 1.0f);
}
//thank you @eFini
static void ui_draw_sidebar_ip_addr(UIState *s) {
  const int network_ip_w = 176;
  const int network_ip_x = !s->scene.uilayout_sidebarcollapsed ? 54 : -(sbr_w);
  const int network_ip_y = 255;

  char network_ip_str[20];
  snprintf(network_ip_str, sizeof(network_ip_str), "%s", s->scene.ipAddr);
  nvgFillColor(s->vg, COLOR_WHITE);
  nvgFontSize(s->vg, 32);
  nvgFontFace(s->vg, "sans-regular");
  nvgTextAlign(s->vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  nvgTextBox(s->vg, network_ip_x, network_ip_y, network_ip_w, network_ip_str, NULL);
}

static void ui_draw_sidebar_battery_text(UIState *s) {
  const int battery_img_h = 36;
  const int battery_img_w = 76;
  const int battery_img_x = !s->scene.uilayout_sidebarcollapsed ? 150 : -(sbr_w);
  const int battery_img_y = 305;

  char battery_str[7];
  snprintf(battery_str, sizeof(battery_str), "%d%%%s", s->scene.batteryPercent, strcmp(s->scene.batteryStatus, "Charging") == 0 ? "+" : "-");
  nvgFillColor(s->vg, COLOR_WHITE);
  nvgFontSize(s->vg, 40);
  nvgFontFace(s->vg, "sans-regular");
  nvgTextAlign(s->vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  nvgTextBox(s->vg, battery_img_x, battery_img_y, battery_img_w, battery_str, NULL);
}

static void ui_draw_sidebar_battery_icon(UIState *s) {
  const int battery_img_h = 36;
  const int battery_img_w = 76;
  const int battery_img_x = !s->scene.uilayout_sidebarcollapsed ? 160 : -(sbr_w);
  const int battery_img_y = 255;

  int battery_img = strcmp(s->scene.batteryStatus, "Charging") == 0 ?
    s->img_battery_charging : s->img_battery;

  nvgBeginPath(s->vg);
  nvgRect(s->vg, battery_img_x + 6, battery_img_y + 5,
    ((battery_img_w - 19) * (s->scene.batteryPercent * 0.01)), battery_img_h - 11);
  nvgFillColor(s->vg, COLOR_WHITE);
  nvgFill(s->vg);

  ui_draw_image(s->vg, battery_img_x, battery_img_y, battery_img_w, battery_img_h, battery_img, 1.0f);
}



static void ui_draw_sidebar_network_type(UIState *s) {
  const int network_x = !s->scene.uilayout_sidebarcollapsed ? 50 : -(sbr_w);
  const int network_y = 300;
  const int network_w = 100;
  const int network_h = 100;
  const char *network_types[6] = {"--", "WiFi", "2G", "3G", "4G", "5G"};
  char network_type_str[32];

  if (s->scene.networkType <= 5) {
    snprintf(network_type_str, sizeof(network_type_str), "%s", network_types[s->scene.networkType]);
  }

  nvgFillColor(s->vg, COLOR_WHITE);
  nvgFontSize(s->vg, 48);
  nvgFontFaceId(s->vg, s->font_sans_regular);
  nvgTextAlign(s->vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
  nvgTextBox(s->vg, network_x, network_y, network_w, network_type_str, NULL);
}

static void ui_draw_sidebar_metric(UIState *s, const char* label_str, const char* value_str, const int severity, const int y_offset, const char* message_str) {
  const int metric_x = !s->scene.uilayout_sidebarcollapsed ? 30 : -(sbr_w);
  const int metric_y = 338 + y_offset;
  const int metric_w = 240;
  const int metric_h = message_str ? strchr(message_str, '\n') ? 124 : 100 : 148;

  NVGcolor status_color;

  if (severity == 0) {
    status_color = COLOR_WHITE;
  } else if (severity == 1) {
    status_color = COLOR_YELLOW;
  } else if (severity > 1) {
    status_color = COLOR_RED;
  }

  nvgBeginPath(s->vg);
  nvgRoundedRect(s->vg, metric_x, metric_y, metric_w, metric_h, 20);
  nvgStrokeColor(s->vg, severity > 0 ? COLOR_WHITE : COLOR_WHITE_ALPHA(85));
  nvgStrokeWidth(s->vg, 2);
  nvgStroke(s->vg);

  nvgBeginPath(s->vg);
  nvgRoundedRectVarying(s->vg, metric_x + 6, metric_y + 6, 18, metric_h - 12, 25, 0, 0, 25);
  nvgFillColor(s->vg, status_color);
  nvgFill(s->vg);

  if (!message_str) {
    nvgFillColor(s->vg, COLOR_WHITE);
    nvgFontSize(s->vg, 78);
    nvgFontFaceId(s->vg, s->font_sans_bold);
    nvgTextAlign(s->vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgTextBox(s->vg, metric_x + 50, metric_y + 50, metric_w - 60, value_str, NULL);

    nvgFillColor(s->vg, COLOR_WHITE);
    nvgFontSize(s->vg, 46);
    nvgFontFaceId(s->vg, s->font_sans_regular);
    nvgTextAlign(s->vg, NVG_ALIGN_LEFT | NVG_ALIGN_MIDDLE);
    nvgTextBox(s->vg, metric_x + 50, metric_y + 50 + 66, metric_w - 60, label_str, NULL);
  } else {
    nvgFillColor(s->vg, COLOR_WHITE);
    nvgFontSize(s->vg, 46);
    nvgFontFaceId(s->vg, s->font_sans_bold);
    nvgTextAlign(s->vg, NVG_ALIGN_CENTER | NVG_ALIGN_MIDDLE);
    nvgTextBox(s->vg, metric_x + 35, metric_y + (strchr(message_str, '\n') ? 40 : 50), metric_w - 50, message_str, NULL);
  }
}

static void ui_draw_sidebar_temp_metric(UIState *s) {
  int temp_severity;
  char temp_label_str[32];
  char temp_value_str[32];
  char temp_value_unit[32];
  const int temp_y_offset = 0;

  if (s->scene.thermalStatus == cereal_ThermalData_ThermalStatus_green) {
    temp_severity = 0;
  } else if (s->scene.thermalStatus == cereal_ThermalData_ThermalStatus_yellow) {
    temp_severity = 1;
  } else if (s->scene.thermalStatus == cereal_ThermalData_ThermalStatus_red) {
    temp_severity = 2;
  } else if (s->scene.thermalStatus == cereal_ThermalData_ThermalStatus_danger) {
    temp_severity = 3;
  }

  snprintf(temp_value_str, sizeof(temp_value_str), "%d", s->scene.paTemp);
  snprintf(temp_value_unit, sizeof(temp_value_unit), "%s", "°C");
  snprintf(temp_label_str, sizeof(temp_label_str), "%s", "TEMP");
  strcat(temp_value_str, temp_value_unit);

  ui_draw_sidebar_metric(s, temp_label_str, temp_value_str, temp_severity, temp_y_offset, NULL);
}

static void ui_draw_sidebar_panda_metric(UIState *s) {
  int panda_severity;
  char panda_message_str[32];
  const int panda_y_offset = 32 + 148;

  if (s->scene.hwType == cereal_HealthData_HwType_unknown) {
    panda_severity = 2;
    snprintf(panda_message_str, sizeof(panda_message_str), "%s", "PANDA\nN/A");
  } else if (s->scene.hwType == cereal_HealthData_HwType_whitePanda) {
    panda_severity = 0;
    snprintf(panda_message_str, sizeof(panda_message_str), "%s", "PANDA\nACTIVE");
  } else if (
      (s->scene.hwType == cereal_HealthData_HwType_greyPanda) ||
      (s->scene.hwType == cereal_HealthData_HwType_blackPanda) ||
      (s->scene.hwType == cereal_HealthData_HwType_uno)) {
      if (s->scene.satelliteCount == -1) {
        panda_severity = 0;
      } else {
        if (s->scene.satelliteCount < 6) {
          panda_severity = 1;
        } else if (s->scene.satelliteCount >= 6) {
          panda_severity = 0;
        }
      }
      snprintf(panda_message_str, sizeof(panda_message_str), "%s %d", "PANDA\nGPS:", s->scene.satelliteCount);
  }

  ui_draw_sidebar_metric(s, NULL, NULL, panda_severity, panda_y_offset, panda_message_str);
}

static void ui_draw_sidebar_connectivity(UIState *s) {
  if (s->scene.athenaStatus == NET_DISCONNECTED) {
    ui_draw_sidebar_metric(s, NULL, NULL, 1, 180+158, "ATHENA\nOFFLINE");
  } else if (s->scene.athenaStatus == NET_CONNECTED) {
    ui_draw_sidebar_metric(s, NULL, NULL, 0, 180+158, "ATHENA\nONLINE");
  } else {
    ui_draw_sidebar_metric(s, NULL, NULL, 2, 180+158, "ATHENA\nERROR");
  }
}

void ui_draw_sidebar(UIState *s) {
  ui_draw_sidebar_background(s);
  ui_draw_sidebar_settings_button(s);
  ui_draw_sidebar_home_button(s);
  ui_draw_sidebar_network_strength(s);
  ui_draw_sidebar_ip_addr(s);
  ui_draw_sidebar_battery_text(s);
  ui_draw_sidebar_network_type(s);
  ui_draw_sidebar_temp_metric(s);
  ui_draw_sidebar_panda_metric(s);
  ui_draw_sidebar_connectivity(s);
}
