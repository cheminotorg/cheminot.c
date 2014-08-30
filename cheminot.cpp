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

Json::Value toJson(std::string value) {
  Json::Value json;
  Json::Reader reader;
  reader.parse(value, json);
  return json;
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
  long size = array.size();
  for(int index=0; index < size; ++index) {
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

TdspVertice parseTdspRow(std::list< std::map<std::string, const void*> >::const_iterator it) {
  std::map<std::string, const void*> row = *it;
  const char*id = (const char*)row["id"];
  const char*name = (const char*)row["name"];
  const char*edges = (const char*)row["edges"];
  const char*stopTimes = (const char*)row["stopTimes"];
  struct TdspVertice tdspVertice;
  tdspVertice.id = id;
  tdspVertice.name = name;
  tdspVertice.edges = parseEdges(toJson(edges));
  tdspVertice.stopTimes = parseStopTimes(toJson(stopTimes));
  return tdspVertice;
}

std::map< std::string, const void*> parseRow(sqlite3_stmt *stmt) {
  int cols = sqlite3_column_count(stmt);
  std::map<std::string, const void*> row;
  for(int col=0 ; col<cols; col++) {
    std::string name(sqlite3_column_name(stmt, col));
    const void *value = sqlite3_column_text(stmt, col);
    row[name] = value;
  }
  printf("\nRow name: %s", (char*) row["name"]);
  printf("\nRow size:  %lu", row.size());
  return row;
}

std::list< std::map<std::string, const void*> > executeSQL(sqlite3 *handle, std::string query) {
  std::list< std::map <std::string, const void*> > results;;
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(handle, "SELECT * FROM TDSP;",-1, &stmt, 0);
  int retval;
  int c = 0;
  while(1) {
    retval = sqlite3_step(stmt);
    if(retval == SQLITE_ROW) {
      results.push_back(parseRow(stmt));
      printf("\n\n########");
      for (std::list< std::map<std::string, const void*> >::const_iterator it = results.begin(), end = results.end(); it != end; ++it) {
        std::map<std::string, const void*> x = *it;
        printf("\nName: %s", x["name"]);
      }
      printf("\n########\n");
      if(c > 1) {
        return results;
      }
    } else if(retval == SQLITE_DONE) {
      return results;
    } else {
      // TODO
      return results;
    }
    c++;
  }
}

sqlite3* openConnection() {
  sqlite3 *handle;
  sqlite3_open_v2("cheminot.db", &handle, SQLITE_OPEN_READONLY, NULL);
  return handle;
}

std::list<TdspVertice> buildTdspGraph(sqlite3 *handle) {
  std::list<TdspVertice> graph;
  std::list< std::map<std::string, const void*> > results = executeSQL(handle, "SELECT * FROM TDSP;");
  // std::vector< std::map<std::string, const void*> > x(std::begin(results), std::end(results));

  // for (std::list< std::map<std::string, const void*> >::const_iterator iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
  //   //try {
  //     TdspVertice tdspVertice = parseTdspRow(iterator);
  //     graph.push_back(tdspVertice);
  //   // } catch(std::runtime_error &e) {
  //   //   printf("Caught a runtime_error exception: %s", e.what());
  //   // }
  // }
  return graph;
}

int main(void) {
  printf("cheminot !");
  sqlite3 *handle = openConnection();
  std::list<TdspVertice> graph = buildTdspGraph(handle);
  std::vector<TdspVertice> vertices(std::begin(graph), std::end(graph));
  printf("\n%lu", vertices.size());
  sqlite3_close(handle);
  return 0;
}
