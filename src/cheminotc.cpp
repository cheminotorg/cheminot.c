#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <list>
#include <queue>
#include <numeric>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <memory>
#include "cheminotc.h"

namespace cheminotc {

  template <typename T>
  std::string to_string(T value) {
    std::ostringstream os ;
    os << value ;
    return os.str() ;
  }

  struct tm getNow() {
    time_t rawtime;
    time(&rawtime);
    struct tm *info = gmtime(&rawtime);
    return *info;
  }

  std::string formatTime(struct tm time) {
    return to_string(time.tm_hour) + ":" + to_string(time.tm_min);
  }

  std::string formatDate(struct tm time) {
    return to_string(time.tm_mday) + "/" + to_string(time.tm_mon) + "/" + to_string(time.tm_year);
  }

  std::string formatDateTime(struct tm datetime) {
    return formatDate(datetime) + " " + formatTime(datetime);
  }

  struct tm parseTime(std::string datetime) {
    struct tm now = getNow();
    struct tm time;
    strptime(datetime.c_str(), "%H:%M", &time);
    now.tm_hour = time.tm_hour;
    now.tm_min = time.tm_min;
    now.tm_sec = 0;
    return now;
  }

  struct tm parseDate(std::string datetime) {
    struct tm now = getNow();
    struct tm date;
    strptime(datetime.c_str(), "%d/%m/%Y", &date);
    date.tm_hour = now.tm_hour;
    date.tm_min = now.tm_min;
    date.tm_sec = now.tm_sec;
    return date;
  }

  struct tm asDateTime(time_t t) {
    struct tm dateTime;
    return *(localtime (&t));
  }

  time_t asTimestamp(struct tm a) {
    return mktime(&a);
  }

  struct tm addHours(struct tm datetime, int n) {
    time_t t = asTimestamp(datetime);
    t += (n * 60 * 60);
    return asDateTime(t);
  }

  struct tm addDays(struct tm datetime, int n) {
    return addHours(datetime, 24 * n);
  }

  bool hasSameTime(struct tm *a, struct tm *b) {
    return (a->tm_hour == b->tm_hour) && (a->tm_min == b->tm_min);
  }

  bool hasSameDate(struct tm *a, struct tm *b) {
    return (a->tm_year == b->tm_year) && (a->tm_mon == b->tm_mon) && (a->tm_mday == b->tm_mday);
  }

