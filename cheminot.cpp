#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <string>
#include <list>
#include <json/json.h>

struct Stop {
  std::string id;
  std::string name;
  std::string desc;
  double lat;
  double lng;
};

struct StopTime {
  std::string tripId;
  struct tm arrival;
  struct tm departure;
  std::string stopId;
  int index;
};

struct TdspVertice {
  std::string id;
  std::string name;
  std::list<std::string> edges;
  std::list<StopTime> stopTimes;
};

struct tm parseTime(std::string datetime) {
  struct tm tm;
  strptime(datetime.c_str(), "%H:%M", &tm);
  return tm;
}

StopTime parseStopTime(Json::Value value) {
  struct StopTime stopTime;
  stopTime.tripId = value["tripId"].asString();
  stopTime.arrival = parseTime(value["arrival"].asString());
  stopTime.departure = parseTime(value["departure"].asString());
  stopTime.stopId = value["stopId"].asString();
  stopTime.index = value["index"].asInt();
  return stopTime;
}

std::list<StopTime> parseStopTimes(Json::Value array) {
  std::list<StopTime> stopTimes;
  for(int index=0; index < array.size(); index++) {
    StopTime stopTime = parseStopTime(array[index]);
    stopTimes.push_back(stopTime);
  }
  return stopTimes;
}

std::list<std::string> parseEdges(Json::Value array) {
  std::list<std::string> stopIds;
  for(int index=0; index < array.size(); index++) {
    stopIds.push_back(array[index].asString());
  }
  return stopIds;
}

Json::Value toJson(std::string value) {
  Json::Value json;
  Json::Reader reader;
  reader.parse(value, json);
  return json;
}

TdspVertice parseTdspRow(sqlite3_stmt *stmt) {
  const char *id = (const char*)sqlite3_column_text(stmt, 0);
  const char *name = (const char*)sqlite3_column_text(stmt, 1);
  const char *edges = (const char*)sqlite3_column_text(stmt, 2);
  const char *stopTimes = (const char*)sqlite3_column_text(stmt, 3);
  struct TdspVertice tdspVertice;

  tdspVertice.id = id;
  tdspVertice.name = name;
  tdspVertice.edges = parseEdges(toJson(edges));
  tdspVertice.stopTimes = parseStopTimes(toJson(stopTimes));
  return tdspVertice;
}

int main(void) {
  printf("cheminot !");
  int retval;
  sqlite3 *handle;
  retval = sqlite3_open_v2("cheminot.db", &handle, SQLITE_OPEN_READONLY, NULL);
  sqlite3_stmt *stmt;
  retval = sqlite3_prepare_v2(handle, "SELECT * FROM TDSP;",-1, &stmt, 0);
  retval = sqlite3_step(stmt);
  if(retval == SQLITE_ROW) {
    TdspVertice tdspVertice = parseTdspRow(stmt);
    std::vector<std::string> edges(std::begin(tdspVertice.edges), std::end(tdspVertice.edges));
    std::vector<StopTime> stopTimes(std::begin(tdspVertice.stopTimes), std::end(tdspVertice.stopTimes));
    printf("====> %s", stopTimes[0].tripId.c_str());
  } else if(retval == SQLITE_DONE) {
    printf("DONE\n");
  } else {
    printf("An error occured\n");
  }
  sqlite3_close(handle);
  return 0;
}
