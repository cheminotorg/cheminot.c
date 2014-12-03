#include <cheminotc.h>
#include "../protobuf/cheminotBuf.pb.h"
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
  cheminotc::Graph graph;
  cheminotc::CalendarDates calendarDates;

  virtual void SetUp() {
    std::string path = "cheminot.db";
    handle = cheminotc::openConnection(path);
    cheminotc::parseGraph("graph", &graph);
    cheminotc::parseCalendarDates("calendar_dates", &calendarDates);
  }
};

TEST(GraphParsing, parse_graph) {
  std::string chartresStop = "StopPoint:OCETrain TER-87394007";
  cheminotc::Graph graph;
  cheminotc::parseGraph("graph", &graph);
  EXPECT_STREQ(chartresStop.c_str(), graph[chartresStop].id().c_str());
}

TEST(CalendarDatesParsing, parse_calendarDates) {
  std::string serviceId = "7007";
  cheminotc::CalendarDates calendarDates;
  cheminotc::parseCalendarDates("calendar_dates", &calendarDates);
  auto exceptions = calendarDates[serviceId];
  auto x = exceptions.calendardates();
  auto y = x.Get(0);
  EXPECT_STREQ(serviceId.c_str(), y.serviceid().c_str());
}

TEST_F(SqliteFixture, get_version) {
  std::string version = cheminotc::getVersion(handle);
  EXPECT_STREQ("MjAxNC0xMC0wM18wOS0yOS0wMA==", version.c_str());
}

TEST_F(SqliteFixture, get_trip) {
  std::string tripId = "OCESN026208F0100133422";
  auto trips = cheminotc::getTripsByIds(handle, { tripId });
  auto trip = trips.begin();
  EXPECT_STREQ(tripId.c_str(), trip->id.c_str());
  EXPECT_STREQ("1", trip->calendar->serviceId.c_str());
  EXPECT_STREQ("7/9/114", cheminotc::formatDate(trip->calendar->startDate).c_str());
}

TEST_F(GraphFixture, chartresParis) {
  std::string chartres = "StopPoint:OCETrain TER-87394007";
  std::string paris = "StopPoint:OCETrain TER-87391003";
  struct tm ts = cheminotc::getNow();
  ts.tm_hour = 6;
  ts.tm_min = 30;
  std::time_t start = std::time(nullptr);
  auto results = cheminotc::lookForBestTrip(handle, &graph, &calendarDates, chartres, paris, ts);
  auto end = std::time(nullptr);
  std::time_t duration = end - start;
  printf("\n %lu %lu %lu", start, end, duration);
  for (std::list<cheminotc::ArrivalTime>::const_iterator iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
    printf("\n%s - %i:%i", iterator->stopId.c_str() ,iterator->departure.tm_hour, iterator->departure.tm_min);
  }
  EXPECT_EQ(10, results.size());
}
