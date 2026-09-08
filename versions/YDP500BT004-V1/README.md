<p align="left"><img alt="OSPTEK" src="./images/logo.png" width="200" /></p>

<h1 align="center">OSPTEK 5.0″ TFT 720×1280（JD9365 · MIPI）</h1>

<p align="center"><b>触摸 TFT 模组 · MIPI DSI · JD9365</b></p>

<p align="center"><a href="./README_EN.md">English</a> | 简体中文 · <a href="../../README.md">规格族索引</a></p>

<p align="center">
  <img alt="Size: 5.0 inch" src="https://img.shields.io/badge/Size-5.0%22-3498DB?style=flat-square" />
  <img alt="Resolution: 720x1280" src="https://img.shields.io/badge/Resolution-720%C3%971280-8E44AD?style=flat-square" />
  <img alt="Interface: MIPI" src="https://img.shields.io/badge/Interface-MIPI-27AE60?style=flat-square" />
  <img alt="Driver: JD9365" src="https://img.shields.io/badge/Driver-JD9365-E7352C?style=flat-square" />
</p>

## 目录

- [产品简介](#产品简介)
- [规格参数](#规格参数)
- [示例工程](#示例工程)
- [仓库结构](#仓库结构)
- [相关资料](#相关资料)
- [购买链接](#购买链接)
- [技术支持](#技术支持)

---

## 产品简介

OSPTEK **5.0 寸 720×1280 TFT** 是一款 **MIPI DSI** 接口彩色显示模组，显示驱动为 **JD9365**，触摸驱动为 **CHSC5432**。适合竖屏 HMI、手持终端与中尺寸交互面板等场景。

规格标识（仓库名）：`5.0-tft-720x1280-mipi-jd9365`

当前模组版本：**YDP500BT004-V1**。电气与外形细节以 [`docs/YDP500BT004-V1.pdf`](./docs/YDP500BT004-V1.pdf) 为准。

## 规格参数

| 项目 | 规格 |
| ---- | ---- |
| 尺寸 | 5.0 英寸 |
| 类型 | TFT / IPS（彩色） |
| 分辨率 | 720×1280 |
| 接口 | MIPI DSI |
| 驱动 IC | JD9365 |
| 触摸驱动 | CHSC5432 |

> 完整外形尺寸、FPC 定义、供电与时序以产品规格书 / 驱动手册为准。

## 示例工程

| 说明 | 路径 |
| ---- | ---- |
| ESP32-P4 · JD9365 MIPI + esp-lvgl-port / LVGL9（触摸 CHSC5432） | [`examples/esp32p4-idf5_jd9365-mipi_esp-lvgl-port_lvgl9/`](./examples/esp32p4-idf5_jd9365-mipi_esp-lvgl-port_lvgl9/) |
| Raspberry Pi 5 · JD9365 720×1280 面板驱动 / DT overlay（仅显示） | [`examples/rpi5-panel-jd9365-720x1280/`](./examples/rpi5-panel-jd9365-720x1280/) |
| Raspberry Pi 5 · CHSC5432 触摸驱动 / DT overlay（仅触摸） | [`examples/rpi5-touch-chsc5432/`](./examples/rpi5-touch-chsc5432/) |
| Raspberry Pi 5 · JD9365 显示 + CHSC5432 触摸 / DT overlay | [`examples/rpi5-panel-jd9365-chsc5432-720x1280/`](./examples/rpi5-panel-jd9365-chsc5432-720x1280/) |

## 仓库结构

```text
5.0-tft-720x1280-mipi-jd9365/                                # 仓库根（导航见 ../../README.md）
└── versions/
    └── YDP500BT004-V1/                                # 本料号完整资料
        ├── README.md
        ├── README_EN.md
        ├── images/
        ├── docs/
        └── examples/
```

## 相关资料

### 本产品资料

| 资料 | 链接 |
| ---- | ---- |
| 产品规格书（YDP500BT004-V1） | [`docs/YDP500BT004-V1.pdf`](./docs/YDP500BT004-V1.pdf) |
| 3D 图纸（YDP500BT004-V1） | [`docs/YDP500BT004-V1.dwg`](./docs/YDP500BT004-V1.dwg) |
| 驱动 IC 用户手册（JD9365D） | [`docs/JD9365D_User_Guide_standard_V0.02_20171225.pdf`](./docs/JD9365D_User_Guide_standard_V0.02_20171225.pdf) |
| 初始化序列（文本） | [`docs/JD9365D_BOE4.96_720x1280_MIPI4L_init.txt`](./docs/JD9365D_BOE4.96_720x1280_MIPI4L_init.txt) |
| TFT050BT231 转接板原理图 V1.0 | [`docs/SCH_TFT050BT231转接板V1.0_2026-08-03.pdf`](./docs/SCH_TFT050BT231转接板V1.0_2026-08-03.pdf) |
| 转接板引脚对照图 | [`docs/adapter-pinout-compare.png`](./docs/adapter-pinout-compare.png) |

### 示例工程

- [ESP32-P4 JD9365 MIPI + LVGL9](./examples/esp32p4-idf5_jd9365-mipi_esp-lvgl-port_lvgl9/)
- [Raspberry Pi 5 JD9365 面板（仅显示）](./examples/rpi5-panel-jd9365-720x1280/)
- [Raspberry Pi 5 CHSC5432 触摸（仅触摸）](./examples/rpi5-touch-chsc5432/)
- [Raspberry Pi 5 JD9365 显示 + CHSC5432 触摸](./examples/rpi5-panel-jd9365-chsc5432-720x1280/)

## 购买链接

<p align="center">
  <a href="https://shop110742373.taobao.com/"><img alt="淘宝官方店铺" src="https://img.shields.io/badge/淘宝-官方店铺-FF6A00?style=for-the-badge" /></a>
  &nbsp;&nbsp;
  <a href="https://www.aliexpress.com/store/1105701619"><img alt="速卖通官方店铺" src="https://img.shields.io/badge/速卖通-官方店铺-FF6A00?style=for-the-badge" /></a>
</p>

**国内（淘宝）**

- 店铺：[鱼鹰光电工厂店](https://shop110742373.taobao.com/)

**海外（AliExpress）**

- 店铺：[OSPTEK Official Store](https://www.aliexpress.com/store/1105701619)

## 技术支持

- 技术支持 / 产品咨询：<luyu@osptek.com>
- QQ 技术交流群：**985881096**
- 公司官网：<https://osptek.com/>
- 有任何问题，都可以在本仓库 Issues 中提问

---

<p align="center"><sub>© 2026 OSPTEK 鱼鹰光电 · 本仓库资料采用 CC BY 4.0 许可</sub></p>
