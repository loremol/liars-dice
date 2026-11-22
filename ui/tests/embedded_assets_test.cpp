#include <gtest/gtest.h>

#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>

#include "embedded_droid_sans.hpp"

TEST(EmbeddedAssets, DroidSansStartsWithTrueTypeMagic) {
    ASSERT_GE(liars_dice::assets::kDroidSansTtfSize, 4u);
    EXPECT_EQ(liars_dice::assets::kDroidSansTtf[0], 0x00);
    EXPECT_EQ(liars_dice::assets::kDroidSansTtf[1], 0x01);
    EXPECT_EQ(liars_dice::assets::kDroidSansTtf[2], 0x00);
    EXPECT_EQ(liars_dice::assets::kDroidSansTtf[3], 0x00);
}

TEST(EmbeddedAssets, DroidSansMatchesSourceFileByteForByte) {
    std::ifstream file(LIARS_DICE_DROID_SANS_SOURCE, std::ios::binary);
    ASSERT_TRUE(file.is_open());
    const std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)),
                                           std::istreambuf_iterator<char>());
    ASSERT_EQ(bytes.size(), liars_dice::assets::kDroidSansTtfSize);
    EXPECT_TRUE(std::equal(bytes.begin(), bytes.end(), liars_dice::assets::kDroidSansTtf));
}
