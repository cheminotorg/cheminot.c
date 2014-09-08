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
};

struct Calendar {
  std::string serviceId;
  std::map<std::string, bool> week;
  struct tm startDate;
  struct tm endDate;
};

struct CalendarException {
  std::string serviceId;
  struct tm date;
  int exceptionType;
};

struct Trip {
  std::string id;
  std::unique_ptr<Calendar> calendar;
  std::string direction;
};

struct Stop {
  std::string id;
  std::string name;
  double lat;
  double lng;
};

struct Vertice {
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

struct tm asDateTime(time_t t) {
  struct tm dateTime;
  return *(localtime (&t));
}

time_t asTimestamp(struct tm a) {
  return mktime(&a);
}

bool hasSameTime(struct tm *a, struct tm *b) {
  return (a->tm_hour == b->tm_hour) && (a->tm_min == b->tm_min);
}

bool timeIsBefore(struct tm a, struct tm b) {
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

std::map<std::string, std::list<CalendarException>> parseCalendarExceptions(Json::Value json) {
  std::map<std::string, std::list<CalendarException>> calendarExceptions;
  for(auto const &serviceId : json.getMemberNames()) {
    std::list<CalendarException> exceptions;
    Json::Value array = json[serviceId];
    for(int index=0; index < array.size(); ++index) {
      CalendarException calendarException;
      Json::Value value = array[index];
      calendarException.serviceId = value["serviceId"].asString();
      calendarException.date = parseTime(value["date"].asString());
      calendarException.exceptionType = value["exceptionType"].asInt();
      exceptions.push_back(calendarException);
    }
    calendarExceptions[serviceId] = exceptions;
  }
  return calendarExceptions;
}

StopTime parseStopTime(Json::Value value) {
  struct StopTime stopTime;
  stopTime.tripId = value["tripId"].asString();
  stopTime.arrival = parseTime(value["arrivalTime"].asString()); //TODO
  stopTime.departure = parseTime(value["departureTime"].asString()); //TODO
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

std::unique_ptr<Calendar> parseCalendar(Json::Value value) {
  std::unique_ptr<Calendar> calendar(new Calendar());
  std::map<std::string, bool> week;
  week["monday"] = value["monday"].asString() == "1";
  week["tuesday"] = value["tuesday"].asString() == "1";
  week["wednesday"] = value["wednesday"].asString() == "1";
  week["thursday"] = value["thursday"].asString() == "1";
  week["friday"] = value["friday"].asString() == "1";
  week["saturday"] = value["saturday"].asString() == "1";
  week["sunday"] = value["sunday"].asString() == "1";

  calendar->serviceId = value["serviceId"].asString();
  calendar->week = week;
  calendar->startDate = parseTime(value["startDate"].asString());
  calendar->endDate = parseTime(value["endDate"].asString());
  return calendar;
}

Vertice parseVerticeRow(std::list< std::map<std::string, const void*> >::const_iterator it) {
  std::map<std::string, const void*> row = *it;
  const char*id = (const char*)row["id"];
  const char*name = (const char*)row["name"];
  const char*edges = (const char*)row["edges"];
  const char*stopTimes = (const char*)row["stopTimes"];
  struct Vertice vertice;
  vertice.id = id;
  vertice.name = name;
  vertice.edges = parseEdges(toJson(edges));
  vertice.stopTimes = parseStopTimes(toJson(stopTimes));
  return vertice;
}

Trip parseTripRow(std::list< std::map<std::string, const void*> >::const_iterator it) {
  std::map<std::string, const void*> row = *it;
  const char *id = (const char*)row["id"];
  const char *calendar = (const char*)row["service"];
  const char *direction = (const char*)row["direction"];
  struct Trip trip;
  trip.id = id;
  if(calendar != NULL) {
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

std::map<std::string, Vertice> buildGraph(sqlite3 *handle) {
  std::map<std::string, Vertice> graph;
  std::list< std::map<std::string, const void*> > results = executeSQL(handle, "SELECT * FROM TDSP;");
  for (std::list< std::map<std::string, const void*> >::const_iterator iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
    Vertice vertice = parseVerticeRow(iterator);
    graph[vertice.id] = vertice;
  }
  return graph;
}

Vertice getVerticeById(sqlite3 *handle, std:: string id) {
  std::string query = "SELECT * FROM TDSP WHERE id = '" + id +"'";
  std::list< std::map<std::string, const void*> > results = executeSQL(handle, query);
  return parseVerticeRow(results.begin());
}

std::string getStopsTree(sqlite3 *handle) {
  std::string query = "SELECT value FROM CACHE WHERE key = 'stopsTree'";
  std::list< std::map<std::string, const void*> > results = executeSQL(handle, query);
  char *stopsTree = (char *)results.front()["value"];
  return stopsTree;
}

std::map<std::string, std::list<CalendarException>> getCalendarExceptions(sqlite3 *handle) {
  std::string query = "SELECT value FROM CACHE WHERE key = 'exceptions'";
  std::list< std::map<std::string, const void*> > results = executeSQL(handle, query);
  char *exceptions = (char *)results.front()["value"];
  return parseCalendarExceptions(toJson(exceptions));
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
  Vertice *vertice;
};

class CompareArrivalTime {
public:
  bool operator()(const PQueueItem& gi, const PQueueItem& gj) {
    return timeIsBefore(gj.departure, gi.departure);
  }
};

bool isTripRemovedOn(std::list<Trip>::const_iterator trip, std::map<std::string, std::list<CalendarException>> *calendarExceptions, struct tm when) {
  auto exceptions = calendarExceptions->find(trip->calendar->serviceId);
  if(exceptions != calendarExceptions->end()) {
    auto it = std::find_if(exceptions->second.begin(), exceptions->second.end(), [&](CalendarException exception) {
      return exception.date.tm_wday == when.tm_wday && exception.exceptionType == 2;
    });
    return it != exceptions->second.end();
  } else {
    return false;
  }
}

bool isTripAddedOn(std::list<Trip>::const_iterator trip, std::map<std::string, std::list<CalendarException>> *calendarExceptions, struct tm when) {
  auto exceptions = calendarExceptions->find(trip->calendar->serviceId);
  if(exceptions != calendarExceptions->end()) {
    auto it = std::find_if(exceptions->second.begin(), exceptions->second.end(), [&](CalendarException exception) {
      return exception.date.tm_wday == when.tm_wday && exception.exceptionType == 1;
    });
    return it != exceptions->second.end();
  } else {
    return false;
  }
}

bool isTripValidToday(std::list<Trip>::const_iterator trip, struct tm when) {
  std::map<int, std::string> week { {1, "monday"}, {2, "tuesday"}, {3, "wednesday"}, {4, "thursday"}, {5, "friday"}, {6, "saturday"}, {0, "sunday"}};
  return trip->calendar->week[week[when.tm_wday]];
}

bool isTripInPeriod(Trip *trip, struct tm when) {
  struct tm startDate = trip->calendar->startDate;
  struct tm endDate = trip->calendar->endDate;
  bool before = (asTimestamp(startDate) < asTimestamp(when) || startDate.tm_wday == when.tm_wday);
  bool after = (asTimestamp(startDate) > asTimestamp(when) || endDate.tm_wday == when.tm_wday);
  return before && after;
}

bool isTripValidOn(std::list<Trip>::const_iterator trip, std::map<std::string, std::list<CalendarException>> *calendarExceptions, struct tm when) {
  if(trip->calendar != NULL) {
    bool removed = isTripRemovedOn(trip, calendarExceptions, when);
    bool added = isTripAddedOn(trip, calendarExceptions, when);
    bool availableToday = isTripValidToday(trip, when);
    // bool inPeriod = isTripInPeriod(trip, when);
    //return (!removed && availableToday) || added;
    return true;
  }
  return false;
}

std::map<std::string, bool> tripsAvailability(sqlite3 *handle, std::list<std::string> ids, std::map<std::string, std::list<CalendarException>> *calendarExceptions, struct tm when) {
  std::map<std::string, bool> availablities;
  auto trips = getTripsByIds(handle, ids);
  for (std::list<Trip>::const_iterator iterator = trips.begin(), end = trips.end(); iterator != end; ++iterator) {
    availablities[iterator->id] = isTripValidOn(iterator, calendarExceptions, when);
  }
  return availablities;
}

std::list<StopTime> getAvailableDepartures(sqlite3 *handle, std::map<std::string, std::list<CalendarException>> *calendarExceptions, PQueueItem *vi, struct tm ts) {

  std::list<StopTime> departures(vi->vertice->stopTimes);
  departures.remove_if([&] (const StopTime& stopTime) {
    return timeIsBefore(stopTime.departure, vi->arrival);
  });

  std::list<std::string> tripIds;
  for (std::list<StopTime>::const_iterator iterator = departures.begin(), end = departures.end(); iterator != end; ++iterator) {
    tripIds.push_back(iterator->tripId);
  }

  auto availablities = tripsAvailability(handle, tripIds, calendarExceptions, ts);

  departures.remove_if([&] (const StopTime& stopTime) {
    return !availablities[stopTime.tripId];
  });

  departures.sort([](const StopTime& first, const StopTime& second) {
    return timeIsBefore(first.departure, second.departure);
  });

  return departures;
}

std::map<std::string, PQueueItem> refineArrivalTimes(sqlite3 *handle, std::map<std::string, Vertice> *graph, std::map<std::string, std::list<CalendarException>> *calendarExceptions, std::string vsId, std::string veId, struct tm ts) {
  std::map<std::string, PQueueItem> results;
  std::priority_queue<PQueueItem, std::vector<PQueueItem>, CompareArrivalTime> queue;
  std::map<std::string, PQueueItem*> visited;

  // Starting
  Vertice vs = (*graph)[vsId];

  StopTime stopTimeVs = (*std::find_if (vs.stopTimes.begin(), vs.stopTimes.end(), [&] (StopTime stopTime) {
    return hasSameTime(&stopTime.departure, &ts);
  }));

  struct PQueueItem vsItem;
  vsItem.stopId = vsId;
  vsItem.arrival = stopTimeVs.arrival;
  vsItem.departure = stopTimeVs.departure;
  vsItem.tripId = stopTimeVs.tripId;
  vsItem.vertice = &vs;

  queue.push(vsItem);

  // OK, Let's go !

  while(!queue.empty()) {
    printf("\nWHILE");
    PQueueItem head = queue.top();
    queue.pop();
    visited[head.stopId] = &head;
    results[head.stopId] = head;
    printf("\n%lu -> %s -> %s -> %i : %i", queue.size(), head.stopId.c_str(), head.tripId.c_str(), head.departure.tm_hour, head.departure.tm_min);
    if(head.stopId == veId) {
      printf("\nDONE!");
      return results;
    } else {
      Vertice vi = *(head.vertice);
      auto departures = getAvailableDepartures(handle, calendarExceptions, &head, ts);
      for (std::list<std::string>::const_iterator iterator = vi.edges.begin(), end = vi.edges.end(); iterator != end; ++iterator) {
        std::string vjId = *iterator;
        Vertice *vj = &(*graph)[vjId];
        std::list<StopTime> stopTimes(vj->stopTimes);
        stopTimes.remove_if([&] (const StopTime& stopTime) {
          if(vsId == head.stopId) {
            return !((vsItem.tripId == stopTime.tripId) && timeIsBefore(ts, stopTime.arrival));
          } else {
            auto it = std::find_if(departures.begin(), departures.end(), [&](StopTime dt) {
              return (stopTime.tripId == dt.tripId) && timeIsBefore(dt.departure, stopTime.arrival);
            });
            return it == departures.end();
          }
        });

        printf("\n%s => %lu", vjId.c_str(), stopTimes.size());

        stopTimes.sort([](const StopTime& first, const StopTime& second) {
          return timeIsBefore(first.departure, second.departure);
        });

        if(!stopTimes.empty()) {
          StopTime next = stopTimes.front();
          if(visited[vjId] == NULL) {
            struct PQueueItem item;
            item.stopId = vjId;
            item.arrival = next.arrival;
            item.departure = next.departure;
            item.tripId = next.tripId;
            item.vertice = vj;
            queue.push(item);
          }
        }
      }
    }
  };
  return results;
}

std::list<PQueueItem> pathSelection(std::map<std::string, Vertice> *graph, std::map<std::string, PQueueItem> *arrivalTimes, struct tm ts, std::string vsId, std::string veId) {
  Vertice vs = (*graph)[vsId];
  Vertice vj = (*graph)[veId];
  PQueueItem ge = (*arrivalTimes)[vj.id];
  std::list<PQueueItem> path;
  printf("\n**PATH SELECTION**");
  while(vj.id != vs.id) {
    PQueueItem gj = (*arrivalTimes)[vj.id];
    printf("\nWHILE %s - %s - %i:%i", vj.id.c_str(), gj.tripId.c_str(), gj.departure.tm_hour, gj.departure.tm_min);
    for (std::list<std::string>::const_iterator iterator = vj.edges.begin(), end = vj.edges.end(); iterator != end; ++iterator) {
      std::string viId = *iterator;
      Vertice vi = (*graph)[viId];
      auto gi = arrivalTimes->find(vi.id);
      if(gi != arrivalTimes->end()) {
        if(timeIsBefore(gi->second.departure, gj.arrival)) {
          vj = vi;
          path.push_front(gj);
          break;
        }
      }
    }
  }

  return path;
}

int main(void) {
  printf("cheminot !");
  sqlite3 *handle = openConnection();

  const time_t startTime = time(0);
  struct tm *ts = localtime(&startTime);
  ts->tm_hour = 7;
  ts->tm_min = 57;

  std::string vsId = "StopPoint:OCETrain TER-87394007";
  std::string veId = "StopPoint:OCETrain TER-87391003";

  std::map<std::string, Vertice> graph = buildGraph(handle);
  auto calendarExceptions = getCalendarExceptions(handle);
  auto arrivalTimes = refineArrivalTimes(handle, &graph, &calendarExceptions, vsId, veId, *ts);
  pathSelection(&graph, &arrivalTimes, *ts, vsId, veId);

  sqlite3_close(handle);
  return 0;
}
