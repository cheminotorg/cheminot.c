#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include <string>
#include <list>
#include <queue>
#include <numeric>
#include <json/json.h>

struct StopTime {
  std::string tripId;
  struct tm arrival;
  struct tm departure;
  std::string stopId;
  int index;
};

struct Calendar {
  std::string serviceId;
  bool monday;
  bool tuesday;
  bool wednesday;
  bool thursday;
  bool friday;
  bool saturday;
  bool sunday;
  struct tm startDate;
  struct tm endDate;
};

struct Trip {
  std::string id;
  Calendar calendar;
  std::string direction;
};

struct Stop {
  std::string id;
  std::string name;
  double lat;
  double lng;
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

struct tm* asDateTime(time_t t) {
  struct tm dateTime;
  return localtime (&t);
}

time_t asTimestamp(struct tm a) {
  return mktime(&a);
}

bool hasSameTime(struct tm *a, struct tm *b) {
  return (a->tm_hour == b->tm_hour) && (a->tm_min == b->tm_min);
}

struct tm INFINI() {
  struct tm *infini = asDateTime(time(0));
  infini->tm_year = 9999;
  return *infini;
}

bool isInfini(struct tm *t) {
  return t->tm_year == 9999;
}

bool compareTime(struct tm a, struct tm b) {
  if(a.tm_hour > b.tm_hour) {
    return false;
  } else if(a.tm_hour < b.tm_hour) {
    return true;
  } else {
    if(a.tm_min > b.tm_min) {
      return false;
    } else {
      return true;
    }
  }
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
  stopTime.arrival = parseTime(value["arrivalTime"].asString());
  stopTime.departure = parseTime(value["departureTime"].asString());
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

Calendar parseCalendar(Json::Value value) {
  struct Calendar calendar;
  calendar.serviceId = value["serviceId"].asString();
  calendar.monday = value["monday"].asString() == "1";
  calendar.tuesday = value["tuesday"].asString() == "1";
  calendar.wednesday = value["wednesday"].asString() == "1";
  calendar.thursday = value["thursday"].asString() == "1";
  calendar.friday = value["friday"].asString() == "1";
  calendar.saturday = value["saturday"].asString() == "1";
  calendar.sunday = value["sunday"].asString() == "1";
  calendar.startDate = parseTime(value["startDate"].asString());
  calendar.endDate = parseTime(value["endDate"].asString());
  return calendar;
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

Trip parseTripRow(std::list< std::map<std::string, const void*> >::const_iterator it) {
  std::map<std::string, const void*> row = *it;
  const char*id = (const char*)row["id"];
  const char*calendar = (const char*)row["service"];
  const char*direction = (const char*)row["direction"];
  struct Trip trip;
  trip.id = id;
  if(calendar) {
    trip.calendar = parseCalendar(toJson(calendar));
  }
  trip.direction = direction;
  return trip;
}

std::map< std::string, const void*> parseRow(sqlite3_stmt *stmt) {
  int cols = sqlite3_column_count(stmt);
  std::map<std::string, const void*> row;
  for(int col=0 ; col<cols; col++) {
    std::string name(sqlite3_column_name(stmt, col));
    row[name] = strdup((const char *)sqlite3_column_text(stmt, col));
  }
  return row;
}

std::list< std::map<std::string, const void*> > executeSQL(sqlite3 *handle, std::string query) {
  std::list< std::map <std::string, const void*> > results;
  sqlite3_stmt *stmt;
  sqlite3_prepare_v2(handle, query.c_str(),-1, &stmt, 0);
  int retval;
  while(1) {
    retval = sqlite3_step(stmt);
    if(retval == SQLITE_ROW) {
      results.push_back(parseRow(stmt));
    } else if(retval == SQLITE_DONE) {
      return results;
    } else {
      throw std::runtime_error("Unexpected error while executing this SQL query: " + query);
    }
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
  for (std::list< std::map<std::string, const void*> >::const_iterator iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
    graph.push_back(parseTdspRow(iterator));
  }
  return graph;
}

TdspVertice getTdspVerticeById(sqlite3 *handle, std:: string id) {
  std::string query = "SELECT * FROM TDSP WHERE id = '" + id +"'";
  std::list< std::map<std::string, const void*> > results = executeSQL(handle, query);
  return parseTdspRow(results.begin());
}

std::string getStopsTree(sqlite3 *handle) {
  std::string query = "SELECT value FROM CACHE WHERE key = 'stopsTree'";
  std::list< std::map<std::string, const void*> > results = executeSQL(handle, query);
  char *stopsTree = (char *)results.front()["value"];
  return stopsTree;
}

std::list<Trip> getTripsByIds(sqlite3 *handle, std::list<std::string> ids) {
  std::list<Trip> trips;
  std::string params = std::accumulate(ids.begin(), ids.end(), (std::string)"", [](std::string acc, std::string id) {
    std::string p = "id = '" + id + "'";
    return (acc == "") ? p : (acc + " OR " + p);
  });
  if(params != "") {
    std::string query = "SELECT * FROM TRIPS WHERE " + params;
    std::list< std::map<std::string, const void*> > results = executeSQL(handle, query);
    for (std::list< std::map<std::string, const void*> >::const_iterator iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
      trips.push_back(parseTripRow(iterator));
    }
  }
  return trips;
}

struct PQueueItem {
  std::string stopId;
  struct tm arrival;
  struct tm departure;
  std::string tripId;
  TdspVertice *vertice;

  bool operator < (const PQueueItem& x) const {
    return compareTime(arrival, x.arrival);
  }
};

std::tuple<std::priority_queue<PQueueItem>, std::map<std::string, TdspVertice*>> initialize(sqlite3 *handle, std::list<TdspVertice> *vertices, std::string vsId, struct tm ts) {
  std::priority_queue<PQueueItem> queue;
  std::map<std::string, TdspVertice*> indexed;

  TdspVertice vs = getTdspVerticeById(handle, vsId);
  auto isStartTime = [&] (StopTime stopTime) {
    return hasSameTime(&stopTime.departure, &ts);
  };
  StopTime vsStopTime = (*std::find_if (vs.stopTimes.begin(), vs.stopTimes.end(), isStartTime));
  printf("\nTripId %s", vsStopTime.tripId.c_str());

  struct PQueueItem vsQueueItem;
  vsQueueItem.stopId = vsId;
  vsQueueItem.arrival = vsStopTime.arrival;
  vsQueueItem.departure = vsStopTime.departure;
  vsQueueItem.tripId = vsStopTime.tripId;
  queue.push(vsQueueItem);

  for (std::list<TdspVertice>::const_iterator iterator = (*vertices).begin(), end = (*vertices).end(); iterator != end; ++iterator) {
    TdspVertice vertice = *iterator;
    indexed[vsId] = &vertice;
    if(vertice.id != vsId) {
      struct PQueueItem item;
      item.stopId = vertice.id;
      item.arrival = INFINI();
      item.departure = INFINI();
      item.vertice = &vertice;
      queue.push(item);
    }
  }
  return std::tuple<std::priority_queue<PQueueItem>, std::map<std::string, TdspVertice*>> (queue, indexed);
}

std::map<std::string, PQueueItem> refineArrivalTimes(sqlite3 *handle, std::list<TdspVertice> *vertices, std::priority_queue<PQueueItem> *queue, std::map<std::string, TdspVertice*> *indexed, std::string veId) {
  std::map<std::string, PQueueItem> results;
  while(!queue->empty()) {
    PQueueItem head = queue->top();
    queue->pop();
    results[head.stopId] = head;
    if(isInfini(&head.arrival)) {
      throw std::runtime_error("break");
    } else if(head.stopId == veId) {
      return results;
    } else {
      TdspVertice vi = (*(*indexed)[head.stopId]);
      vi.stopTimes.remove_if([&] (const StopTime& stopTime) {
        return compareTime(stopTime.departure, head.departure);
      });
      vi.stopTimes.sort([](const StopTime& first, const StopTime& second) {
        return compareTime(first.departure, second.departure);
      });
    }
  };
  return results;
}

int main(void) {
  printf("cheminot !");
  sqlite3 *handle = openConnection();

  std::list<TdspVertice> graph = buildTdspGraph(handle);

  const time_t t = time(0);
  struct tm *ts = localtime(&t);
  ts->tm_hour = 7;
  ts->tm_min = 57;

  auto initialized = initialize(handle, &graph, "StopPoint:OCETrain TER-87394007", *ts);
  std::priority_queue<PQueueItem> queue = std::get<0>(initialized);
  std::map<std::string, TdspVertice*> indexed = std::get<1>(initialized);
  refineArrivalTimes(handle, &graph, &queue, &indexed, "StopPoint:OCETrain TER-87391003");

  sqlite3_close(handle);
  return 0;
}
