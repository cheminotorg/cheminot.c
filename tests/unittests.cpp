#include <cheminotc.h>
#include "../src/protobuf/cheminotBuf.pb.h"
#include <sqlite3.h>
#include <gtest/gtest.h>
#include <string>
#include <numeric>

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

// TEST(GraphParsing, parse_graph) {
//   std::string chartresStop = "StopPoint:OCETrain TER-87394007";
//   cheminotc::Graph graph;
//   cheminotc::parseGraph("graph", &graph);
//   EXPECT_STREQ(chartresStop.c_str(), graph[chartresStop].id().c_str());
// }

// TEST(CalendarDatesParsing, parse_calendarDates) {
//   std::string serviceId = "7007";
//   cheminotc::CalendarDates calendarDates;
//   cheminotc::parseCalendarDates("calendar_dates", &calendarDates);
//   auto exceptions = calendarDates[serviceId];
//   auto x = exceptions.calendardates();
//   auto y = x.Get(0);
//   EXPECT_STREQ(serviceId.c_str(), y.serviceid().c_str());
// }

// TEST_F(SqliteFixture, get_version) {
//   std::string version = cheminotc::getVersion(handle);
//   EXPECT_STREQ("MjAxNC0xMC0wM18wOS0yOS0wMA==", version.c_str());
// }

// TEST_F(SqliteFixture, get_trip) {
//   std::string tripId = "OCESN026208F0100133422";
//   auto trips = cheminotc::getTripsByIds(handle, { tripId });
//   auto trip = trips.begin();
//   EXPECT_STREQ(tripId.c_str(), trip->id.c_str());
//   EXPECT_STREQ("1", trip->calendar->serviceId.c_str());
//   EXPECT_STREQ("7/9/114", cheminotc::formatDate(trip->calendar->startDate).c_str());
// }

// TEST_F(GraphFixture, chartresParisAs6h30) {
//   std::string chartres = "StopPoint:OCETrain TER-87394007";
//   std::string paris = "StopPoint:OCETrain TER-87391003";
//   tm ts = cheminotc::getNow();
//   ts.tm_hour = 8;
//   ts.tm_min = 45;
//   tm te = cheminotc::getNow();
//   te.tm_hour = 9;
//   te.tm_min = 45;
//   auto results = cheminotc::lookForBestTrip(handle, &graph, &calendarDates, chartres, paris, ts, te);
//   for (auto iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
//     printf("%s - %s\n", iterator->stopId.c_str() , cheminotc::formatDateTime(iterator->arrival).c_str());
//   }
//   EXPECT_EQ(true, true);
// }

// TEST(datetime, datetimeIsBeforeEq) {
//   tm a = cheminotc::getNow();
//   tm b = cheminotc::getNow();

//   a.tm_mday = 1;
//   a.tm_hour = 11;

//   b.tm_mday = 2;
//   b.tm_hour = 9;

//   printf("%s %s\n", cheminotc::formatDateTime(a).c_str(), cheminotc::formatDateTime(b).c_str());
//   EXPECT_EQ(true, cheminotc::timeIsBeforeEq(b, a));
//   EXPECT_EQ(true, cheminotc::dateIsBeforeEq(a, b));
//   EXPECT_EQ(true, cheminotc::datetimeIsBeforeEq(a, b));

//   a.tm_mday = 1;
//   a.tm_hour = 8;
//   b.tm_mday = 1;
//   b.tm_hour = 9;

//   printf("%s %s\n", cheminotc::formatDateTime(a).c_str(), cheminotc::formatDateTime(b).c_str());
//   EXPECT_EQ(true, cheminotc::datetimeIsBeforeEq(a, b));

//   a.tm_mday = 1;
//   a.tm_hour = 8;
//   b.tm_mday = 1;
//   b.tm_hour = 8;

//   printf("%s %s\n", cheminotc::formatDateTime(a).c_str(), cheminotc::formatDateTime(b).c_str());
//   EXPECT_EQ(true, cheminotc::datetimeIsBeforeEq(a, b));

