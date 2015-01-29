#include "../src/cheminotc.h"
#include "../src/protobuf/cheminotBuf.pb.h"
#include <sqlite3.h>

int main(int argc, char **argv) {
  std::string start = "StopPoint:OCETrain TER-87478404";
  std::string end = "StopPoint:OCETrain TER-87391003";
  struct tm ts = cheminotc::getNow();
  ts.tm_hour = 0;
  ts.tm_min = 0;
  tm te = cheminotc::getNow();

  te.tm_hour = 12;
  te.tm_min = 0;

  sqlite3 *handle = cheminotc::openConnection("/Users/sre/data/Projects/me/cheminot.c/cheminot.db");
  cheminotc::Graph graph;
  cheminotc::CalendarDates calendarDates;
  cheminotc::parseGraph("/Users/sre/data/Projects/me/cheminot.c/graph", &graph);
  cheminotc::parseCalendarDates("/Users/sre/data/Projects/me/cheminot.c/calendar_dates", &calendarDates);
  cheminotc::VerticesCache verticesCache;
  cheminotc::TripsCache tripsCache;
  cheminotc::CalendarDatesCache calendarDatesCache;
  cheminotc::Tracker tracker = { false };
  auto results = cheminotc::lookForBestTrip(&tracker, handle, &graph, &tripsCache, &verticesCache, &calendarDates, &calendarDatesCache, start, end, ts, te, 1);

  for (auto iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
    printf("%s - %s\n", iterator->stopId.c_str() , cheminotc::formatDateTime(iterator->arrival).c_str());
  }
}
