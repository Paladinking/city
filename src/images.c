#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <images.h>

SDL_Texture* load_image(SDL_Renderer* renderer, uint32_t x, uint32_t y) {
    SDL_IOStream* stream = SDL_IOFromConstMem(tile_0_0, sizeof(tile_0_0));
    switch (x + 16 * y) {
    case 0:
        stream = SDL_IOFromConstMem(tile_0_0, sizeof(tile_0_0));
        break;
    case 1:
        stream = SDL_IOFromConstMem(tile_1_0, sizeof(tile_1_0));
        break;
    case 2:
        stream = SDL_IOFromConstMem(tile_2_0, sizeof(tile_2_0));
        break;
    case 3:
        stream = SDL_IOFromConstMem(tile_3_0, sizeof(tile_3_0));
        break;
    case 4:
        stream = SDL_IOFromConstMem(tile_4_0, sizeof(tile_4_0));
        break;
    case 5:
        stream = SDL_IOFromConstMem(tile_5_0, sizeof(tile_5_0));
        break;
    case 6:
        stream = SDL_IOFromConstMem(tile_6_0, sizeof(tile_6_0));
        break;
    case 7:
        stream = SDL_IOFromConstMem(tile_7_0, sizeof(tile_7_0));
        break;
    case 8:
        stream = SDL_IOFromConstMem(tile_8_0, sizeof(tile_8_0));
        break;
    case 9:
        stream = SDL_IOFromConstMem(tile_9_0, sizeof(tile_9_0));
        break;
    case 10:
        stream = SDL_IOFromConstMem(tile_10_0, sizeof(tile_10_0));
        break;
    case 11:
        stream = SDL_IOFromConstMem(tile_11_0, sizeof(tile_11_0));
        break;
    case 12:
        stream = SDL_IOFromConstMem(tile_12_0, sizeof(tile_12_0));
        break;
    case 13:
        stream = SDL_IOFromConstMem(tile_13_0, sizeof(tile_13_0));
        break;
    case 14:
        stream = SDL_IOFromConstMem(tile_14_0, sizeof(tile_14_0));
        break;
    case 15:
        stream = SDL_IOFromConstMem(tile_15_0, sizeof(tile_15_0));
        break;
    case 16:
        stream = SDL_IOFromConstMem(tile_0_1, sizeof(tile_0_1));
        break;
    case 17:
        stream = SDL_IOFromConstMem(tile_1_1, sizeof(tile_1_1));
        break;
    case 18:
        stream = SDL_IOFromConstMem(tile_2_1, sizeof(tile_2_1));
        break;
    case 19:
        stream = SDL_IOFromConstMem(tile_3_1, sizeof(tile_3_1));
        break;
    case 20:
        stream = SDL_IOFromConstMem(tile_4_1, sizeof(tile_4_1));
        break;
    case 21:
        stream = SDL_IOFromConstMem(tile_5_1, sizeof(tile_5_1));
        break;
    case 22:
        stream = SDL_IOFromConstMem(tile_6_1, sizeof(tile_6_1));
        break;
    case 23:
        stream = SDL_IOFromConstMem(tile_7_1, sizeof(tile_7_1));
        break;
    case 24:
        stream = SDL_IOFromConstMem(tile_8_1, sizeof(tile_8_1));
        break;
    case 25:
        stream = SDL_IOFromConstMem(tile_9_1, sizeof(tile_9_1));
        break;
    case 26:
        stream = SDL_IOFromConstMem(tile_10_1, sizeof(tile_10_1));
        break;
    case 27:
        stream = SDL_IOFromConstMem(tile_11_1, sizeof(tile_11_1));
        break;
    case 28:
        stream = SDL_IOFromConstMem(tile_12_1, sizeof(tile_12_1));
        break;
    case 29:
        stream = SDL_IOFromConstMem(tile_13_1, sizeof(tile_13_1));
        break;
    case 30:
        stream = SDL_IOFromConstMem(tile_14_1, sizeof(tile_14_1));
        break;
    case 31:
        stream = SDL_IOFromConstMem(tile_15_1, sizeof(tile_15_1));
        break;
    case 32:
        stream = SDL_IOFromConstMem(tile_0_2, sizeof(tile_0_2));
        break;
    case 33:
        stream = SDL_IOFromConstMem(tile_1_2, sizeof(tile_1_2));
        break;
    case 34:
        stream = SDL_IOFromConstMem(tile_2_2, sizeof(tile_2_2));
        break;
    case 35:
        stream = SDL_IOFromConstMem(tile_3_2, sizeof(tile_3_2));
        break;
    case 36:
        stream = SDL_IOFromConstMem(tile_4_2, sizeof(tile_4_2));
        break;
    case 37:
        stream = SDL_IOFromConstMem(tile_5_2, sizeof(tile_5_2));
        break;
    case 38:
        stream = SDL_IOFromConstMem(tile_6_2, sizeof(tile_6_2));
        break;
    case 39:
        stream = SDL_IOFromConstMem(tile_7_2, sizeof(tile_7_2));
        break;
    case 40:
        stream = SDL_IOFromConstMem(tile_8_2, sizeof(tile_8_2));
        break;
    case 41:
        stream = SDL_IOFromConstMem(tile_9_2, sizeof(tile_9_2));
        break;
    case 42:
        stream = SDL_IOFromConstMem(tile_10_2, sizeof(tile_10_2));
        break;
    case 43:
        stream = SDL_IOFromConstMem(tile_11_2, sizeof(tile_11_2));
        break;
    case 44:
        stream = SDL_IOFromConstMem(tile_12_2, sizeof(tile_12_2));
        break;
    case 45:
        stream = SDL_IOFromConstMem(tile_13_2, sizeof(tile_13_2));
        break;
    case 46:
        stream = SDL_IOFromConstMem(tile_14_2, sizeof(tile_14_2));
        break;
    case 47:
        stream = SDL_IOFromConstMem(tile_15_2, sizeof(tile_15_2));
        break;
    case 48:
        stream = SDL_IOFromConstMem(tile_0_3, sizeof(tile_0_3));
        break;
    case 49:
        stream = SDL_IOFromConstMem(tile_1_3, sizeof(tile_1_3));
        break;
    case 50:
        stream = SDL_IOFromConstMem(tile_2_3, sizeof(tile_2_3));
        break;
    case 51:
        stream = SDL_IOFromConstMem(tile_3_3, sizeof(tile_3_3));
        break;
    case 52:
        stream = SDL_IOFromConstMem(tile_4_3, sizeof(tile_4_3));
        break;
    case 53:
        stream = SDL_IOFromConstMem(tile_5_3, sizeof(tile_5_3));
        break;
    case 54:
        stream = SDL_IOFromConstMem(tile_6_3, sizeof(tile_6_3));
        break;
    case 55:
        stream = SDL_IOFromConstMem(tile_7_3, sizeof(tile_7_3));
        break;
    case 56:
        stream = SDL_IOFromConstMem(tile_8_3, sizeof(tile_8_3));
        break;
    case 57:
        stream = SDL_IOFromConstMem(tile_9_3, sizeof(tile_9_3));
        break;
    case 58:
        stream = SDL_IOFromConstMem(tile_10_3, sizeof(tile_10_3));
        break;
    case 59:
        stream = SDL_IOFromConstMem(tile_11_3, sizeof(tile_11_3));
        break;
    case 60:
        stream = SDL_IOFromConstMem(tile_12_3, sizeof(tile_12_3));
        break;
    case 61:
        stream = SDL_IOFromConstMem(tile_13_3, sizeof(tile_13_3));
        break;
    case 62:
        stream = SDL_IOFromConstMem(tile_14_3, sizeof(tile_14_3));
        break;
    case 63:
        stream = SDL_IOFromConstMem(tile_15_3, sizeof(tile_15_3));
        break;
    case 64:
        stream = SDL_IOFromConstMem(tile_0_4, sizeof(tile_0_4));
        break;
    case 65:
        stream = SDL_IOFromConstMem(tile_1_4, sizeof(tile_1_4));
        break;
    case 66:
        stream = SDL_IOFromConstMem(tile_2_4, sizeof(tile_2_4));
        break;
    case 67:
        stream = SDL_IOFromConstMem(tile_3_4, sizeof(tile_3_4));
        break;
    case 68:
        stream = SDL_IOFromConstMem(tile_4_4, sizeof(tile_4_4));
        break;
    case 69:
        stream = SDL_IOFromConstMem(tile_5_4, sizeof(tile_5_4));
        break;
    case 70:
        stream = SDL_IOFromConstMem(tile_6_4, sizeof(tile_6_4));
        break;
    case 71:
        stream = SDL_IOFromConstMem(tile_7_4, sizeof(tile_7_4));
        break;
    case 72:
        stream = SDL_IOFromConstMem(tile_8_4, sizeof(tile_8_4));
        break;
    case 73:
        stream = SDL_IOFromConstMem(tile_9_4, sizeof(tile_9_4));
        break;
    case 74:
        stream = SDL_IOFromConstMem(tile_10_4, sizeof(tile_10_4));
        break;
    case 75:
        stream = SDL_IOFromConstMem(tile_11_4, sizeof(tile_11_4));
        break;
    case 76:
        stream = SDL_IOFromConstMem(tile_12_4, sizeof(tile_12_4));
        break;
    case 77:
        stream = SDL_IOFromConstMem(tile_13_4, sizeof(tile_13_4));
        break;
    case 78:
        stream = SDL_IOFromConstMem(tile_14_4, sizeof(tile_14_4));
        break;
    case 79:
        stream = SDL_IOFromConstMem(tile_15_4, sizeof(tile_15_4));
        break;
    case 80:
        stream = SDL_IOFromConstMem(tile_0_5, sizeof(tile_0_5));
        break;
    case 81:
        stream = SDL_IOFromConstMem(tile_1_5, sizeof(tile_1_5));
        break;
    case 82:
        stream = SDL_IOFromConstMem(tile_2_5, sizeof(tile_2_5));
        break;
    case 83:
        stream = SDL_IOFromConstMem(tile_3_5, sizeof(tile_3_5));
        break;
    case 84:
        stream = SDL_IOFromConstMem(tile_4_5, sizeof(tile_4_5));
        break;
    case 85:
        stream = SDL_IOFromConstMem(tile_5_5, sizeof(tile_5_5));
        break;
    case 86:
        stream = SDL_IOFromConstMem(tile_6_5, sizeof(tile_6_5));
        break;
    case 87:
        stream = SDL_IOFromConstMem(tile_7_5, sizeof(tile_7_5));
        break;
    case 88:
        stream = SDL_IOFromConstMem(tile_8_5, sizeof(tile_8_5));
        break;
    case 89:
        stream = SDL_IOFromConstMem(tile_9_5, sizeof(tile_9_5));
        break;
    case 90:
        stream = SDL_IOFromConstMem(tile_10_5, sizeof(tile_10_5));
        break;
    case 91:
        stream = SDL_IOFromConstMem(tile_11_5, sizeof(tile_11_5));
        break;
    case 92:
        stream = SDL_IOFromConstMem(tile_12_5, sizeof(tile_12_5));
        break;
    case 93:
        stream = SDL_IOFromConstMem(tile_13_5, sizeof(tile_13_5));
        break;
    case 94:
        stream = SDL_IOFromConstMem(tile_14_5, sizeof(tile_14_5));
        break;
    case 95:
        stream = SDL_IOFromConstMem(tile_15_5, sizeof(tile_15_5));
        break;
    case 96:
        stream = SDL_IOFromConstMem(tile_0_6, sizeof(tile_0_6));
        break;
    case 97:
        stream = SDL_IOFromConstMem(tile_1_6, sizeof(tile_1_6));
        break;
    case 98:
        stream = SDL_IOFromConstMem(tile_2_6, sizeof(tile_2_6));
        break;
    case 99:
        stream = SDL_IOFromConstMem(tile_3_6, sizeof(tile_3_6));
        break;
    case 100:
        stream = SDL_IOFromConstMem(tile_4_6, sizeof(tile_4_6));
        break;
    case 101:
        stream = SDL_IOFromConstMem(tile_5_6, sizeof(tile_5_6));
        break;
    case 102:
        stream = SDL_IOFromConstMem(tile_6_6, sizeof(tile_6_6));
        break;
    case 103:
        stream = SDL_IOFromConstMem(tile_7_6, sizeof(tile_7_6));
        break;
    case 104:
        stream = SDL_IOFromConstMem(tile_8_6, sizeof(tile_8_6));
        break;
    case 105:
        stream = SDL_IOFromConstMem(tile_9_6, sizeof(tile_9_6));
        break;
    case 106:
        stream = SDL_IOFromConstMem(tile_10_6, sizeof(tile_10_6));
        break;
    case 107:
        stream = SDL_IOFromConstMem(tile_11_6, sizeof(tile_11_6));
        break;
    case 108:
        stream = SDL_IOFromConstMem(tile_12_6, sizeof(tile_12_6));
        break;
    case 109:
        stream = SDL_IOFromConstMem(tile_13_6, sizeof(tile_13_6));
        break;
    case 110:
        stream = SDL_IOFromConstMem(tile_14_6, sizeof(tile_14_6));
        break;
    case 111:
        stream = SDL_IOFromConstMem(tile_15_6, sizeof(tile_15_6));
        break;
    case 112:
        stream = SDL_IOFromConstMem(tile_0_7, sizeof(tile_0_7));
        break;
    case 113:
        stream = SDL_IOFromConstMem(tile_1_7, sizeof(tile_1_7));
        break;
    case 114:
        stream = SDL_IOFromConstMem(tile_2_7, sizeof(tile_2_7));
        break;
    case 115:
        stream = SDL_IOFromConstMem(tile_3_7, sizeof(tile_3_7));
        break;
    case 116:
        stream = SDL_IOFromConstMem(tile_4_7, sizeof(tile_4_7));
        break;
    case 117:
        stream = SDL_IOFromConstMem(tile_5_7, sizeof(tile_5_7));
        break;
    case 118:
        stream = SDL_IOFromConstMem(tile_6_7, sizeof(tile_6_7));
        break;
    case 119:
        stream = SDL_IOFromConstMem(tile_7_7, sizeof(tile_7_7));
        break;
    case 120:
        stream = SDL_IOFromConstMem(tile_8_7, sizeof(tile_8_7));
        break;
    case 121:
        stream = SDL_IOFromConstMem(tile_9_7, sizeof(tile_9_7));
        break;
    case 122:
        stream = SDL_IOFromConstMem(tile_10_7, sizeof(tile_10_7));
        break;
    case 123:
        stream = SDL_IOFromConstMem(tile_11_7, sizeof(tile_11_7));
        break;
    case 124:
        stream = SDL_IOFromConstMem(tile_12_7, sizeof(tile_12_7));
        break;
    case 125:
        stream = SDL_IOFromConstMem(tile_13_7, sizeof(tile_13_7));
        break;
    case 126:
        stream = SDL_IOFromConstMem(tile_14_7, sizeof(tile_14_7));
        break;
    case 127:
        stream = SDL_IOFromConstMem(tile_15_7, sizeof(tile_15_7));
        break;
    case 128:
        stream = SDL_IOFromConstMem(tile_0_8, sizeof(tile_0_8));
        break;
    case 129:
        stream = SDL_IOFromConstMem(tile_1_8, sizeof(tile_1_8));
        break;
    case 130:
        stream = SDL_IOFromConstMem(tile_2_8, sizeof(tile_2_8));
        break;
    case 131:
        stream = SDL_IOFromConstMem(tile_3_8, sizeof(tile_3_8));
        break;
    case 132:
        stream = SDL_IOFromConstMem(tile_4_8, sizeof(tile_4_8));
        break;
    case 133:
        stream = SDL_IOFromConstMem(tile_5_8, sizeof(tile_5_8));
        break;
    case 134:
        stream = SDL_IOFromConstMem(tile_6_8, sizeof(tile_6_8));
        break;
    case 135:
        stream = SDL_IOFromConstMem(tile_7_8, sizeof(tile_7_8));
        break;
    case 136:
        stream = SDL_IOFromConstMem(tile_8_8, sizeof(tile_8_8));
        break;
    case 137:
        stream = SDL_IOFromConstMem(tile_9_8, sizeof(tile_9_8));
        break;
    case 138:
        stream = SDL_IOFromConstMem(tile_10_8, sizeof(tile_10_8));
        break;
    case 139:
        stream = SDL_IOFromConstMem(tile_11_8, sizeof(tile_11_8));
        break;
    case 140:
        stream = SDL_IOFromConstMem(tile_12_8, sizeof(tile_12_8));
        break;
    case 141:
        stream = SDL_IOFromConstMem(tile_13_8, sizeof(tile_13_8));
        break;
    case 142:
        stream = SDL_IOFromConstMem(tile_14_8, sizeof(tile_14_8));
        break;
    case 143:
        stream = SDL_IOFromConstMem(tile_15_8, sizeof(tile_15_8));
        break;
    case 144:
        stream = SDL_IOFromConstMem(tile_0_9, sizeof(tile_0_9));
        break;
    case 145:
        stream = SDL_IOFromConstMem(tile_1_9, sizeof(tile_1_9));
        break;
    case 146:
        stream = SDL_IOFromConstMem(tile_2_9, sizeof(tile_2_9));
        break;
    case 147:
        stream = SDL_IOFromConstMem(tile_3_9, sizeof(tile_3_9));
        break;
    case 148:
        stream = SDL_IOFromConstMem(tile_4_9, sizeof(tile_4_9));
        break;
    case 149:
        stream = SDL_IOFromConstMem(tile_5_9, sizeof(tile_5_9));
        break;
    case 150:
        stream = SDL_IOFromConstMem(tile_6_9, sizeof(tile_6_9));
        break;
    case 151:
        stream = SDL_IOFromConstMem(tile_7_9, sizeof(tile_7_9));
        break;
    case 152:
        stream = SDL_IOFromConstMem(tile_8_9, sizeof(tile_8_9));
        break;
    case 153:
        stream = SDL_IOFromConstMem(tile_9_9, sizeof(tile_9_9));
        break;
    case 154:
        stream = SDL_IOFromConstMem(tile_10_9, sizeof(tile_10_9));
        break;
    case 155:
        stream = SDL_IOFromConstMem(tile_11_9, sizeof(tile_11_9));
        break;
    case 156:
        stream = SDL_IOFromConstMem(tile_12_9, sizeof(tile_12_9));
        break;
    case 157:
        stream = SDL_IOFromConstMem(tile_13_9, sizeof(tile_13_9));
        break;
    case 158:
        stream = SDL_IOFromConstMem(tile_14_9, sizeof(tile_14_9));
        break;
    case 159:
        stream = SDL_IOFromConstMem(tile_15_9, sizeof(tile_15_9));
        break;
    case 160:
        stream = SDL_IOFromConstMem(tile_0_10, sizeof(tile_0_10));
        break;
    case 161:
        stream = SDL_IOFromConstMem(tile_1_10, sizeof(tile_1_10));
        break;
    case 162:
        stream = SDL_IOFromConstMem(tile_2_10, sizeof(tile_2_10));
        break;
    case 163:
        stream = SDL_IOFromConstMem(tile_3_10, sizeof(tile_3_10));
        break;
    case 164:
        stream = SDL_IOFromConstMem(tile_4_10, sizeof(tile_4_10));
        break;
    case 165:
        stream = SDL_IOFromConstMem(tile_5_10, sizeof(tile_5_10));
        break;
    case 166:
        stream = SDL_IOFromConstMem(tile_6_10, sizeof(tile_6_10));
        break;
    case 167:
        stream = SDL_IOFromConstMem(tile_7_10, sizeof(tile_7_10));
        break;
    case 168:
        stream = SDL_IOFromConstMem(tile_8_10, sizeof(tile_8_10));
        break;
    case 169:
        stream = SDL_IOFromConstMem(tile_9_10, sizeof(tile_9_10));
        break;
    case 170:
        stream = SDL_IOFromConstMem(tile_10_10, sizeof(tile_10_10));
        break;
    case 171:
        stream = SDL_IOFromConstMem(tile_11_10, sizeof(tile_11_10));
        break;
    case 172:
        stream = SDL_IOFromConstMem(tile_12_10, sizeof(tile_12_10));
        break;
    case 173:
        stream = SDL_IOFromConstMem(tile_13_10, sizeof(tile_13_10));
        break;
    case 174:
        stream = SDL_IOFromConstMem(tile_14_10, sizeof(tile_14_10));
        break;
    case 175:
        stream = SDL_IOFromConstMem(tile_15_10, sizeof(tile_15_10));
        break;
    }
    return IMG_LoadTexture_IO(renderer, stream, true);
}
