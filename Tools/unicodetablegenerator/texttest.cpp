// texttest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "fastlz.h"

uint32_t g_indx = 0;

struct UnicodeCharNode
{
    uint32_t m_utf8;
    uint32_t m_utf16;
};

UnicodeCharNode g_UnicodeChars[0x32000];

void Write(FILE* f, size_t sz, uint32_t utf8, uint32_t utf16)
{
    int colcount = 0;
    static char b[50];
    for (size_t i = 0; i < sz; ++i)
    {
        int rn = snprintf(b, 50, "{0x%X,0x%X},", utf8, utf16);
        if (rn > 0)
        {
            fwrite(b, 1, rn, f);
        }
        ++utf8;
        ++utf16;
        ++colcount;
        if (colcount == 16)
        {
            colcount = 0;
            char c = '\n';
            fwrite(&c, 1, 1, f);
        }

        ++g_indx;
    }
    char c = '\n';
    fwrite(&c, 1, 1, f);
}

int main()
{
    FILE* f = fopen("UnicodeChars.inl", "wb");
    if (f)
    {
        int colcount = 0;
        for (uint32_t i = 0; i < 0x32000; ++i)
        {
            static char b[50];

            uint32_t utf8 = 0;
            uint32_t utf16 = 0;

            uint8_t * utf8buf = (uint8_t*)&utf8;
            uint16_t* utf16buf = (uint16_t*)&utf16;
            
            if (i)
            {
                if (i < 0x80)
                {
                    utf8 = i;
                }
                else if (i < 0x800)
                {
                    utf8buf[1] = 0xC0 | (i >> 6);            /* 110xxxxx */
                    utf8buf[0] = 0x80 | (i & 0x3F);          /* 10xxxxxx */
                }
                else if (i < 0x10000)
                {
                    utf8buf[2] = 0xE0 | (i >> 12);           /* 1110xxxx */
                    utf8buf[1] = 0x80 | ((i >> 6) & 0x3F);   /* 10xxxxxx */
                    utf8buf[0] = 0x80 | (i & 0x3F);          /* 10xxxxxx */
                }
                else if (i < 0x110000)
                {
                    utf8buf[3] = 0xF0 | (i >> 18);           /* 11110xxx */
                    utf8buf[2] = 0x80 | ((i >> 12) & 0x3F);  /* 10xxxxxx */
                    utf8buf[1] = 0x80 | ((i >> 6) & 0x3F);   /* 10xxxxxx */
                    utf8buf[0] = 0x80 | (i & 0x3F);          /* 10xxxxxx */
                }

                if (i < 0xD800 || (i > 0xDFFF && i < 0x10000))
                {
                    utf16buf[1] = char16_t(i);
                    utf16buf[0] = 0;
                }
                else
                {
                    uint32_t i2 = i - 0x010000;
                    utf16buf[1] = char16_t(((0b1111'1111'1100'0000'0000 & i2) >> 10) + 0xD800);
                    utf16buf[0] = char16_t(((0b0000'0000'0011'1111'1111 & i2) >> 00) + 0xDC00);
                }
            }

            g_UnicodeChars[i].m_utf16 = utf16;
            g_UnicodeChars[i].m_utf8 = utf8;

            int rn = snprintf(b, 50, "{0x%X,0x%X},", utf8, utf16);
            if (rn > 0)
            {
                fwrite(b, 1, rn, f);
            }

            ++colcount;
            if (colcount == 16)
            {
                colcount = 0;
                char c = '\n';
                fwrite(&c, 1, 1, f);
            }
        }

        colcount = 0;

        size_t sizeUncompressed = 0x32000 * sizeof(UnicodeCharNode);
        uint8_t* compressedOutput = (uint8_t*)malloc(sizeUncompressed  + (sizeUncompressed / 3));
        if (compressedOutput)
        {
            int compressed_size = fastlz_compress_level(
                2,
                g_UnicodeChars,
                sizeUncompressed,
                compressedOutput);
            if ((uint32_t)compressed_size >= sizeUncompressed)
            {
                printf("Failed to compress\n");
                free(compressedOutput);
            }
            else
            {
                char b[50];
                FILE* f2 = fopen("UnicodeCharsLZ.inl", "wb");
                if (f2)
                {
                    fprintf(f2, "// compressed size [%i]\r\n", compressed_size);
                    for (int i = 0; i < compressed_size; ++i)
                    {
                        int rn = snprintf(b, 50, "0x%X,", compressedOutput[i]);
                        if (rn > 0)
                        {
                            fwrite(b, 1, rn, f2);
                        }

                        ++colcount;
                        if (colcount == 16)
                        {
                            colcount = 0;
                            char c = '\n';
                            fwrite(&c, 1, 1, f2);
                        }
                    }
                    fclose(f2);
                }
                free(compressedOutput);
            }
        }
        //0000
//        Write(f, 0x80, 0, 0); // C0 Controls and Basic Latin
//        Write(f, 0x80, 0xC280, 0x80); //C1 Controls and Latin-1 Supplement
//        Write(f, 0x80, 0xC480, 0x100); //Latin Extended-A
//        Write(f, 0x80, 0xC680, 0x180); //Latin Extended-B
//        Write(f, 5 * 16, 0xC880, 0x200);
//        Write(f, 6 * 16, 0xC990, 0x250); //IPA Extensions
//        Write(f, 5 * 16, 0xCAB0, 0x2B0); //Spacing Modifier Letters
//        Write(f, 7 * 16, 0xCC80, 0x300); //Combining Diacritical Marks
//        Write(f, 9 * 16, 0xCDB0, 0x370); //Greek and Coptic
//        Write(f, 16 * 16, 0xD080, 0x400); //Cyrillic
//        Write(f, 3 * 16, 0xD480, 0x500); //Cyrillic Supplement
//        Write(f, 6 * 16, 0xD4B0, 0x530); //Armenian
//        Write(f, 7 * 16, 0xD690, 0x590); //Hebrew
//        Write(f, 16 * 16, 0xD880, 0x600); //Arabic
//        Write(f, 5 * 16, 0xDC80, 0x700); //Syriac
//        Write(f, 3 * 16, 0xDD90, 0x750); //Arabic Supplement
//        Write(f, 4 * 16, 0xDE80, 0x780); //Thaana
//        Write(f, 4 * 16, 0xDF80, 0x7C0); //NKo
//        Write(f, 4 * 16, 0xE0A080, 0x800); //Samaritan
//        Write(f, 2 * 16, 0xE0A180, 0x840); //Mandaic
//        Write(f, 16, 0xE0A1A0, 0x860); //Syriac Supplement
//        Write(f, 3 * 16, 0xE0A1B0, 0x870); //Arabic Extended-B
//        Write(f, 6 * 16, 0xE0A2A0, 0x8A0); //Arabic Extended-A
//        Write(f, 8 * 16, 0xE0A480, 0x900); //Devanagari
//        Write(f, 8 * 16, 0xE0A680, 0x980); //Bengali
//        Write(f, 8 * 16, 0xE0A880, 0xA00); //Gurmukhi
//        Write(f, 8 * 16, 0xE0AA80, 0xA80); //Gujarati
//        Write(f, 8 * 16, 0xE0AC80, 0xB00); //Oriya
//        Write(f, 8 * 16, 0xE0AE80, 0xB80); //Tamil
//        Write(f, 8 * 16, 0xE0B080, 0xC00); //Telugu
//        Write(f, 8 * 16, 0xE0B280, 0xC80); //Kannada
//        Write(f, 8 * 16, 0xE0B480, 0xD00); //Malayalam
//        Write(f, 8 * 16, 0xE0B680, 0xD80); //Sinhala
//        Write(f, 8 * 16, 0xE0B880, 0xE00); //Thai
//        Write(f, 8 * 16, 0xE0BA80, 0xE80); //Lao
//        Write(f, 16 * 16, 0xE0BC80, 0xF00); //Tibetan
//
//        //1000
//        Write(f, 16 * 16, 0xE18080, 0x1000); //Myanmar
//        Write(f, 6 * 16, 0xE182A0, 0x10A0); //Georgian
//        Write(f, 6 * 16, 0xE18480, 0x1100); //Hangul Jamo
//        Write(f, 16 * 16, 0xE18880, 0x1200); //Ethiopic
//        Write(f, 8 * 16, 0xE18C80, 0x1300);
//        Write(f, 2 * 16, 0xE18E80, 0x1380); //Ethiopic Supplement
//        Write(f, 6 * 16, 0xE18EA0, 0x13A0); //Cherokee
//        Write(f, 16 * 16, 0xE19080, 0x1400); //Unified Canadian Aboriginal Syllabics
//        Write(f, 16 * 16, 0xE19480, 0x1500);
//        Write(f, 8 * 16, 0xE19880, 0x1600);
//        Write(f, 2 * 16, 0xE19A80, 0x1680); //Ogham
//        Write(f, 6 * 16, 0xE19AA0, 0x16A0); //Runic
//        Write(f, 2 * 16, 0xE19C80, 0x1700); //Tagalog
//        Write(f, 2 * 16, 0xE19CA0, 0x1720); //Hanunoo
//        Write(f, 2 * 16, 0xE19D80, 0x1740); //Buhid
//        Write(f, 2 * 16, 0xE19DA0, 0x1760); //Tagbanwa
//        Write(f, 8 * 16, 0xE19E80, 0x1780); //Khmer
//        Write(f, 11 * 16, 0xE1A080, 0x1800); //Mongolian
//        Write(f, 5 * 16, 0xE1A2B0, 0x18B0); //Unified Canadian Aboriginal Syllabics Extended
//        Write(f, 5 * 16, 0xE1A480, 0x1900); //Limbu
//        Write(f, 3 * 16, 0xE1A590, 0x1950); //Tai Le
//        Write(f, 6 * 16, 0xE1A680, 0x1980); //New Tai Lue
//        Write(f, 2 * 16, 0xE1A7A0, 0x19E0); //Khmer Symbols
//        Write(f, 2 * 16, 0xE1A880, 0x1A00); //Buginese
//        Write(f, 9 * 16, 0xE1A8A0, 0x1A20); //Tai Tham
//        Write(f, 5 * 16, 0xE1AAB0, 0x1AB0); //Combining Diacritical Marks Extended
//        Write(f, 8 * 16, 0xE1AC80, 0x1B00); //Balinese
//        Write(f, 4 * 16, 0xE1AE80, 0x1B80); //Sundanese
//        Write(f, 4 * 16, 0xE1AF80, 0x1BC0); //Batak
//        Write(f, 5 * 16, 0xE1B080, 0x1C00); //Lepcha
//        Write(f, 3 * 16, 0xE1B190, 0x1C50); //Ol Chiki
//        Write(f, 16, 0xE1B280, 0x1C80); //Cyrillic Extended-C
//        Write(f, 3 * 16, 0xE1B290, 0x1C90); //Georgian Extended
//        Write(f, 16, 0xE1B380, 0x1CC0); //Sundanese Supplement
//        Write(f, 3 * 16, 0xE1B390, 0x1CD0); //Vedic Extensions
//        Write(f, 8 * 16, 0xE1B480, 0x1D00); //Phonetic Extensions
//        Write(f, 4 * 16, 0xE1B680, 0x1D80); //Phonetic Extensions Supplement
//        Write(f, 4 * 16, 0xE1B780, 0x1DC0); //Combining Diacritical Marks Supplement
//        Write(f, 16 * 16, 0xE1B880, 0x1E00); //Latin Extended Additional
//        Write(f, 16 * 16, 0xE1BC80, 0x1F00); //Greek Extended
//
//        /*for (uint32_t i = g_indx; i < 0x31FFF; ++i)
//        {
//            static char b[50];
//            int rn = snprintf(b, 50, "{0x%X,0x%X},", 0, 0);
//            if (rn > 0)
//            {
//                fwrite(b, 1, rn, f);
//            }
//        }*/
//
//        //2000
//        Write(f, 7 * 16, 0xE28080, 0x2000); //General Punctuation
//        Write(f, 3 * 16, 0xE281B0, 0x2070); //Superscripts and Subscripts
//        Write(f, 3 * 16, 0xE282A0, 0x20A0); //Currency Symbols
//        Write(f, 3 * 16, 0xE28390, 0x20D0); //Combining Diacritical Marks for Symbols
//        Write(f, 5 * 16, 0xE28480, 0x2100); //Letterlike Symbols
//        Write(f, 4 * 16, 0xE28590, 0x2150); //Number Forms
//        Write(f, 7 * 16, 0xE28690, 0x2190); //Arrows
//        Write(f, 16 * 16, 0xE28880, 0x2200); //Mathematical Operators
//        Write(f, 16 * 16, 0xE28C80, 0x2300); //Miscellaneous Technical
//        Write(f, 4 * 16, 0xE29080, 0x2400); //Control Pictures
//        Write(f, 2 * 16, 0xE29180, 0x2440); //Optical Character Recognition
//        Write(f, 10 * 16, 0xE291A0, 0x2460); //Enclosed Alphanumerics
//        Write(f, 8 * 16, 0xE29480, 0x2500); //Box Drawing
//        Write(f, 2 * 16, 0xE29680, 0x2580); //Box Elements
//        Write(f, 6 * 16, 0xE296A0, 0x25A0); //Geometric Shapes
//        Write(f, 16 * 16, 0xE29880, 0x2600); //Miscellaneous Symbols
//        Write(f, 12 * 16, 0xE29C80, 0x2700); //Dingbats
//        Write(f, 3 * 16, 0xE29F80, 0x27C0); //Miscellaneous Mathematical Symbols-A
//        Write(f, 16, 0xE29FB0, 0x27F0); //Supplemental Arrows-A
//        Write(f, 16 * 16, 0xE2A080, 0x2800); //Braille Patterns
//        Write(f, 8 * 16, 0xE2A480, 0x2900); //Supplemental Arrows-B
//        Write(f, 8 * 16, 0xE2A680, 0x2980); //Miscellaneous Mathematical Symbols-B
//        Write(f, 16 * 16, 0xE2A880, 0x2A00); //Supplemental Mathematical Operators
//        Write(f, 16 * 16, 0xE2AC80, 0x2B00); //Miscellaneous Symbols and Arrows
//        Write(f, 6 * 16, 0xE2B080, 0x2C00); //Glagolitic
//        Write(f, 2 * 16, 0xE2B1A0, 0x2C60); //Latin Extended-C
//        Write(f, 8 * 16, 0xE2B280, 0x2C80); //Coptic
//        Write(f, 3 * 16, 0xE2B480, 0x2D00); //Georgian Supplement
//        Write(f, 5 * 16, 0xE2B4B0, 0x2D00); //Tifinagh
//        Write(f, 6 * 16, 0xE2B680, 0x2D80); //Ethiopic Extended
//        Write(f, 2 * 16, 0xE2B7A0, 0x2DE0); //Cyrillic Extended-A
//        Write(f, 8 * 16, 0xE2B880, 0x2E00); //Supplemental Punctuation
//        Write(f, 8 * 16, 0xE2BA80, 0x2E80); //CJK Radicals Supplement
//        Write(f, 14 * 16, 0xE2BC80, 0x2F00); //Kangxi Radicals
//        Write(f, 16, 0xE2BFA0, 0x2FE0); //Unassigned
//        Write(f, 16, 0xE2BFB0, 0x2FF0); //Ideographic Description Characters
//
//        //3000
//        Write(f, 4 * 16, 0xE38080, 0x3000); //CJK Symbols and Punctuation
//        Write(f, 6 * 16, 0xE38180, 0x3040); //Hiragana
//        Write(f, 6 * 16, 0xE382A0, 0x30A0); //Katakana
//        Write(f, 3 * 16, 0xE38480, 0x3100); //Bopomofo
//        Write(f, 6 * 16, 0xE384B0, 0x3130); //Hangul Compatibility Jamo
//        Write(f, 16, 0xE38690, 0x3190); //Kanbun
//        Write(f, 2 * 16, 0xE386A0, 0x31A0); //Bopomofo Extended
//        Write(f, 3 * 16, 0xE38780, 0x31C0); //CJK Strokes
//        Write(f, 16, 0xE387B0, 0x31F0); //Katakana Phonetic Extensions
//        Write(f, 16 * 16, 0xE38880, 0x3200); //Enclosed CJK Letters and Months
//        Write(f, 16 * 16, 0xE38C80, 0x3300); //CJK Compatibility
//        Write(f, 16 * 16, 0xE39080, 0x3400);
///*3500*/Write(f, 16 * 16, 0xE39480, 0x3500);
///*3600*/Write(f, 16 * 16, 0xE39880, 0x3600);
///*3700*/Write(f, 16 * 16, 0xE39C80, 0x3700);
///*3800*/Write(f, 16 * 16, 0xE3A080, 0x3800);
///*3900*/Write(f, 16 * 16, 0xE3A480, 0x3900);
///*3A00*/Write(f, 16 * 16, 0xE3A880, 0x3A00);
///*3B00*/Write(f, 16 * 16, 0xE3AC80, 0x3B00);
///*3C00*/Write(f, 16 * 16, 0xE3B080, 0x3C00);
///*3D00*/Write(f, 16 * 16, 0xE3B480, 0x3D00);
///*3E00*/Write(f, 16 * 16, 0xE3B880, 0x3E00);
///*3F00*/Write(f, 16 * 16, 0xE3BC80, 0x3F00);
//
//        //4000
//        Write(f, 16 * 13 * 16, 0xE48080, 0x4000); //CJK Unified Ideographs Extension A (ctd.)
//        Write(f, 12 * 16, 0xE4B480, 0x4D00);
//        Write(f, 4 * 16, 0xE4B780, 0x4DC0); //Yijing Hexagram Symbols
//        Write(f, 16 * 2 * 16, 0xE4B880, 0x4E00); //CJK Unified Ideographs


        fclose(f);
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
