#include <cheminotc.h>
#include <sqlite3.h>
#include <gtest/gtest.h>
#include <string>

class SqliteFixture : public testing::Test {
  protected:

  sqlite3 *handle;

  virtual void SetUp() {
    std::string path = "cheminot.db";
    handle = cheminotc::openConnection(path);
  }
};

class GraphFixture : public testing::Test {
  protected:

  sqlite3 *handle;
  Json::Value graph;
  Json::Value datesExceptions;

  virtual void SetUp() {
    std::string path = "cheminot.db";
    handle = cheminotc::openConnection(path);
    graph = cheminotc::parseGraph("graph.json");
    datesExceptions = cheminotc::parseCalendarExceptions("date_exceptions.json");
  }
};

TEST_F(SqliteFixture, get_version) {
  std::string version = cheminotc::getVersion(handle);
  EXPECT_STREQ(version.c_str(), "MjAxNC0xMC0wM18wOS0yOS0wMA==");
}

TEST_F(GraphFixture, chartresParis) {
  std::string chartres = "StopPoint:OCETrain TER-87394007";
  std::string paris = "StopPoint:OCETrain TER-87391003";
  struct tm ts = cheminotc::getNow();
  ts.tm_hour = 6;
  ts.tm_min = 30;
  std::time_t start = std::time(nullptr);
  auto results = cheminotc::lookForBestTrip(handle, &graph, &datesExceptions, chartres, paris, ts);
  auto end = std::time(nullptr);
  std::time_t duration = end - start;
  printf("\n %lu %lu %lu", start, end, duration);
  for (std::list<cheminotc::ArrivalTime>::const_iterator iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
    printf("\n%s - %i:%i", iterator->stopId.c_str() ,iterator->departure.tm_hour, iterator->departure.tm_min);
  }
  EXPECT_EQ(results.size(), 10);
}