  bool timeIsBeforeEq(struct tm a, struct tm b) {
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

  bool timeIsEq(struct tm a, struct tm b) {
    return (a.tm_hour == b.tm_hour) && (a.tm_min == b.tm_min);
  }

  bool timeIsBeforeNotEq(struct tm a, struct tm b) {
    return timeIsBeforeEq(a, b) && !timeIsEq(a, b);
  }

  bool dateIsBeforeEq(struct tm a, struct tm b) {
    if(a.tm_year > b.tm_year) {
      return false;
    } else if(a.tm_year < b.tm_year) {
      return true;
    } else {
      if(a.tm_mon > b.tm_mon) {
        return false;
      } else if(a.tm_mon < b.tm_mon) {
        return true;
      } else {
        if(a.tm_mday > b.tm_mday) {
          return false;
        } else {
          return true;
        }
      }
    }
  }

  bool dateIsEq(struct tm a, struct tm b) {
    return (a.tm_year == b.tm_year) && (a.tm_mon == b.tm_mon) && (a.tm_mday == b.tm_mday);
  }

  bool dateIsBeforeNotEq(struct tm a, struct tm b) {
    return dateIsBeforeEq(a, b) && !dateIsEq(a, b);
  }

  bool datetimeIsBeforeEq(struct tm a, struct tm b) {
    if(dateIsBeforeNotEq(a, b)) {
      return true;
    } else if(dateIsBeforeNotEq(b, a)) {
      return false;
    } else {
      return timeIsBeforeEq(a, b);
    }
  }

  bool datetimeNotEq(struct tm a, struct tm b) {
    return !dateIsEq(a, b) || !timeIsEq(a, b);
  }

  bool datetimeIsBeforeNotEq(struct tm a, struct tm b) {
    return datetimeIsBeforeEq(a, b) && !(dateIsEq(a, b) && timeIsEq(a, b));
  }

  Json::Value serializeArrivalTime(ArrivalTime *arrivalTime) {
    Json::Value json;
    int arrival = asTimestamp(arrivalTime->arrival);
    int departure = asTimestamp(arrivalTime->departure);
    json["stopId"] = arrivalTime->stopId;
    json["arrival"] = arrival;
    json["departure"] = departure;
    json["tripId"] = arrivalTime->tripId;
    json["pos"] = arrivalTime->pos;
    return json;
  }

  Json::Value serializeArrivalTimes(std::list<ArrivalTime> arrivalTimes) {
    Json::Value array;
    for(std::list<ArrivalTime>::const_iterator iterator = arrivalTimes.begin(), end = arrivalTimes.end(); iterator != end; ++iterator) {
      ArrivalTime arrivalTime = *iterator;
      array.append(serializeArrivalTime(&arrivalTime));
    }
    return array;
  }

  std::string formatArrivalTime(ArrivalTime *arrivalTime) {
    Json::Value serialized = serializeArrivalTime(arrivalTime);
    return serialized.toStyledString();
  }

  std::string formatArrivalTimes(std::list<ArrivalTime> arrivalTimes) {
    return serializeArrivalTimes(arrivalTimes).toStyledString();
  }

  std::string formatEdges(std::list<std::string> edges) {
    Json::Value array;
    for (auto iterator = edges.begin(), end = edges.end(); iterator != end; ++iterator) {
      array.append(*iterator);
    }
    return array.toStyledString();
  }

  Json::Value serializeStopTime(StopTime *stopTime) {
    Json::Value json;
    int arrival = asTimestamp(stopTime->arrival);
    int departure = asTimestamp(stopTime->departure);
    json["tripId"] = stopTime->tripId;
    json["arrival"] = arrival;
    json["departure"] = departure;
    json["pos"] = stopTime->pos;
    return json;
  }

  std::string formatStopTime(StopTime *stopTime) {
    Json::Value serialized = serializeStopTime(stopTime);
    return serialized.toStyledString();
  }

  std::string formatStopTimes(std::list<StopTime> stopTimes) {
    Json::Value array;
    for (auto iterator = stopTimes.begin(), end = stopTimes.end(); iterator != end; ++iterator) {
      array.append(serializeStopTime(&*iterator));
    }
    return array.toStyledString();
  }

  std::list<CalendarDate> getCalendarDatesByServiceId(CalendarDates *calendarDates, std::string serviceId) {
    std::list<CalendarDate> results;
    auto exceptions = (*calendarDates)[serviceId].calendardates();
    for (auto iterator = exceptions.begin(), end = exceptions.end(); iterator != end; ++iterator) {
      m::cheminot::data::CalendarDate calendarDateBuf = *iterator;
      CalendarDate calendarDate;
      calendarDate.serviceId = calendarDateBuf.serviceid();
      calendarDate.date = parseDate(calendarDateBuf.date());
      calendarDate.exceptionType = calendarDateBuf.exceptiontype();
      results.push_back(calendarDate);
    }
    return results;
  }

  StopTime parseStopTime(m::cheminot::data::StopTime stopTimeBuf) {
    struct StopTime stopTime;
    stopTime.tripId = stopTimeBuf.tripid();
    stopTime.arrival = parseTime(stopTimeBuf.arrival());
    stopTime.departure = parseTime(stopTimeBuf.departure());
    stopTime.pos = stopTimeBuf.pos();
    return stopTime;
  }

  std::list<StopTime> parseStopTimes(google::protobuf::RepeatedPtrField< ::m::cheminot::data::StopTime> stopTimesBuf) {
    std::list<StopTime> stopTimes;
    for(auto iterator = stopTimesBuf.begin(), end = stopTimesBuf.end(); iterator != end; ++iterator) {
      StopTime stopTime = parseStopTime(*iterator);
      stopTimes.push_back(stopTime);
    }
    return stopTimes;
  }

  std::list<std::string> parseEdges(google::protobuf::RepeatedPtrField< std::string > edgesBuf) {
    std::list<std::string> stopIds;
    for(auto iterator = edgesBuf.begin(), end = edgesBuf.end(); iterator != end; ++iterator) {
      stopIds.push_back(*iterator);
    }
    return stopIds;
  }

  std::unique_ptr<Calendar> parseCalendar(m::cheminot::data::Calendar calendarBuf) {
    std::unique_ptr<Calendar> calendar(new Calendar());
    std::map<std::string, bool> week;
    week["monday"] = calendarBuf.monday() == "1";
    week["tuesday"] = calendarBuf.tuesday() == "1";
    week["wednesday"] = calendarBuf.wednesday() == "1";
    week["thursday"] = calendarBuf.thursday() == "1";
    week["friday"] = calendarBuf.friday() == "1";
    week["saturday"] = calendarBuf.saturday() == "1";
    week["sunday"] = calendarBuf.sunday() == "1";

    calendar->serviceId = calendarBuf.serviceid();
    calendar->week = week;
    calendar->startDate = parseDate(calendarBuf.startdate());
    calendar->endDate = parseDate(calendarBuf.enddate());
    return calendar;
  }

  Vertice getVerticeFromGraph(Graph *graph, std::string id) {
    struct Vertice vertice;
    auto verticeBuf = (*graph)[id];
    vertice.id = verticeBuf.id();
    vertice.name = verticeBuf.name();
    vertice.edges = parseEdges(verticeBuf.edges());
    vertice.stopTimes = parseStopTimes(verticeBuf.stoptimes());
    return vertice;
  }

  Trip parseTripRow(std::list< std::map<std::string, const void*> >::const_iterator it) {
    std::map<std::string, const void*> row = *it;
    struct Trip trip;
    trip.id = (const char*)row["id"];
    trip.direction = (const char*)row["direction"];
    const char *calendar = (const char*)row["calendar"];
    if(calendar != NULL) {
      m::cheminot::data::Calendar calendarBuf;
      calendarBuf.ParseFromString(calendar);
      trip.calendar = parseCalendar(calendarBuf);
    }
    return trip;
  }

  std::map< std::string, const void*> parseRow(sqlite3_stmt *stmt) {
    int cols = sqlite3_column_count(stmt);
    std::map<std::string, const void*> row;
    for(int col=0 ; col<cols; col++) {
      std::string name(sqlite3_column_name(stmt, col));
      const char *value = (const char *)sqlite3_column_text(stmt, col);
      if(value != NULL) {
        row[name] = strdup(value);
      }
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

  sqlite3* openConnection(std::string path) {
    sqlite3 *handle;
    sqlite3_open_v2(path.c_str(), &handle, SQLITE_OPEN_READONLY, NULL);
    return handle;
  }

  void parseGraph(std::string path, Graph *graph) {
    std::ifstream in(path);
    if(in.is_open()) {
      m::cheminot::data::Graph graphBuf;
      graphBuf.ParseFromIstream(&in);
      *graph = *graphBuf.mutable_vertices();
      in.close();
    } else {
      throw std::runtime_error("Unexpected error while reading: " + path);
    }
  }

  void parseCalendarDates(std::string path, CalendarDates *calendarDates) {
    std::ifstream in(path);
    if(in.is_open()) {
      m::cheminot::data::CalendarDates calendarDatesBuf;
      calendarDatesBuf.ParseFromIstream(&in);
      *calendarDates = calendarDatesBuf.exceptionsbyserviceid();
      in.close();
    } else {
      throw std::runtime_error("Unexpected error while reading: " + path);
    }
  }

  std::string getVersion(sqlite3 *handle) {
    std::string query = "SELECT value FROM META WHERE key = 'version'";
    std::list< std::map<std::string, const void*> > results = executeSQL(handle, query);
    return (char *)results.front()["value"];
  }

  std::list<Trip> getTripsByIds(sqlite3 *handle, std::list<std::string> ids) {
    std::list<Trip> trips;
    std::string values = std::accumulate(ids.begin(), ids.end(), (std::string)"", [](std::string acc, std::string id) {
        std::string quoted = "'" + id + "'";
        return (acc == "") ? quoted : (acc + ", " + quoted);
    });
    if(!ids.empty()) {
      std::string query = "SELECT * FROM TRIPS WHERE id IN (" + values + ")";
      std::list< std::map<std::string, const void*> > results = executeSQL(handle, query);
      for (std::list< std::map<std::string, const void*> >::const_iterator iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
        trips.push_back(parseTripRow(iterator));
      }
    }
    return trips;
  }

  bool isTerminus(StopTime *a) {
    return hasSameTime(&a->arrival, &a->departure) && a->pos > 0;
  }

  class CompareArrivalTime {
  public:
    bool operator()(const ArrivalTime& gi, const ArrivalTime& gj) {
      return datetimeIsBeforeEq(gj.departure, gi.departure);
    }
  };

  bool isTripRemovedOn(std::list<Trip>::const_iterator trip, CalendarDates *calendarDates, struct tm when) {
    auto exceptions = getCalendarDatesByServiceId(calendarDates, trip->calendar->serviceId);
    auto it = std::find_if(exceptions.begin(), exceptions.end(), [&when](CalendarDate calendarDate) {
        return hasSameDate(&calendarDate.date, &when) && (calendarDate.exceptionType == 2);
      });
    return it != exceptions.end();
  }

  bool isTripAddedOn(std::list<Trip>::const_iterator trip, CalendarDates *calendarDates, struct tm when) {
    auto exceptions = getCalendarDatesByServiceId(calendarDates, trip->calendar->serviceId);
    auto it = std::find_if(exceptions.begin(), exceptions.end(), [&when](CalendarDate exception) {
        return hasSameDate(&exception.date, &when) && (exception.exceptionType == 1);
      });
    return it != exceptions.end();
  }

  bool isTripValidToday(std::list<Trip>::const_iterator trip, struct tm when) {
    std::map<int, std::string> week { {1, "monday"}, {2, "tuesday"}, {3, "wednesday"}, {4, "thursday"}, {5, "friday"}, {6, "saturday"}, {0, "sunday"}};
    return trip->calendar->week[week[when.tm_wday]];
  }

  bool isTripInPeriod(std::list<Trip>::const_iterator trip, struct tm when) {
    struct tm startDate = trip->calendar->startDate;
    struct tm endDate = trip->calendar->endDate;
    bool before = dateIsBeforeEq(startDate, when);
    bool after = dateIsBeforeEq(when, endDate);
    return before && after;
  }

  bool isTripValidOn(std::list<Trip>::const_iterator trip, CalendarDates *calendarDates, struct tm when) {
    if(trip->calendar != NULL) {
      bool removed = isTripRemovedOn(trip, calendarDates, when);
      bool added = isTripAddedOn(trip, calendarDates, when);
      bool availableToday = isTripValidToday(trip, when);
      bool inPeriod = isTripInPeriod(trip, when);
      return (!removed && inPeriod && availableToday) || added;
    }
    return false;
  }

  std::map<std::string, bool> tripsAvailability(sqlite3 *handle, std::list<std::string> ids, CalendarDates *calendarDates, struct tm when) {
    std::map<std::string, bool> availablities;
    auto trips = getTripsByIds(handle, ids);
    for (std::list<Trip>::const_iterator iterator = trips.begin(), end = trips.end(); iterator != end; ++iterator) {
      availablities[iterator->id] = isTripValidOn(iterator, calendarDates, when);
    }
    return availablities;
  }

  std::list<StopTime> sortStopTimesBy(std::list<StopTime> stopTimes, struct tm t) {
    std::list<StopTime> stopTimesFromT = std::accumulate(stopTimes.begin(), stopTimes.end(), std::list<StopTime>(), [t](std::list<StopTime> acc, StopTime stopTime) {
        if(timeIsBeforeEq(stopTime.departure, t)) {
          stopTime.departure = addDays(stopTime.departure, 1);
          stopTime.arrival = addDays(stopTime.arrival, 1);
        }
        acc.push_back(stopTime);
        return acc;
    });

    stopTimesFromT.sort([](const StopTime& first, const StopTime& second) {
      return datetimeIsBeforeEq(first.departure, second.departure);
    });

    return stopTimesFromT;
  }

  std::list<StopTime> getAvailableDepartures(sqlite3 *handle, CalendarDates *calendarDates, ArrivalTime *vi, struct tm ts) {
    std::list<StopTime> departures(sortStopTimesBy(vi->vertice.stopTimes, ts));

    departures.remove_if([&vi] (StopTime &stopTime) {
        return !(datetimeIsBeforeNotEq(vi->arrival, stopTime.departure) && !isTerminus(&stopTime));
    });

    std::list<std::string> tripIds;
    for (std::list<StopTime>::const_iterator iterator = departures.begin(), end = departures.end(); iterator != end; ++iterator) {
      tripIds.push_back(iterator->tripId);
    }

    auto availablities = tripsAvailability(handle, tripIds, calendarDates, ts);

    departures.remove_if([&availablities] (const StopTime& stopTime) {
      return !availablities[stopTime.tripId];
    });

    printf("[Departures]\n");
    printf("%s\n", formatStopTimes(departures).c_str());

    return departures;
  }

  std::string hashArrivalTime(ArrivalTime *arrivalTime) {
    return arrivalTime->stopId + "#" + arrivalTime->tripId;
  }

  std::map<std::string, ArrivalTime> refineArrivalTimes(sqlite3 *handle, Graph *graph, CalendarDates *calendarDates, std::string vsId, std::string veId, struct tm ts) {

    std::map<std::string, ArrivalTime> results;
    std::map< std::string, int > pushed;

    // Initialize the queue

    std::priority_queue<ArrivalTime, std::vector<ArrivalTime>, CompareArrivalTime> queue;

    Vertice vs = getVerticeFromGraph(graph, vsId);

    struct ArrivalTime gs;
    gs.stopId = vsId;
    gs.arrival = ts;
    gs.vertice = vs;
    queue.push(gs);
    pushed[gs.stopId] = 1;

    // OK, Let's go !

    while(!queue.empty()) {

      ArrivalTime head = queue.top();
      queue.pop();
      results[head.stopId] = head;

      if(head.stopId == veId) {
        //printf("DONE!\n");
        return results;
      } else {
        printf("[HEAD]\n");
        printf("%s\n", formatArrivalTime(&head).c_str());
        Vertice vi = head.vertice;
        auto departures = getAvailableDepartures(handle, calendarDates, &head, ts);

        if(!departures.empty()) {
          printf("[EDGES] \n");
          printf("%s\n", formatEdges(vi.edges).c_str());
          std::list<StopTime> matched;
          std::list<ArrivalTime> justPushed;
          for (std::list<std::string>::const_iterator iterator = vi.edges.begin(), end = vi.edges.end(); iterator != end; ++iterator) {

            std::string vjId = *iterator;
            Vertice vj = getVerticeFromGraph(graph, vjId);
            std::list<StopTime> stopTimes(sortStopTimesBy(vj.stopTimes, ts));

            auto departuresAndArrivalTimes = std::accumulate(departures.begin(), departures.end(), std::list<std::tuple<StopTime, StopTime>>(), [&stopTimes](std::list<std::tuple<StopTime, StopTime>> acc, StopTime departureTime) {
                auto it = std::find_if(stopTimes.begin(), stopTimes.end(), [&departureTime](StopTime stopTime) {
                    return departureTime.tripId == stopTime.tripId && timeIsBeforeNotEq(departureTime.departure, stopTime.arrival);
                });

                if(it != stopTimes.end()) {
                  acc.push_back(std::tuple<StopTime, StopTime>(departureTime, *it));
                }
                return acc;
            });

            if(!departuresAndArrivalTimes.empty()) {
              std::tuple<StopTime, StopTime> next = departuresAndArrivalTimes.front();
              StopTime departureTime = std::get<0>(next);
              StopTime nextArrivalTime = std::get<1>(next);
              struct ArrivalTime arrivalTime;
              arrivalTime.stopId = vjId;
              arrivalTime.arrival = nextArrivalTime.arrival;
              arrivalTime.departure = nextArrivalTime.departure;
              arrivalTime.tripId = nextArrivalTime.tripId;
              arrivalTime.vertice = vj;
              arrivalTime.pos = nextArrivalTime.pos;
              matched.push_back(nextArrivalTime);
              //printf("====> departure %s \n", formatTime(departureTime.departure).c_str());
              //printf("=====> arrival at %s to %s at %s\n", formatDateTime(departureTime.arrival).c_str(), vjId.c_str(), formatDateTime(arrivalTime.arrival).c_str());
              if(pushed.find(vjId) == pushed.end()) {
                queue.push(arrivalTime);
                justPushed.push_back(arrivalTime);
                pushed[vjId] = 1;
              }
              if(head.stopId == vsId) {
                auto vs = results.find(vsId);
                vs->second.tripId = departureTime.tripId;
                vs->second.arrival = departureTime.arrival;
                vs->second.departure = departureTime.departure;
                vs->second.pos = departureTime.pos;
              }
            } else {
              //printf("--NONE--\n");
            }
          }
          printf("[MATCHED]\n");
          printf("%s\n", formatStopTimes(matched).c_str());

          printf("[JUST PUSHED]\n");
          printf("%s\n", formatArrivalTimes(justPushed).c_str());
        } else {
          //printf("\nEMPTY!");
        }
      }
    };
    return results;
  }

  std::list<ArrivalTime> pathSelection(Graph *graph, std::map<std::string, ArrivalTime> *arrivalTimes, struct tm ts, std::string vsId, std::string veId) {
    Vertice vs = getVerticeFromGraph(graph, vsId);
    Vertice vj = getVerticeFromGraph(graph, veId);
    ArrivalTime ge = (*arrivalTimes)[vj.id];
    ArrivalTime gs = (*arrivalTimes)[vs.id];
    std::list<ArrivalTime> path;
    while(vj.id != vs.id) {
      ArrivalTime gj = (*arrivalTimes)[vj.id];
      std::list< std::pair<ArrivalTime, Vertice> > matched;
      for (std::list<std::string>::const_iterator iterator = vj.edges.begin(), end = vj.edges.end(); iterator != end; ++iterator) {
        std::string viId = *iterator;
        Vertice vi = getVerticeFromGraph(graph, viId);
        auto gi = arrivalTimes->find(viId);
        if(gi != arrivalTimes->end()) {
          auto found = std::find_if(vi.stopTimes.begin(), vi.stopTimes.end(), [&gj](StopTime viStopTime) {
            return viStopTime.tripId == gj.tripId && timeIsBeforeNotEq(viStopTime.departure, gj.arrival);
          });
          if(found != vi.stopTimes.end()) {
            matched.push_back(std::make_pair(gi->second, vi));
          }
        }
      }

      if(!matched.empty()) {
        matched.sort([](std::pair<ArrivalTime, Vertice> a, std::pair<ArrivalTime, Vertice> b) {
            return a.first.pos >= b.first.pos;
        });
        vj = matched.front().second;
        path.push_front(gj);
      } else {
        //printf("\n ERROR");
        //ERROR
        break;
      }
    }

    path.push_front(gs);
    return path;
  }

  std::list<ArrivalTime> lookForBestTrip(sqlite3 *handle, Graph *graph, CalendarDates *calendarDates, std::string vsId, std::string veId, struct tm at) {
    auto arrivalTimes = refineArrivalTimes(handle, graph, calendarDates, vsId, veId, at);
    return pathSelection(graph, &arrivalTimes, at, vsId, veId);
  }
}
