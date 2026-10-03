# BL0906 / BL0910 ESPHome 外部组件

为 BL0906 和 BL0910 电能计量芯片提供 ESPHome 外部组件支持，包含电压/电流/功率测量、电量统计、校准持久化等功能。

<p align="center">
  <img src="https://raw.githubusercontent.com/carrot8848/ESPHome-YAML/main/docs/modules-overview.png" width="420" alt="电量计量模块产品全图">
</p>

**注意：本组件是本店售卖的电量计量模块的专用 external component，需要特殊的硬件配置以及校准过程。**

## 购买成品

不想自己动手焊接与校准？可直接购买已烧录固件并完成出厂校准的成品模块：

| 产品 | 购买链接 |
|------|---------|
| 6 通道电量计量模块 | [淘宝购买](https://item.taobao.com/item.htm?id=793797215362) |
| 10 通道电量计量模块 | [淘宝购买](https://item.taobao.com/item.htm?id=999413514343) |
| 16 通道电量计量模块 | [淘宝购买](https://item.taobao.com/item.htm?id=999413514343) |
| 3×6 通道电量计量模块 | [淘宝购买](https://item.taobao.com/item.htm?id=971222114086) |

## 支持的芯片

| 芯片 | 通道数 | 说明 |
|------|-------|------|
| BL0906 | 6 | 6 通道电能计量 |
| BL0910 | 10 | 10 通道电能计量 |

## 支持的平台

预编译库已为以下 ESP-IDF 目标构建（ESP-IDF v5.5.5）：

| 目标 | 状态 |
|------|------|
| `esp32c3` | ✅ 已支持 |

## 安装

在 ESPHome 配置中添加 `external_components`：

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/carrot8848/bl0906_carrot8848
      ref: main
    components: [bl0906_carrot8848]
```

模块参考 yaml 配置请参考另外的仓库：[ESPHome-YAML](https://github.com/carrot8848/ESPHome-YAML)

## 配置项说明

### `bl0906_carrot8848` 组件

| 参数 | 类型 | 必填 | 默认值 | 说明 |
|------|------|------|--------|------|
| `chip_model` | string | 是 | - | `bl0906` 或 `bl0910` |
| `instance_id` | int | 是 | - | 芯片实例 ID |
| `spi_id` | id | 是 | - | SPI 总线 ID |
| `cs_pin` | pin | 是 | - | SPI CS 引脚 |
| `freq_adapt` | string | 否 | `auto` | 频率自适应模式 |
| `transformer_ratio` | float | 否 | `2000.0` | 互感器变比 |
| `voltage_sampling_mode` | string | 否 | `transformer` | 电压采样模式：`transformer` 或 `resistor_divider` |
| `eeprom_type` | string | 否 | `24c02` | EEPROM 型号（24c02/24c04/24c08/24c16） |
| `i2c_id` | id | 是 | - | I2C 总线 ID（EEPROM 校准存储） |
| `address` | int | 否 | `0x50` | EEPROM I2C 地址 |

## 版本兼容性

- **ESPHome**: 2026.8.0 及以上（实测 2026.8.2 / 2026.9.1 编译通过）
- **ESP-IDF**: v5.x（建议使用 ESPHome 捆绑版本）
- **Arduino**: 不支持（仅支持 ESP-IDF 框架）

## 问题反馈

- 提交 GitHub Issue 报告问题
- 请附上 ESPHome 版本、ESP-IDF 版本、目标芯片、配置文件（移除敏感信息）

## 许可证

本组件（头文件、Python 脚本与预编译静态库）以 [PolyForm Noncommercial 1.0.0](LICENSE) 协议发布：

- **允许**：个人使用、学习、研究，以及在个人非商业 ESPHome 项目中集成（包括刷写自己购买设备的固件）
- **禁止**：任何商业用途，包括将本组件集成到对外销售的产品中；商业授权请联系作者
- 本仓库不包含源代码，以预编译库形式分发

以上为中文摘要，具体条款以 [LICENSE](LICENSE)（英文原文）为准。

固件基于 [ESPHome](https://github.com/esphome/esphome) 构建，其 C++ 运行时以 GPLv3 发布，源码可从 ESPHome 官方仓库获取。

Copyright (c) 2026 carrot8848. All rights reserved.
