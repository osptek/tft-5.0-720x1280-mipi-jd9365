<p align="left"><img alt="OSPTEK" src="./images/logo.png" width="200" /></p>

<h1 align="center">OSPTEK 5.0″ TFT 720×1280 (JD9365 · MIPI)</h1>

<p align="center"><b>Touch TFT module · MIPI DSI · JD9365</b></p>

<p align="center"><a href="./README.md">简体中文</a> | English · <a href="../../README_EN.md">Family index</a></p>

<p align="center">
  <img alt="Size: 5.0 inch" src="https://img.shields.io/badge/Size-5.0%22-3498DB?style=flat-square" />
  <img alt="Resolution: 720x1280" src="https://img.shields.io/badge/Resolution-720%C3%971280-8E44AD?style=flat-square" />
  <img alt="Interface: MIPI" src="https://img.shields.io/badge/Interface-MIPI-27AE60?style=flat-square" />
  <img alt="Driver: JD9365" src="https://img.shields.io/badge/Driver-JD9365-E7352C?style=flat-square" />
</p>

## Contents

- [Overview](#overview)
- [Specifications](#specifications)
- [Sample projects](#sample-projects)
- [Repository layout](#repository-layout)
- [Resources](#resources)
- [Buy](#buy)
- [Support](#support)

---

## Overview

OSPTEK **5.0″ 720×1280 TFT** is a **MIPI DSI** color display module driven by **JD9365**, with capacitive touch (**CHSC5432**). Suited to portrait HMI, handheld terminals, and mid-size interactive panels.

Spec ID (repository name): `5.0-tft-720x1280-mipi-jd9365`

Current module version: **YDP500BT004-V1**. Electrical and mechanical details follow [`docs/YDP500BT004-V1.pdf`](./docs/YDP500BT004-V1.pdf).

## Specifications

| Item | Spec |
| ---- | ---- |
| Size | 5.0 inch |
| Type | TFT / IPS (color) |
| Resolution | 720×1280 |
| Interface | MIPI DSI |
| Driver IC | JD9365 |
| Touch driver | CHSC5432 |

> Full outline, FPC definition, power, and timing follow the product datasheet / driver IC datasheet.

## Sample projects

| Description | Path |
| ---- | ---- |
| ESP32-P4 · JD9365 MIPI + esp-lvgl-port / LVGL9 (touch CHSC5432) | [`examples/esp32p4-idf5_jd9365-mipi_esp-lvgl-port_lvgl9/`](./examples/esp32p4-idf5_jd9365-mipi_esp-lvgl-port_lvgl9/) |
| Raspberry Pi 5 · JD9365 720×1280 panel / DT overlay (display only) | [`examples/rpi5-panel-jd9365-720x1280/`](./examples/rpi5-panel-jd9365-720x1280/) |
| Raspberry Pi 5 · CHSC5432 touch / DT overlay (touch only) | [`examples/rpi5-touch-chsc5432/`](./examples/rpi5-touch-chsc5432/) |
| Raspberry Pi 5 · JD9365 display + CHSC5432 touch / DT overlay | [`examples/rpi5-panel-jd9365-chsc5432-720x1280/`](./examples/rpi5-panel-jd9365-chsc5432-720x1280/) |

## Repository layout

```text
5.0-tft-720x1280-mipi-jd9365/                                # repo root (nav: ../../README_EN.md)
└── versions/
    └── YDP500BT004-V1/                                # full materials for this part number
        ├── README.md
        ├── README_EN.md
        ├── images/
        ├── docs/
        └── examples/
```

## Resources

### Product files

| Resource | Link |
| ---- | ---- |
| Product datasheet (YDP500BT004-V1) | [`docs/YDP500BT004-V1.pdf`](./docs/YDP500BT004-V1.pdf) |
| 3D drawing (YDP500BT004-V1) | [`docs/YDP500BT004-V1.dwg`](./docs/YDP500BT004-V1.dwg) |
| Driver IC user guide (JD9365D) | [`docs/JD9365D_User_Guide_standard_V0.02_20171225.pdf`](./docs/JD9365D_User_Guide_standard_V0.02_20171225.pdf) |
| Init sequence (text) | [`docs/JD9365D_BOE4.96_720x1280_MIPI4L_init.txt`](./docs/JD9365D_BOE4.96_720x1280_MIPI4L_init.txt) |
| TFT050BT231 adapter schematic V1.0 | [`docs/SCH_TFT050BT231转接板V1.0_2026-08-03.pdf`](./docs/SCH_TFT050BT231%E8%BD%AC%E6%8E%A5%E6%9D%BFV1.0_2026-08-03.pdf) |
| Adapter pinout comparison | [`docs/adapter-pinout-compare.png`](./docs/adapter-pinout-compare.png) |

### Samples

- [ESP32-P4 JD9365 MIPI + LVGL9](./examples/esp32p4-idf5_jd9365-mipi_esp-lvgl-port_lvgl9/)
- [Raspberry Pi 5 JD9365 panel (display only)](./examples/rpi5-panel-jd9365-720x1280/)
- [Raspberry Pi 5 CHSC5432 touch (touch only)](./examples/rpi5-touch-chsc5432/)
- [Raspberry Pi 5 JD9365 display + CHSC5432 touch](./examples/rpi5-panel-jd9365-chsc5432-720x1280/)

## Buy

<p align="center">
  <a href="https://www.aliexpress.com/store/1105701619"><img alt="AliExpress Official Store" src="https://img.shields.io/badge/AliExpress-Official_Store-E62E04?style=for-the-badge&logo=aliexpress&logoColor=white" /></a>
  &nbsp;&nbsp;
  <a href="https://shop110742373.taobao.com/"><img alt="Taobao Official Store" src="https://img.shields.io/badge/Taobao-Official_Store-FF6A00?style=for-the-badge" /></a>
</p>

**International (AliExpress)**

- Store: [OSPTEK Official Store](https://www.aliexpress.com/store/1105701619)

**China (Taobao)**

- Store: [鱼鹰光电工厂店](https://shop110742373.taobao.com/)

## Support

- Technical support / product inquiry: <luyu@osptek.com>
- QQ group: **985881096**
- Website: <https://osptek.com/>
- Feel free to open an Issue in this repository if you have any questions

---

<p align="center"><sub>© 2026 OSPTEK · Materials in this repository are licensed under CC BY 4.0</sub></p>