//   EXPECT_EQ(false, cheminotc::datetimeIsBeforeNotEq(a, b));
// }

TEST_F(GraphFixture, chartresParisAs6h30) {
  std::string chartres = "StopPoint:OCETrain TER-87394007";
  std::string paris = "StopPoint:OCETrain TER-87391003";
  struct tm ts = cheminotc::getNow();
  ts.tm_hour = 6;
  ts.tm_min = 0;
  printf("-----------------> %s\n", cheminotc::formatDateTime(ts).c_str());
  tm te = cheminotc::addHours(ts, 10);
  printf("-----------------> %s\n", cheminotc::formatDateTime(te).c_str());
  cheminotc::VerticesCache verticesCache;
  cheminotc::TripsCache tripsCache;
  cheminotc::CalendarDatesCache calendarDatesCache;
  auto results = cheminotc::lookForBestDirectTrip(handle, &graph, &verticesCache, &tripsCache, &calendarDates, &calendarDatesCache, chartres, paris, ts, te);
  for (auto iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
    printf("%s - %s - %s || %s\n", iterator->stopId.c_str() , iterator->tripId.c_str(), cheminotc::formatDateTime(iterator->arrival).c_str(), cheminotc::formatDateTime(iterator->departure).c_str());
  }
  EXPECT_EQ(true, true);
}

// TEST_F(GraphFixture, chartresParisAs6h30) {
//   std::string chartres = "StopPoint:OCETrain TER-87478404";
//   std::string paris = "StopPoint:OCETrain TER-87391003";
//   struct tm ts = cheminotc::getNow();
//   ts.tm_hour = 0;
//   ts.tm_min = 0;
//   ts.tm_mday = 29;
//   printf("-----------------> %s\n", cheminotc::formatDateTime(ts).c_str());
//   tm te = cheminotc::addHours(ts, 10);
//   printf("-----------------> %s\n", cheminotc::formatDateTime(te).c_str());
//   cheminotc::VerticesCache verticesCache;
//   cheminotc::TripsCache tripsCache;
//   cheminotc::CalendarDatesCache calendarDatesCache;
//   cheminotc::Tracker tracker = { false };
//   auto results = cheminotc::lookForBestTrip(&tracker, handle, &graph, &tripsCache, &verticesCache, &calendarDates, &calendarDatesCache, chartres, paris, ts, te, 1);
//   for (auto iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
//     printf("%s - %s - %s || %s\n", iterator->stopId.c_str() , iterator->tripId.c_str(), cheminotc::formatDateTime(iterator->arrival).c_str(), cheminotc::formatDateTime(iterator->departure).c_str());
//   }
//   EXPECT_EQ(true, true);
// }

// TEST_F(GraphFixture, chartresParis) {
//   std::string chartres = "StopPoint:OCETrain TER-87394007";
//   std::string paris = "StopPoint:OCETrain TER-87391003";

//   struct tm today = cheminotc::getNow();
//   today.tm_hour = 0;
//   today.tm_min = 0;

//   struct tm ts = today;
//   struct tm te = cheminotc::addMinutes(ts, 30);

//   while(ts.tm_mday < (today.tm_mday + 1)) {
//     printf("\n################################################# \n");
//     printf("###> %s %s\n", cheminotc::formatDateTime(ts).c_str(), cheminotc::formatDateTime(te).c_str());
//     auto results = cheminotc::lookForBestTrip(handle, &graph, &calendarDates, chartres, paris, ts, te, 1);
//     for (auto iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
//       printf("%s %s - %s\n", iterator->stopId.c_str() , iterator->tripId.c_str(), cheminotc::formatDateTime(iterator->departure).c_str());
//     }
//     if(results.empty()) {
//       ts = te;
//     } else {
//       ts = cheminotc::addMinutes(results.begin()->departure, 1);
//     }
//     te = cheminotc::addMinutes(ts, 30);
//   }
//   EXPECT_EQ(true, true);
// }
