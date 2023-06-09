#include <string>
#include <unordered_map>
#include "shannon-fano.hpp"
namespace shannon_fano
{
  std::unordered_map<std::string, code> compression_table = {
      /*   */ {" ", code(3, 0b000)},
      /* e */ {"e", code(3, 0b001)},
      /* n */ {"n", code(4, 0b0100)},
      /* i */ {"i", code(4, 0b0101)},
      /* r */ {"r", code(4, 0b0110)},
      /* t */ {"t", code(4, 0b0111)},
      /* s */ {"s", code(4, 0b1000)},
      /* a */ {"a", code(5, 0b10010)},
      /* d */ {"d", code(5, 0b10011)},
      /* l */ {"l", code(5, 0b10100)},
      /* u */ {"u", code(5, 0b10101)},
      /* h */ {"h", code(5, 0b10110)},
      /* o */ {"o", code(5, 0b10111)},
      /* c */ {"c", code(5, 0b11000)},
      /* m */ {"m", code(6, 0b110010)},
      /* g */ {"g", code(6, 0b110011)},
      /* b */ {"b", code(6, 0b110100)},
      /* f */ {"f", code(7, 0b1101010)},
      /* k */ {"k", code(7, 0b1101011)},
      /* w */ {"w", code(6, 0b110110)},
      /* . */ {".", code(7, 0b1101110)},
      /* z */ {"z", code(7, 0b1101111)},
      /* p */ {"p", code(7, 0b1110000)},
      /* , */ {",", code(7, 0b1110001)},
      /* S */ {"S", code(7, 0b1110010)},
      /* v */ {"v", code(7, 0b1110011)},
      /* - */ {"-", code(7, 0b1110100)},
      /* D */ {"D", code(8, 0b11101010)},
      /* A */ {"A", code(8, 0b11101011)},
      /* ü */ {"\xc3\xbc", code(8, 0b11101100)},
      /* ä */ {"\xc3\xa4", code(8, 0b11101101)},
      /* P */ {"P", code(8, 0b11101110)},
      /* B */ {"B", code(9, 0b111011110)},
      /* T */ {"T", code(9, 0b111011111)},
      /* M */ {"M", code(8, 0b11110000)},
      /* E */ {"E", code(9, 0b111100010)},
      /* 0 */ {"0", code(9, 0b111100011)},
      /* I */ {"I", code(8, 0b11110010)},
      /* 1 */ {"1", code(9, 0b111100110)},
      /* W */ {"W", code(9, 0b111100111)},
      /* F */ {"F", code(9, 0b111101000)},
      /* K */ {"K", code(9, 0b111101001)},
      /* G */ {"G", code(9, 0b111101010)},
      /* V */ {"V", code(9, 0b111101011)},
      /* y */ {"y", code(9, 0b111101100)},
      /* 2 */ {"2", code(9, 0b111101101)},
      /* R */ {"R", code(9, 0b111101110)},
      /* L */ {"L", code(9, 0b111101111)},
      /* ö */ {"\xc3\xb6", code(9, 0b111110000)},
      /* U */ {"U", code(9, 0b111110001)},
      /* N */ {"N", code(9, 0b111110010)},
      /* C */ {"C", code(9, 0b111110011)},
      /* H */ {"H", code(9, 0b111110100)},
      /* ) */ {")", code(10, 0b1111101010)},
      /* : */ {":", code(10, 0b1111101011)},
      /* ( */ {"(", code(9, 0b111110110)},
      /* x */ {"x", code(10, 0b1111101110)},
      /* O */ {"O", code(10, 0b1111101111)},
      /* “ */ {"\xe2\x80\x9c", code(9, 0b111111000)},
      /* „ */ {"\xe2\x80\x9e", code(10, 0b1111110010)},
      /* Z */ {"Z", code(10, 0b1111110011)},
      /* 3 */ {"3", code(10, 0b1111110100)},
      /* ß */ {"\xc3\x9f", code(10, 0b1111110101)},
      /* j */ {"j", code(10, 0b1111110110)},
      /* 4 */ {"4", code(11, 0b11111101110)},
      /* / */ {"/", code(11, 0b11111101111)},
      /* 5 */ {"5", code(10, 0b1111111000)},
      /* 6 */ {"6", code(11, 0b11111110010)},
      /* J */ {"J", code(11, 0b11111110011)},
      /* – */ {"\xe2\x80\x93", code(11, 0b11111110100)},
      /* 8 */ {"8", code(11, 0b11111110101)},
      /* 7 */ {"7", code(11, 0b11111110110)},
      /* 9 */ {"9", code(12, 0b111111101110)},
      /* Q */ {"Q", code(12, 0b111111101111)},
      /* " */ {"\"", code(12, 0b111111110000)},
      /* @ */ {"@", code(12, 0b111111110001)},
      /* = */ {"=", code(12, 0b111111110010)},
      /* X */ {"X", code(12, 0b111111110011)},
      /* [ */ {"[", code(12, 0b111111110100)},
      /* ] */ {"]", code(12, 0b111111110101)},
      /* ; */ {";", code(12, 0b111111110110)},
      /* ? */ {"?", code(13, 0b1111111101110)},
      /* _ */ {"_", code(13, 0b1111111101111)},
      /* ’ */ {"\xe2\x80\x99", code(12, 0b111111111000)},
      /* q */ {"q", code(13, 0b1111111110010)},
      /* Ü */ {"\xc3\x9c", code(13, 0b1111111110011)},
      /* > */ {">", code(13, 0b1111111110100)},
      /* < */ {"<", code(13, 0b1111111110101)},
      /* ' */ {"'", code(14, 0b11111111101100)},
      /* + */ {"+", code(14, 0b11111111101101)},
      /* { */ {"{", code(14, 0b11111111101110)},
      /* & */ {"&", code(14, 0b11111111101111)},
      /* } */ {"}", code(14, 0b11111111110000)},
      /* Ä */ {"\xc3\x84", code(14, 0b11111111110001)},
      /* Y */ {"Y", code(14, 0b11111111110010)},
      /* Ö */ {"\xc3\x96", code(14, 0b11111111110011)},
      /* * */ {"*", code(14, 0b11111111110100)},
      /* … */ {"\xe2\x80\xa6", code(15, 0b111111111101010)},
      /* # */ {"#", code(15, 0b111111111101011)},
      /* | */ {"|", code(14, 0b11111111110110)},
      /* € */ {"\xe2\x82\xac", code(15, 0b111111111101110)},
      /* § */ {"\xc2\xa7", code(15, 0b111111111101111)},
      /* ! */ {"!", code(15, 0b111111111110000)},
      /* \ */ {"\\", code(15, 0b111111111110001)},
      /* % */ {"%", code(16, 0b1111111111100100)},
      /* $ */ {"$", code(16, 0b1111111111100101)},
      /* × */ {"\xc3\x97", code(16, 0b1111111111100110)},
      /* é */ {"\xc3\xa9", code(16, 0b1111111111100111)},
      /* µ */ {"\xc2\xb5", code(17, 0b11111111111010000)},
      /* « */ {"\xc2\xab", code(18, 0b111111111110100010)},
      /* − */ {"\xe2\x88\x92", code(18, 0b111111111110100011)},
      /* à */ {"\xc3\xa0", code(18, 0b111111111110100100)},
      /* � */ {"\xda", code(19, 0b1111111111101001010)},
      /*  */ {"\x07", code(19, 0b1111111111101001011)},
      /*  */ {"\x04", code(19, 0b1111111111101001100)},
      /* � */ {"\xa3", code(19, 0b1111111111101001101)},
      /*  */ {"\x03", code(19, 0b1111111111101001110)},
      /* � */ {"\xe3", code(19, 0b1111111111101001111)},
      /*   */ {"\x00", code(19, 0b1111111111101010000)},
      /* � */ {"\xe9", code(19, 0b1111111111101010001)},
      /*
       */
      {"\x0a", code(19, 0b1111111111101010010)},
      /* ć */ {"\xc4\x87", code(19, 0b1111111111101010011)},
      /* � */ {"\xa8", code(19, 0b1111111111101010100)},
      /* Ž */ {"\xc5\xbd", code(19, 0b1111111111101010101)},
      /* � */ {"\xbf", code(19, 0b1111111111101010110)},
      /*  */ {"\x0b", code(19, 0b1111111111101010111)},
      /* � */ {"\x9d", code(19, 0b1111111111101011000)},
      /* ‟ */ {"\xe2\x80\x9f", code(19, 0b1111111111101011001)},
      /*  */ {"\x0c", code(19, 0b1111111111101011010)},
      /* 	 */ {"\x09", code(19, 0b1111111111101011011)},
      /*
       */
      {"\x0d", code(19, 0b1111111111101011100)},
      /*  */ {"\x0e", code(19, 0b1111111111101011101)},
      /* • */ {"\xe2\x80\xa2", code(18, 0b111111111110101111)},
      /*  */ {"\x05", code(19, 0b1111111111101100000)},
      /* ” */ {"\xe2\x80\x9d", code(19, 0b1111111111101100001)},
      /* � */ {"\xff", code(19, 0b1111111111101100010)},
      /* � */ {"\xd0", code(19, 0b1111111111101100011)},
      /* � */ {"\x8b", code(19, 0b1111111111101100100)},
      /* � */ {"\xf7", code(19, 0b1111111111101100101)},
      /*  */ {"\x10", code(19, 0b1111111111101100110)},
      /* � */ {"\xa0", code(19, 0b1111111111101100111)},
      /* � */ {"\xbc", code(19, 0b1111111111101101000)},
      /*  */ {"\x0f", code(19, 0b1111111111101101001)},
      /* ≡ */ {"\xe2\x89\xa1", code(19, 0b1111111111101101010)},
      /*   */ {"\xe2\x80\xaf", code(19, 0b1111111111101101011)},
      /*  */ {"\x11", code(19, 0b1111111111101101100)},
      /*  */ {"\x12", code(19, 0b1111111111101101101)},
      /* � */ {"\x9f", code(18, 0b111111111110110111)},
      /* ² */ {"\xc2\xb2", code(19, 0b1111111111101110000)},
      /* � */ {"\x93", code(19, 0b1111111111101110001)},
      /* � */ {"\x89", code(19, 0b1111111111101110010)},
      /* — */ {"\xe2\x80\x94", code(19, 0b1111111111101110011)},
      /*  */ {"\x13", code(19, 0b1111111111101110100)},
      /*  */ {"\x15", code(19, 0b1111111111101110101)},
      /* � */ {"\xa4", code(19, 0b1111111111101110110)},
      /* ~ */ {"~", code(19, 0b1111111111101110111)},
      /*  */ {"\x18", code(19, 0b1111111111101111000)},
      /*  */ {"\x1a", code(19, 0b1111111111101111001)},
      /* � */ {"\xe2", code(19, 0b1111111111101111010)},
      /*  */ {"\x1b", code(19, 0b1111111111101111011)},
      /* � */ {"\x9b", code(19, 0b1111111111101111100)},
      /*  */ {"\x1c", code(19, 0b1111111111101111101)},
      /*  */ {"\x01", code(18, 0b111111111110111111)},
      /* � */ {"\x97", code(19, 0b1111111111110000000)},
      /* � */ {"\xd6", code(19, 0b1111111111110000001)},
      /*  */ {"\x1d", code(19, 0b1111111111110000010)},
      /* � */ {"\xfa", code(19, 0b1111111111110000011)},
      /* � */ {"\x8f", code(19, 0b1111111111110000100)},
      /*  */ {"\x1f", code(19, 0b1111111111110000101)},
      /* � */ {"\x82", code(19, 0b1111111111110000110)},
      /* ` */ {"`", code(19, 0b1111111111110000111)},
      /* � */ {"\x81", code(19, 0b1111111111110001000)},
      /* � */ {"\x83", code(19, 0b1111111111110001001)},
      /* � */ {"\xed", code(19, 0b1111111111110001010)},
      /* � */ {"\xe5", code(19, 0b1111111111110001011)},
      /* � */ {"\x84", code(19, 0b1111111111110001100)},
      /* � */ {"\x99", code(19, 0b1111111111110001101)},
      /* � */ {"\x86", code(19, 0b1111111111110001110)},
      /* � */ {"\xce", code(19, 0b1111111111110001111)},
      /* � */ {"\x87", code(19, 0b1111111111110010000)},
      /* � */ {"\x8c", code(19, 0b1111111111110010001)},
      /* � */ {"\x8d", code(19, 0b1111111111110010010)},
      /* � */ {"\x90", code(19, 0b1111111111110010011)},
      /* � */ {"\x88", code(19, 0b1111111111110010100)},
      /* � */ {"\xc6", code(19, 0b1111111111110010101)},
      /* � */ {"\x91", code(19, 0b1111111111110010110)},
      /* � */ {"\xf9", code(19, 0b1111111111110010111)},
      /* � */ {"\xcb", code(19, 0b1111111111110011000)},
      /* ä */ {"a\xcc\x88", code(19, 0b1111111111110011001)},
      /* � */ {"\x92", code(19, 0b1111111111110011010)},
      /* � */ {"\x94", code(19, 0b1111111111110011011)},
      /* � */ {"\x95", code(19, 0b1111111111110011100)},
      /* � */ {"\xac", code(19, 0b1111111111110011101)},
      /* � */ {"\x96", code(19, 0b1111111111110011110)},
      /* � */ {"\x98", code(19, 0b1111111111110011111)},
      /*  */ {"\x1e", code(19, 0b1111111111110100000)},
      /* � */ {"\x80", code(19, 0b1111111111110100001)},
      /* � */ {"\x9c", code(19, 0b1111111111110100010)},
      /* � */ {"\xea", code(19, 0b1111111111110100011)},
      /* � */ {"\x9e", code(19, 0b1111111111110100100)},
      /* � */ {"\xe7", code(19, 0b1111111111110100101)},
      /* � */ {"\xa1", code(19, 0b1111111111110100110)},
      /* � */ {"\xa2", code(19, 0b1111111111110100111)},
      /*  */ {"\x14", code(19, 0b1111111111110101000)},
      /* � */ {"\xa5", code(19, 0b1111111111110101001)},
      /* � */ {"\xa6", code(19, 0b1111111111110101010)},
      /* � */ {"\xad", code(19, 0b1111111111110101011)},
      /* � */ {"\xa7", code(19, 0b1111111111110101100)},
      /* � */ {"\xb7", code(19, 0b1111111111110101101)},
      /* � */ {"\xa9", code(19, 0b1111111111110101110)},
      /* � */ {"\xd4", code(19, 0b1111111111110101111)},
      /* � */ {"\xbd", code(19, 0b1111111111110110000)},
      /* � */ {"\xfb", code(19, 0b1111111111110110001)},
      /* � */ {"\xc3", code(19, 0b1111111111110110010)},
      /* � */ {"\xaa", code(19, 0b1111111111110110011)},
      /* � */ {"\xaf", code(19, 0b1111111111110110100)},
      /* ü */ {"u\xcc\x88", code(19, 0b1111111111110110101)},
      /* � */ {"\xb1", code(19, 0b1111111111110110110)},
      /* � */ {"\xd1", code(19, 0b1111111111110110111)},
      /* � */ {"\xab", code(19, 0b1111111111110111000)},
      /* � */ {"\xb3", code(19, 0b1111111111110111001)},
      /* � */ {"\xb4", code(19, 0b1111111111110111010)},
      /* � */ {"\xbe", code(19, 0b1111111111110111011)},
      /* � */ {"\xc8", code(19, 0b1111111111110111100)},
      /*  */ {"\x7f", code(19, 0b1111111111110111101)},
      /* � */ {"\xb5", code(18, 0b111111111111011111)},
      /*  */ {"\x19", code(19, 0b1111111111111000000)},
      /* � */ {"\xae", code(19, 0b1111111111111000001)},
      /*  */ {"\x16", code(19, 0b1111111111111000010)},
      /* � */ {"\xb6", code(19, 0b1111111111111000011)},
      /* � */ {"\xe6", code(19, 0b1111111111111000100)},
      /* � */ {"\x8a", code(19, 0b1111111111111000101)},
      /* � */ {"\xb8", code(19, 0b1111111111111000110)},
      /* � */ {"\xb9", code(19, 0b1111111111111000111)},
      /* � */ {"\xf1", code(19, 0b1111111111111001000)},
      /* � */ {"\xba", code(19, 0b1111111111111001001)},
      /* � */ {"\xf8", code(19, 0b1111111111111001010)},
      /* � */ {"\xbb", code(19, 0b1111111111111001011)},
      /* � */ {"\xc0", code(19, 0b1111111111111001100)},
      /* � */ {"\xc1", code(19, 0b1111111111111001101)},
      /* � */ {"\xc4", code(19, 0b1111111111111001110)},
      /* � */ {"\xc5", code(19, 0b1111111111111001111)},
      /* � */ {"\xc7", code(19, 0b1111111111111010000)},
      /* � */ {"\xc9", code(19, 0b1111111111111010001)},
      /* � */ {"\xca", code(19, 0b1111111111111010010)},
      /* � */ {"\xcc", code(19, 0b1111111111111010011)},
      /* © */ {"\xc2\xa9", code(19, 0b1111111111111010100)},
      /* � */ {"\xcd", code(19, 0b1111111111111010101)},
      /* ^ */ {"^", code(19, 0b1111111111111010110)},
      /* � */ {"\xdd", code(19, 0b1111111111111010111)},
      /* � */ {"\xcf", code(19, 0b1111111111111011000)},
      /* � */ {"\xd2", code(19, 0b1111111111111011001)},
      /* � */ {"\xee", code(19, 0b1111111111111011010)},
      /* � */ {"\xd3", code(19, 0b1111111111111011011)},
      /* � */ {"\xd5", code(19, 0b1111111111111011100)},
      /* � */ {"\xd7", code(19, 0b1111111111111011101)},
      /* � */ {"\xd8", code(19, 0b1111111111111011110)},
      /*  */ {"\x08", code(19, 0b1111111111111011111)},
      /* � */ {"\xec", code(19, 0b1111111111111100000)},
      /* � */ {"\xd9", code(19, 0b1111111111111100001)},
      /* � */ {"\xdb", code(19, 0b1111111111111100010)},
      /* � */ {"\xdc", code(19, 0b1111111111111100011)},
      /* � */ {"\xf6", code(19, 0b1111111111111100100)},
      /* � */ {"\xde", code(19, 0b1111111111111100101)},
      /* š */ {"\xc5\xa1", code(19, 0b1111111111111100110)},
      /* � */ {"\xeb", code(19, 0b1111111111111100111)},
      /* � */ {"\xdf", code(19, 0b1111111111111101000)},
      /* � */ {"\x9a", code(19, 0b1111111111111101001)},
      /* � */ {"\xc2", code(19, 0b1111111111111101010)},
      /* � */ {"\xe0", code(19, 0b1111111111111101011)},
      /* � */ {"\xe1", code(19, 0b1111111111111101100)},
      /* � */ {"\xe4", code(19, 0b1111111111111101101)},
      /* � */ {"\xe8", code(19, 0b1111111111111101110)},
      /* � */ {"\xef", code(19, 0b1111111111111101111)},
      /* � */ {"\xf0", code(19, 0b1111111111111110000)},
      /* � */ {"\xb0", code(19, 0b1111111111111110001)},
      /* � */ {"\xf2", code(19, 0b1111111111111110010)},
      /* � */ {"\xb2", code(19, 0b1111111111111110011)},
      /* � */ {"\xf3", code(19, 0b1111111111111110100)},
      /* � */ {"\xf4", code(19, 0b1111111111111110101)},
      /*  */ {"\x06", code(19, 0b1111111111111110110)},
      /* � */ {"\xf5", code(19, 0b1111111111111110111)},
      /* � */ {"\xfc", code(19, 0b1111111111111111000)},
      /* � */ {"\x8e", code(19, 0b1111111111111111001)},
      /* � */ {"\xfd", code(19, 0b1111111111111111010)},
      /* � */ {"\x85", code(19, 0b1111111111111111011)},
      /*  */ {"\x02", code(19, 0b1111111111111111100)},
      /* � */ {"\xfe", code(19, 0b1111111111111111101)},
      /*  */ {"\x17", code(18, 0b111111111111111111)},
  };
}
