#include <cheminotc.h>
#include <sqlite3.h>
#include <gtest/gtest.h>
#include <string>

class CheminotcTest : public testing::Test {
  protected:

  sqlite3 *handle;

  virtual void SetUp() {
    std::string path = "cheminot.db";
    handle = cheminotc::openConnection(path);
  }
};

TEST_F(CheminotcTest, sample_test) {
  std::string version = cheminotc::getVersion(handle);
  EXPECT_STREQ(version.c_str(), "MjAxNC0xMC0wM18wOS0yOS0wMA==");
}
