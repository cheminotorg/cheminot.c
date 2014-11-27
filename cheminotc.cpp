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

  struct tm parseTime(std::string datetime) {
    struct tm now = getNow();
    struct tm time;
    strptime(datetime.c_str(), "%H:%M", &time);
    now.tm_hour = time.tm_hour;
    now.tm_min = time.tm_min;
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

  Json::Value serializeArrivalTime(ArrivalTime *arrivalTime) {
    Json::Value json;
    int arrival = asTimestamp(arrivalTime->arrival);
    int departure = asTimestamp(arrivalTime->departure);
    json["stopId"] = arrivalTime->stopId;
    json["arrivalTime"] = arrival;
    json["departureTime"] = departure;
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

  std::list<CalendarDate> getCalendarDatesByServiceId(m::cheminot::data::CalendarDates *calendarDates, std::string serviceId) {
    std::list<CalendarDate> results;
    auto exceptionsByServiceId = calendarDates->exceptionsbyserviceid();
    auto exceptions = exceptionsByServiceId[serviceId].calendardates();

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

  Vertice getVerticeFromGraph(m::cheminot::data::Graph *graph, std::string id) {
    struct Vertice vertice;
    auto verticesBuf = *(graph->mutable_vertices());
    auto verticeBuf = verticesBuf[id];
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

  m::cheminot::data::Graph parseGraph(std::string path) {
    std::ifstream in(path);
    if(in.is_open()) {
      m::cheminot::data::Graph graphBuf;
      graphBuf.ParseFromIstream(&in);
      in.close();
      return graphBuf;
    } else {
      throw std::runtime_error("Unexpected error while reading: " + path);
    }
  }

  m::cheminot::data::CalendarDates parseCalendarDates(std::string path) {
    std::ifstream in(path);
    if(in.is_open()) {
      m::cheminot::data::CalendarDates calendarDates;
      calendarDates.ParseFromIstream(&in);
      in.close();
      return calendarDates;
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

  bool isTerminus(StopTime *a) {
    return hasSameTime(&a->arrival, &a->departure) && a->pos > 0;
  }

  class CompareArrivalTime {
  public:
    bool operator()(const ArrivalTime& gi, const ArrivalTime& gj) {
      return timeIsBeforeEq(gj.departure, gi.departure);
    }
  };

  bool isTripRemovedOn(std::list<Trip>::const_iterator trip, m::cheminot::data::CalendarDates *calendarDates, struct tm when) {
    auto exceptions = getCalendarDatesByServiceId(calendarDates, trip->calendar->serviceId);
    auto it = std::find_if(exceptions.begin(), exceptions.end(), [&when](CalendarDate calendarDate) {
        return hasSameDate(&calendarDate.date, &when) && (calendarDate.exceptionType == 2);
      });
    return it != exceptions.end();
  }

  bool isTripAddedOn(std::list<Trip>::const_iterator trip, m::cheminot::data::CalendarDates *calendarDates, struct tm when) {
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

  bool isTripValidOn(std::list<Trip>::const_iterator trip, m::cheminot::data::CalendarDates *calendarDates, struct tm when) {
    if(trip->calendar != NULL) {
      bool removed = isTripRemovedOn(trip, calendarDates, when);
      bool added = isTripAddedOn(trip, calendarDates, when);
      bool availableToday = isTripValidToday(trip, when);
      bool inPeriod = isTripInPeriod(trip, when);
      return (!removed && inPeriod && availableToday) || added;
    }
    return false;
  }

  std::map<std::string, bool> tripsAvailability(sqlite3 *handle, std::list<std::string> ids, m::cheminot::data::CalendarDates *calendarDates, struct tm when) {
    std::map<std::string, bool> availablities;
    auto trips = getTripsByIds(handle, ids);
    for (std::list<Trip>::const_iterator iterator = trips.begin(), end = trips.end(); iterator != end; ++iterator) {
      availablities[iterator->id] = isTripValidOn(iterator, calendarDates, when);
    }
    return availablities;
  }

  std::list<StopTime> sortStopTimesBy(std::list<StopTime> stopTimes, struct tm t) {
    typedef std::tuple<std::list<StopTime>, std::list<StopTime>> Splitted;
    Splitted s = std::tuple<std::list<StopTime>, std::list<StopTime>>();

    Splitted beforeAfter = std::accumulate(stopTimes.begin(), stopTimes.end(), s, [t](Splitted acc, StopTime stopTime) {
        if(timeIsBeforeEq(stopTime.departure, t)) {
          stopTime.departure = addDays(stopTime.departure, 1);
          stopTime.arrival = addDays(stopTime.arrival, 1);
          std::get<0>(acc).push_back(stopTime);
        } else {
          std::get<1>(acc).push_back(stopTime);
        }
        return acc;
    });

    std::list<StopTime> tommorow = std::get<0>(beforeAfter);
    std::list<StopTime> next = std::get<1>(beforeAfter);
    next.insert(next.end(), tommorow.begin(), tommorow.end());
    next.sort([](const StopTime& first, const StopTime& second) {
      return datetimeIsBeforeEq(first.departure, second.departure);
    });

    return next;
  }

  std::list<StopTime> getAvailableDepartures(sqlite3 *handle, m::cheminot::data::CalendarDates *calendarDates, ArrivalTime *vi, struct tm ts) {
    std::list<StopTime> departures(sortStopTimesBy(vi->vertice.stopTimes, ts));

    departures.remove_if([&] (StopTime &stopTime) {
        return !(timeIsBeforeNotEq(vi->arrival, stopTime.departure) && !isTerminus(&stopTime));
    });

    std::list<std::string> tripIds;
    for (std::list<StopTime>::const_iterator iterator = departures.begin(), end = departures.end(); iterator != end; ++iterator) {
      tripIds.push_back(iterator->tripId);
    }

    auto availablities = tripsAvailability(handle, tripIds, calendarDates, ts);

    departures.remove_if([&] (const StopTime& stopTime) {
      return !availablities[stopTime.tripId];
    });

    return departures;
  }

  std::map<std::string, ArrivalTime> refineArrivalTimes(sqlite3 *handle, m::cheminot::data::Graph *graph, m::cheminot::data::CalendarDates *calendarDates, std::string vsId, std::string veId, struct tm ts) {

    std::map<std::string, ArrivalTime> results;
    std::map< std::string, ArrivalTime > pushed;

    // Initialize the queue

    std::priority_queue<ArrivalTime, std::vector<ArrivalTime>, CompareArrivalTime> queue;

    Vertice vs = getVerticeFromGraph(graph, vsId);

    struct ArrivalTime gs;
    gs.stopId = vsId;
    gs.arrival = ts;
    gs.vertice = vs;
    queue.push(gs);
    pushed[vsId] = gs;

    // OK, Let's go !

    while(!queue.empty()) {

      ArrivalTime head = pushed[queue.top().stopId];
      queue.pop();

      results[head.stopId] = head;

      if(head.stopId == veId) {
        printf("\nDONE!");
        return results;
      } else {

        Vertice vi = head.vertice;
        auto departures = getAvailableDepartures(handle, calendarDates, &head, ts);

        if(!departures.empty()) {

          for (std::list<std::string>::const_iterator iterator = vi.edges.begin(), end = vi.edges.end(); iterator != end; ++iterator) {
            std::string vjId = *iterator;
            Vertice vj = getVerticeFromGraph(graph, vjId);
            std::list<StopTime> stopTimes(sortStopTimesBy(vj.stopTimes, head.arrival));

            auto arrivalTimes = std::accumulate(departures.begin(), departures.end(), std::list<StopTime>(), [&stopTimes](std::list<StopTime> acc, StopTime departureTime) {
                auto it = std::find_if(stopTimes.begin(), stopTimes.end(), [&departureTime](StopTime stopTime) {
                    return departureTime.tripId == stopTime.tripId && timeIsBeforeNotEq(departureTime.departure, stopTime.arrival);
                });
                if(it != stopTimes.end()) {
                  acc.push_back(*it);
                }
                return acc;
            });

            if(!arrivalTimes.empty()) {
              StopTime next = arrivalTimes.front();
              struct ArrivalTime arrivalTime;
              arrivalTime.stopId = vjId;
              arrivalTime.arrival = next.arrival;
              arrivalTime.departure = next.departure;
              arrivalTime.tripId = next.tripId;
              arrivalTime.vertice = vj;
              arrivalTime.pos = next.pos;
              if(pushed.find(vjId) == pushed.end()) {
                queue.push(arrivalTime);
              }
              pushed[vjId] = arrivalTime;
            }
          }

        } else {
          printf("\nEMPTY!");
        }
      }
    };
    return results;
  }

  std::list<ArrivalTime> pathSelection(m::cheminot::data::Graph *graph, std::map<std::string, ArrivalTime> *arrivalTimes, struct tm ts, std::string vsId, std::string veId) {
    Vertice vs = getVerticeFromGraph(graph, vsId);
    Vertice vj = getVerticeFromGraph(graph, veId);
    ArrivalTime ge = (*arrivalTimes)[vj.id];
    std::list<ArrivalTime> path;
    while(vj.id != vs.id) {
      ArrivalTime gj = (*arrivalTimes)[vj.id];
      std::list< std::pair<ArrivalTime, Vertice> > matched;
      for (std::list<std::string>::const_iterator iterator = vj.edges.begin(), end = vj.edges.end(); iterator != end; ++iterator) {
        std::string viId = *iterator;
        Vertice vi = getVerticeFromGraph(graph, viId);
        auto gi = arrivalTimes->find(viId);
        if(gi != arrivalTimes->end()) {
          auto found = std::find_if(vi.stopTimes.begin(), vi.stopTimes.end(), [&](StopTime viStopTime) {
            return viStopTime.tripId == gj.tripId && timeIsBeforeNotEq(viStopTime.departure, gj.arrival);
          });
          if(found != vi.stopTimes.end()) {
            matched.push_back(std::make_pair(gi->second, vi));
          }
        }
      }

      if(!matched.empty()) {
        matched.sort([&](std::pair<ArrivalTime, Vertice> a, std::pair<ArrivalTime, Vertice> b) {
            return a.first.pos >= b.first.pos;
        });
        vj = matched.front().second;
        path.push_front(gj);
      } else {
        printf("\n ERROR");
        //ERROR
        break;
      }
    }

    ArrivalTime last = path.front();
    ArrivalTime gs = (*arrivalTimes)[vsId];
    auto stopTimeGs = *(std::find_if(vs.stopTimes.begin(), vs.stopTimes.end(), [&](StopTime stopTime) {
      return stopTime.tripId == last.tripId;
    }));

    gs.tripId = stopTimeGs.tripId;
    gs.departure = stopTimeGs.departure;
    gs.arrival = stopTimeGs.arrival;

    path.push_front(gs);
    return path;
  }

  std::list<ArrivalTime> lookForBestTrip(sqlite3 *handle, m::cheminot::data::Graph *graph, m::cheminot::data::CalendarDates *calendarDates, std::string vsId, std::string veId, struct tm at) {
    auto arrivalTimes = refineArrivalTimes(handle, graph, calendarDates, vsId, veId, at);
    return pathSelection(graph, &arrivalTimes, at, vsId, veId);
  }
}
