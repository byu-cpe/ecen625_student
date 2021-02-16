#include <stdint.h>
#include <vector>

std::vector<std::pair<uint64_t, uint8_t>> test_data = {
    {0x3041060800, 1},   {0x3041060800, 1},   {0x3041060800, 1},
    {0x4081020400, 1},   {0x4081020400, 1},   {0x2041020400, 1},
    {0x4081020600, 1},   {0x6081020c00, 1},   {0x20c1020800, 1},
    {0x4081020400, 1},   {0x6081020400, 1},   {0x2081020400, 1},
    {0xc081830600, 1},   {0x1061861800, 1},   {0x20c1060c00, 1},
    {0x4081020400, 1},   {0x20c1020400, 1},   {0x20c1020400, 1},
    {0x4081020400, 1},   {0x3041860800, 1},   {0x20c1860800, 1},
    {0x40c1020400, 1},   {0x20c1060800, 1},   {0x2041020c00, 1},
    {0x2041860c00, 1},   {0x4183070300, 1},   {0xe1c1860f00, 2},
    {0xe041060f00, 2},   {0xc3c1060e00, 2},   {0x8181e70c00, 2},
    {0x8181078000, 2},   {0x181078000, 2},    {0xc041078000, 2},
    {0xe060c7be00, 2},   {0xe040820c00, 2},   {0x3c7cb8000, 2},
    {0xe260870f00, 2},   {0x20e041060f00, 2}, {0x30e78f0a00, 2},
    {0xc040870c00, 2},   {0xf1618f9800, 2},   {0xe040831e00, 2},
    {0x20c1060200, 2},   {0x6043870a00, 2},   {0x61e0878c00, 2},
    {0x6141870200, 2},   {0xc08207c000, 2},   {0xe041040e00, 2},
    {0xe0c10f9f00, 2},   {0x70608f1a00, 2},   {0x20a041079e00, 2},
    {0xe061879e00, 2},   {0x1e0c3060f80, 2},  {0xe041020e00, 2},
    {0x10f0208f1800, 2}, {0xe0c307c000, 2},   {0x20e041040e00, 2},
    {0xc0c1860300, 2},   {0x60434b0e00, 2},   {0x70e18f9c00, 2},
    {0x41c18f8900, 2},   {0x105126cb1e00, 2}, {0x2070618e1f00, 2},
    {0xe06381111c, 3},   {0xe0c1c31c00, 3},   {0x1c3c1858f00, 3},
    {0xe143c19e00, 3},   {0xe0c1c19e00, 3},   {0x40c3810a00, 3},
    {0x60c3030a08, 3},   {0xe041811e10, 3},   {0xe0c3010e00, 3},
    {0x6043811c00, 3},   {0x1c083808e00, 3},  {0xe081811e00, 3},
    {0xe041811e00, 3},   {0xc081010400, 3},   {0xf0c0cf9f00, 3},
    {0xe1c3c19e00, 3},   {0x6080810c00, 3},   {0x1f063809e00, 3},
    {0x10142878100, 4},  {0x91a3c90400, 4},   {0x11438a0400, 4},
    {0x8147878200, 4},   {0x21e48f0600, 4},   {0x9143c70408, 4},
    {0x8363830600, 4},   {0x11238a0000, 4},   {0x8163830608, 4},
    {0x3147c70400, 4},   {0x8142830400, 4},   {0x2347830304, 4},
    {0x1162870400, 4},   {0x1326cf0200, 4},   {0x9648e0800, 4},
    {0x10a38b0400, 4},   {0x1167860800, 4},   {0x41c3830408, 4},
    {0x10364cf8300, 4},  {0x41a7860800, 4},   {0x1167820c10, 4},
    {0xa1448f8200, 4},   {0xc2878200, 4},     {0x1a7c70408, 4},
    {0x51c68f8600, 4},   {0x11e7cd0200, 4},   {0x7103810600, 5},
    {0xf303c68700, 5},   {0xe303c48f00, 5},   {0x23e6070608, 5},
    {0xf3020800, 5},     {0xf183899e00, 5},   {0x1e3020c00, 5},
    {0x31e30b1c00, 5},   {0x1e30f0c00, 5},    {0x6183030e00, 5},
    {0x30c1021800, 5},   {0x6081810c00, 5},   {0xc2020c00, 5},
    {0x3183811e00, 5},   {0xc103808e00, 5},   {0x71e3851e00, 5},
    {0x7181830c00, 5},   {0x1071038d0e00, 5}, {0x6083c78c00, 6},
    {0x408103870000, 6}, {0x106103078700, 6}, {0x41c3870e00, 6},
    {0xc107cf8e00, 6},   {0x6083870400, 6},   {0x2081060400, 6},
    {0x18387c6c700, 6},  {0x20c107c78400, 6}, {0x3c0820408, 7},
    {0x41c0820410, 7},   {0x23e081060c, 7},   {0x83c0830608, 7},
    {0xe0820c10, 7},     {0x1c0810200, 7},    {0xc1c3830408, 7},
    {0x1e6830c10, 7},    {0x41c0830408, 7},   {0x3e4c30c10, 7},
    {0x71c1820820, 7},   {0x3e4c10204, 7},    {0xe1c1e71810, 7},
    {0x3c1830608, 7},    {0x3c4810608, 7},    {0x41e0830c10, 7},
    {0xe160820820, 7},   {0xe043860408, 7},   {0x23c0830408, 7},
    {0x21e0820810, 7},   {0x1c3c0820408, 7},  {0x3c0810608, 7},
    {0x3e7c10608, 7},    {0x3160820c10, 7},   {0x81c1830600, 7},
    {0x43c0830408, 7},   {0xc3c0830408, 7},   {0x6123850e00, 8},
    {0x41c1060c00, 8},   {0x3c7810204, 9},    {0x41c3810204, 9},
    {0x6166c70600, 9},   {0xc103810204, 9},   {0xe3664d8e00, 0},
    {0xe3e64d8e00, 0},   {0x6142850c00, 0},   {0x30a2c91c00, 0},
    {0xe3e7488f00, 0},   {0x61a24c8e00, 0},   {0x31e64f0c00, 0},
    {0x60c2850e00, 0},   {0x30f24d1c00, 0},   {0x61e2c58e00, 0},
    {0xe3e6cf9e00, 0},   {0x60c2850c00, 0},   {0x71a2499c00, 0},
    {0x71a24c9a00, 0},   {0x31e6cf8c00, 0}};