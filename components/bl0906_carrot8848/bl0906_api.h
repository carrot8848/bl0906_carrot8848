#pragma once

// .a 以 -fvisibility=hidden 编译，BL0906_API 标注对外导出符号
#define BL0906_API __attribute__((visibility("default")))
