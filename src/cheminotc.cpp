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

  void oops(std::string message) {
    throw std::runtime_error(message);
  }

  tm getNow() {
    time_t rawtime;
    time(&rawtime);
    tm *info = gmtime(&rawtime);
    return *info;
  }

  std::string formatTime(tm time) {
    return to_string(time.tm_hour) + ":" + to_string(time.tm_min);
  }

  std::string formatDate(tm time) {
    return to_string(time.tm_mday) + "/" + to_string(time.tm_mon + 1) + "/" + to_string(time.tm_year);
  }

  std::string formatDateTime(tm datetime) {
    return formatDate(datetime) + " " + formatTime(datetime);
  }

  tm parseTime(std::string datetime) {
    tm now = getNow();
    tm time;
    strptime(datetime.c_str(), "%H:%M", &time);
    now.tm_hour = time.tm_hour;
    now.tm_min = time.tm_min;
    now.tm_sec = 0;
    return now;
  }

  tm parseDate(std::string datetime) {
    tm now = getNow();
    tm date;
    strptime(datetime.c_str(), "%d/%m/%Y", &date);
    date.tm_hour = now.tm_hour;
    date.tm_min = now.tm_min;
    date.tm_sec = now.tm_sec;
    return date;
  }

  tm asDateTime(time_t t) {
    tm dateTime;
    return *(localtime (&t));
  }

  time_t asTimestamp(tm a) {
    time_t timestamp = mktime(&a);
    return timestamp;
  }

  tm addMinutes(tm datetime, int n) {
    time_t t = asTimestamp(datetime);
    t += (n * 60);
    return asDateTime(t);
  }

  tm addHours(tm datetime, int n) {
    time_t t = asTimestamp(datetime);
    t += (n * 60 * 60);
    return asDateTime(t);
  }

  tm minusHours(tm datetime, int n) {
    time_t t = asTimestamp(datetime);
    t -= (n * 60 * 60);
    return asDateTime(t);
  }

  tm addDays(tm datetime, int n) {
    return addHours(datetime, 24 * n);
  }

  bool hasSameTime(const tm &a, const tm &b) {
    return (a.tm_hour == b.tm_hour) && (a.tm_min == b.tm_min);
  }

  bool hasSameDate(const tm &a, const tm &b) {
    return (a.tm_year == b.tm_year) && (a.tm_mon == b.tm_mon) && (a.tm_mday == b.tm_mday);
  }

  bool hasSameDateTime(const tm &a, const tm &b) {
    return hasSameTime(a, b) && hasSameDate(a, b);
  }

  bool timeIsBeforeEq(const tm &a, const tm &b) {
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

  bool timeIsBeforeNotEq(const tm &a, const tm &b) {
    return timeIsBeforeEq(a, b) && !hasSameTime(a, b);
  }

  bool dateIsBeforeEq(const tm &a, const tm &b) {
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

  bool dateIsBeforeNotEq(const tm &a, const tm &b) {
    return dateIsBeforeEq(a, b) && !hasSameDate(a, b);
  }

  bool datetimeIsBeforeEq(const tm &a, const tm &b) {
    if(dateIsBeforeEq(a, b)) {
      if(hasSameDate(a, b)) {
        return timeIsBeforeEq(a, b);
      } else {
        return true;
      }
    } else {
      return false;
    }
  }

  bool datetimeIsBeforeNotEq(const tm &a, const tm &b) {
    return datetimeIsBeforeEq(a, b) && !hasSameDateTime(a, b);
  }

  Json::Value serializeArrivalTime(ArrivalTime arrivalTime) {
    Json::Value json;
    int arrival = asTimestamp(arrivalTime.arrival);
    int departure = asTimestamp(arrivalTime.departure);
    json["stopId"] = arrivalTime.stopId;
    json["arrival"] = arrival;
    json["departure"] = departure;
    json["tripId"] = arrivalTime.tripId;
    json["pos"] = arrivalTime.pos;
    return json;
  }

  Json::Value serializeArrivalTimes(std::list<ArrivalTime> arrivalTimes) {
    Json::Value array;
    for(auto iterator = arrivalTimes.begin(), end = arrivalTimes.end(); iterator != end; ++iterator) {
      ArrivalTime arrivalTime = *iterator;
      array.append(serializeArrivalTime(arrivalTime));
    }
    return array;
  }

  std::string formatArrivalTime(ArrivalTime arrivalTime) {
    Json::Value serialized = serializeArrivalTime(arrivalTime);
    return serialized.toStyledString();
  }

  std::string formatArrivalTimes(std::list<ArrivalTime> arrivalTimes) {
    return serializeArrivalTimes(arrivalTimes).toStyledString();
  }

  Json::Value serializeEdges(std::list<std::string> edges) {
    Json::Value array;
    for (auto iterator = edges.begin(), end = edges.end(); iterator != end; ++iterator) {
      array.append(*iterator);
    }
    return array;
  }

  std::string formatEdges(std::list<std::string> edges) {
    return serializeEdges(edges).toStyledString();
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

  Json::Value serializeStopTimes(std::list<StopTime> stopTimes) {
    Json::Value array;
    for (auto iterator = stopTimes.begin(), end = stopTimes.end(); iterator != end; ++iterator) {
      array.append(serializeStopTime(&*iterator));
    }
    return array;
  }

  std::string formatStopTime(StopTime *stopTime) {
    Json::Value serialized = serializeStopTime(stopTime);
    return serialized.toStyledString();
  }

  std::string formatStopTimes(std::list<StopTime> stopTimes) {
    return serializeStopTimes(stopTimes).toStyledString();
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
    StopTime stopTime;
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
    std::unordered_map<std::string, bool> week;
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

  std::shared_ptr<Vertice> getVerticeFromGraph(Graph *graph, VerticesCache *verticesCache, std::string id) {
    auto it = verticesCache->find(id);
    if(it != verticesCache->end()) {
      return it->second;
    } else {
      std::shared_ptr<Vertice> vertice {new Vertice};
      auto verticeBuf = (*graph)[id];
      vertice->id = verticeBuf.id();
      vertice->name = verticeBuf.name();
      vertice->edges = parseEdges(verticeBuf.edges());
      vertice->stopTimes = parseStopTimes(verticeBuf.stoptimes());
      (*verticesCache)[id] = vertice;
      return vertice;
    }
  }

  Trip parseTripRow(std::list< std::unordered_map<std::string, const void*> >::const_iterator it) {
    std::unordered_map<std::string, const void*> row = *it;
    Trip trip;
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

  std::unordered_map< std::string, const void*> parseRow(sqlite3_stmt *stmt) {
    int cols = sqlite3_column_count(stmt);
    std::unordered_map<std::string, const void*> row;
    for(int col=0 ; col<cols; col++) {
      std::string name(sqlite3_column_name(stmt, col));
      const char *value = (const char *)sqlite3_column_text(stmt, col);
      if(value != NULL) {
        row[name] = strdup(value);
      }
    }
    return row;
  }

  std::list< std::unordered_map<std::string, const void*> > executeSQL(sqlite3 *handle, std::string query) {
    std::list< std::unordered_map <std::string, const void*> > results;
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
        oops("Unexpected error while executing this SQL query: " + query);
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
    std::list< std::unordered_map<std::string, const void*> > results = executeSQL(handle, query);
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
      std::list< std::unordered_map<std::string, const void*> > results = executeSQL(handle, query);
      for (std::list< std::unordered_map<std::string, const void*> >::const_iterator iterator = results.begin(), end = results.end(); iterator != end; ++iterator) {
        trips.push_back(parseTripRow(iterator));
      }
    }
    return trips;
  }

  bool isTerminus(const StopTime &a) {
    return hasSameTime(a.arrival, a.departure) && a.pos > 0;
  }

  class CompareArrivalTime {
  public:
    bool operator()(const ArrivalTime& gi, const ArrivalTime& gj) {
      return datetimeIsBeforeEq(gj.departure, gi.departure);
    }
  };

  bool isTripRemovedOn(std::list<Trip>::const_iterator trip, CalendarDates *calendarDates, tm when) {
    auto exceptions = getCalendarDatesByServiceId(calendarDates, trip->calendar->serviceId);
    auto it = std::find_if(exceptions.begin(), exceptions.end(), [&when](CalendarDate calendarDate) {
        return hasSameDate(calendarDate.date, when) && (calendarDate.exceptionType == 2);
      });
    return it != exceptions.end();
  }

  bool isTripAddedOn(std::list<Trip>::const_iterator trip, CalendarDates *calendarDates, tm when) {
    auto exceptions = getCalendarDatesByServiceId(calendarDates, trip->calendar->serviceId);
    auto it = std::find_if(exceptions.begin(), exceptions.end(), [&when](CalendarDate exception) {
        return hasSameDate(exception.date, when) && (exception.exceptionType == 1);
      });
    return it != exceptions.end();
  }

  bool isTripValidToday(std::list<Trip>::const_iterator trip, tm when) {
    std::unordered_map<int, std::string> week { {1, "monday"}, {2, "tuesday"}, {3, "wednesday"}, {4, "thursday"}, {5, "friday"}, {6, "saturday"}, {0, "sunday"}};
    return trip->calendar->week[week[when.tm_wday]];
  }

  bool isTripInPeriod(std::list<Trip>::const_iterator trip, tm when) {
    tm startDate = trip->calendar->startDate;
    tm endDate = trip->calendar->endDate;
    bool before = dateIsBeforeEq(startDate, when);
    bool after = dateIsBeforeEq(when, endDate);
    return before && after;
  }

  bool isTripValidOn(std::list<Trip>::const_iterator trip, CalendarDates *calendarDates, tm when) {
    if(trip->calendar != NULL) {
      bool removed = isTripRemovedOn(trip, calendarDates, when);
      bool added = isTripAddedOn(trip, calendarDates, when);
      bool availableToday = isTripValidToday(trip, when);
      bool inPeriod = isTripInPeriod(trip, when);
      return (!removed && inPeriod && availableToday) || added;
    }
    return false;
  }

  std::unordered_map<std::string, bool> tripsAvailability(sqlite3 *handle, std::list<std::string> ids, CalendarDates *calendarDates, tm when) {
    std::unordered_map<std::string, bool> availablities;
    auto trips = getTripsByIds(handle, ids);
    for (auto iterator = trips.begin(), end = trips.end(); iterator != end; ++iterator) {
      availablities[iterator->id] = isTripValidOn(iterator, calendarDates, when);
    }
    return availablities;
  }

  std::list<StopTime> orderStopTimesBy(std::list<StopTime> stopTimes, const tm &t) {
    std::list<StopTime> stopTimesFromT = std::accumulate(stopTimes.begin(), stopTimes.end(), std::list<StopTime>(), [&t](std::list<StopTime> acc, StopTime stopTime) {
        if(datetimeIsBeforeNotEq(stopTime.departure, t)) {
          stopTime.departure = addDays(stopTime.departure, 1);
          stopTime.arrival = addDays(stopTime.arrival, 1);
        }
        acc.push_back(stopTime);
        return acc;
    });

    stopTimesFromT.sort([](const StopTime &a, const StopTime &b) {
      return datetimeIsBeforeEq(a.departure, b.departure);
    });

    return stopTimesFromT;
  }

  std::list<StopTime> getAvailableDepartures(sqlite3 *handle, CalendarDates *calendarDates, tm arrivalTime, std::shared_ptr<Vertice> vi) {
    std::list<StopTime> departures(orderStopTimesBy(vi->stopTimes, arrivalTime));
    departures.remove_if([&arrivalTime] (StopTime &stopTime) {
      return !(datetimeIsBeforeEq(arrivalTime, stopTime.departure) && !isTerminus(stopTime));
    });

    std::list<std::string> tripIds;
    for (auto iterator = departures.begin(), end = departures.end(); iterator != end; ++iterator) {
      tripIds.push_back(iterator->tripId);
    }

    auto availablities = tripsAvailability(handle, tripIds, calendarDates, arrivalTime);

    departures.remove_if([&availablities] (const StopTime& stopTime) {
      return !availablities[stopTime.tripId];
    });

    return departures;
  }

  struct QueueItem {
    std::string stopId;
    std::string tripId;
    tm ti;
    tm gi;
  };

  class CompareQueueItem {
  public:
    bool operator()(std::shared_ptr<QueueItem> itemA, std::shared_ptr<QueueItem> itemB) {
      return datetimeIsBeforeEq(itemB->gi, itemA->gi);
    }
  };

  typedef std::priority_queue<std::shared_ptr<QueueItem>, std::vector<std::shared_ptr<QueueItem>>, CompareQueueItem> Queue;

  tm infinite() {
    tm t;
    t.tm_sec = INT_MAX;
    t.tm_min = INT_MAX;
    t.tm_hour = INT_MAX;
    t.tm_mday = INT_MAX;
    t.tm_mon = INT_MAX;
    t.tm_year = INT_MAX;
    t.tm_wday = INT_MAX;
    t.tm_yday = INT_MAX;
    t.tm_wday = INT_MAX;
    return t;
  }

  std::unordered_map<std::string, std::shared_ptr<QueueItem>> initTimeRefinement(sqlite3 *handle, Graph *graph, ArrivalTimesFunc *arrivalTimesFunc, CalendarDates *calendarDates, Queue *queue, std::shared_ptr<Vertice> vs, tm ts, std::list<tm> startingPeriod) {

    std::unordered_map<std::string, std::shared_ptr<QueueItem>> items;

    ArrivalTimeFunc gsFunc;
    for (auto iterator = startingPeriod.begin(), end = startingPeriod.end(); iterator != end; ++iterator) {
      tm departureTime = *iterator;
      ArrivalTime gs;
      gs.stopId = vs->id;
      gs.departure = departureTime;
      gs.arrival = departureTime;
      gsFunc[asTimestamp(departureTime)] = gs;
    }

    (*arrivalTimesFunc)[vs->id] = gsFunc;

    std::shared_ptr<QueueItem> qs {new QueueItem};
    qs->stopId = vs->id;
    qs->gi = ts;
    qs->tripId = "<trip>";
    qs->ti = ts;
    queue->push(qs);
    items[vs->id] = qs;

    tm INFINITE = infinite();

    for(auto iterator = graph->begin(), end = graph->end(); iterator != end; ++iterator) {
      std::string stopId = iterator->first;
      if(stopId != vs->id) {
        std::shared_ptr<QueueItem> qi {new QueueItem};
        qi->stopId = stopId;
        qi->gi = INFINITE;
        qi->tripId = "<trip>";
        qi->ti = ts;
        queue->push(qi);
        items[stopId] = qi;
      }
    }
    return items;
  }

  tm enlargeStartingTime(sqlite3 *handle, CalendarDates *calendarDates, Graph *graph, VerticesCache *verticesCache, ArrivalTimeFunc &giFunc, std::shared_ptr<QueueItem> qi, std::shared_ptr<QueueItem> qk, std::shared_ptr<Vertice> vi, std::string vsId, tm te) {
    tm t = minusHours(qk->gi, 2);
    if(qi->stopId == vsId) {
      return te;
    } else {
      std::list<StopTime> viArrivalTimes(orderStopTimesBy(vi->stopTimes, t));
      time_t wfi = LONG_MAX;
      for (auto iterator = vi->edges.begin(), end = vi->edges.end(); iterator != end; ++iterator) {
        std::string edge = *iterator;
        std::shared_ptr<Vertice> vf = getVerticeFromGraph(graph, verticesCache, edge);
        std::list<StopTime> vfDepartureTimes = getAvailableDepartures(handle, calendarDates, t, vf);
        for (auto iterator = vfDepartureTimes.begin(), end = vfDepartureTimes.end(); iterator != end; ++iterator) {
          StopTime vfDepartureTime = *iterator;
          for (auto iterator = viArrivalTimes.begin(), end = viArrivalTimes.end(); iterator != end; ++iterator) {
            StopTime viArrivalTime = *iterator;
            if((vfDepartureTime.tripId == viArrivalTime.tripId) && (vfDepartureTime.pos == (viArrivalTime.pos - 1)) && datetimeIsBeforeNotEq(vfDepartureTime.departure, viArrivalTime.arrival)) {
              time_t travelTime = difftime(asTimestamp(viArrivalTime.arrival), asTimestamp(vfDepartureTime.departure));
              wfi = travelTime < wfi ? travelTime : wfi;
            }
          }
        }
      }

      if(wfi == LONG_MAX) {
        oops("Unable to compute travel time between vf to vi");
      }

      tm nextEarliestArrivalTime = asDateTime(asTimestamp(qk->gi) + wfi);

      std::pair<tm, tm> enlarged = { qi->ti, qi->gi };
      for (auto iterator = giFunc.begin(), end = giFunc.end(); iterator != end; ++iterator) {
        time_t ti = iterator->first;
        tm gi = iterator->second.arrival;
        if(datetimeIsBeforeEq(gi, nextEarliestArrivalTime) && datetimeIsBeforeEq(enlarged.second, gi)) {
          enlarged = { asDateTime(ti), gi };
        }
      }

      if(hasSameDateTime(enlarged.second, qi->gi)) {
        auto last = std::prev(giFunc.end());
        return asDateTime(last->first);
      }

      return enlarged.first;
    }
  }

  ArrivalTime stopTime2ArrivalTime(std::string stopId, const StopTime *stopTime) {
    ArrivalTime arrivalTime;
    arrivalTime.stopId = stopId;
    arrivalTime.arrival = stopTime->arrival;
    arrivalTime.departure = stopTime->departure;
    arrivalTime.tripId = stopTime->tripId;
    arrivalTime.pos = stopTime->pos;
    return arrivalTime;
  }

  std::list<tm> getStartingPeriod(sqlite3 *handle, CalendarDates *calendarDates, std::shared_ptr<Vertice> vs, tm ts, tm te, int maxStartingTimes) {
    auto departures = getAvailableDepartures(handle, calendarDates, ts, vs);

    std::list<tm> startingPeriod;
    for (auto iterator = departures.begin(), end = departures.end(); iterator != end; ++iterator) {
      StopTime departureTime = *iterator;
      //printf("-> %s\n", formatDateTime(iterator->departure).c_str());
      if(datetimeIsBeforeEq(departureTime.departure, te) && (startingPeriod.size() < maxStartingTimes)) {
        //printf("pushed %s \n", iterator->tripId.c_str());
        startingPeriod.push_back(departureTime.departure);
      }
    }

    if(startingPeriod.empty()) {
      return {};
    } else if(startingPeriod.size() == 1) {
      return { *startingPeriod.begin(), *startingPeriod.begin() };
    } else {
      startingPeriod.sort([](const tm &a, const tm &b) {
          return datetimeIsBeforeEq(a, b);
        });
      return { *startingPeriod.begin(), *next(startingPeriod.begin()) };
    }
  }

  bool isQueueItemOutdated(std::unordered_map<std::string, tm> *uptodate, std::shared_ptr<QueueItem> item) {
    auto last = uptodate->find(item->stopId);
    if(last != uptodate->end()) {
      return !datetimeIsBeforeEq(item->gi, last->second);
    } else {
      return false;
    }
  }

  StopTime getEarliestArrivalTime(sqlite3 *handle, CalendarDates *calendarDates, std::shared_ptr<Vertice> vi, std::shared_ptr<Vertice> vj, ArrivalTime *gi) {
    std::list<StopTime> viDepartures = getAvailableDepartures(handle, calendarDates, gi->arrival, vi);
    std::list<StopTime> vjStopTimes(orderStopTimesBy(vj->stopTimes, gi->arrival));
    StopTime earliestArrivalTime;
    earliestArrivalTime.arrival = infinite();
    for(auto iterator = viDepartures.begin(), end = viDepartures.end(); iterator != end; ++iterator) {
      StopTime viDepartureTime = *iterator;
      for(auto iterator = vjStopTimes.begin(), end = vjStopTimes.end(); iterator != end; ++iterator) {
        StopTime vjStopTime = *iterator;
        if(viDepartureTime.tripId == vjStopTime.tripId && datetimeIsBeforeEq(viDepartureTime.departure, vjStopTime.arrival) && datetimeIsBeforeEq(gi->arrival, viDepartureTime.departure)) {
          if(datetimeIsBeforeNotEq(vjStopTime.arrival, earliestArrivalTime.arrival)) {
            earliestArrivalTime = vjStopTime;
          }
        }
      }
    }

    return earliestArrivalTime;
  }

  void updateArrivalTimeFunc(sqlite3 *handle, Graph *graph, VerticesCache *verticesCache, CalendarDates *calendarDates, ArrivalTimesFunc *arrivalTimesFunc, std::shared_ptr<Vertice> vi, ArrivalTime *gi, std::string vjId, tm startingTime, std::function<void(StopTime)> done) {
    std::shared_ptr<Vertice> vj = getVerticeFromGraph(graph, verticesCache, vjId);
    StopTime vjStopTime = getEarliestArrivalTime(handle, calendarDates, vi, vj, gi);
    if(!hasSameDateTime(vjStopTime.arrival, infinite())) { // MAYBE TODAY, ONE EDGE ISN'T AVAILABLE
      ArrivalTimeFunc gjFunc;
      time_t t = asTimestamp(startingTime);
      auto it = arrivalTimesFunc->find(vjId);
      if(it != arrivalTimesFunc->end()) {
        gjFunc = it->second;
        auto currentGj = gjFunc.find(t);
        if(currentGj != gjFunc.end()) {
          if(datetimeIsBeforeNotEq(vjStopTime.arrival, currentGj->second.arrival)) { // UPDATING IF BETTER FOUND
            gjFunc[t] = stopTime2ArrivalTime(vjId, &vjStopTime);
            done(vjStopTime);
          }
        } else {
          gjFunc[t] = stopTime2ArrivalTime(vjId, &vjStopTime); // NEW VALUE
          done(vjStopTime);
        }
      } else {
        gjFunc[t] = stopTime2ArrivalTime(vjId, &vjStopTime); // NEW FUNC
        (*arrivalTimesFunc)[vjId] = gjFunc;
        done(vjStopTime);
      }
      (*arrivalTimesFunc)[vjId] = gjFunc;
    }
  }

  ArrivalTimesFunc refineArrivalTimes(sqlite3 *handle, Graph *graph, VerticesCache *verticesCache, CalendarDates *calendarDates, std::string vsId, std::string veId, tm ts, tm te, int maxStartingTimes) {
    Queue queue;
    ArrivalTimesFunc arrivalTimesFunc;
    std::unordered_map<std::string, tm> uptodate;
    std::shared_ptr<Vertice> vs = getVerticeFromGraph(graph, verticesCache, vsId);
    std::list<tm> startingPeriod = getStartingPeriod(handle, calendarDates, vs, ts, te, maxStartingTimes);

    if(startingPeriod.empty()) {
      return arrivalTimesFunc;
    }

    ts = *startingPeriod.begin();
    te = *(std::prev(startingPeriod.end()));

    //printf("####--> %lu StartingTimes %s to %s\n", startingPeriod.size(), formatDateTime(ts).c_str(), formatDateTime(te).c_str());

    std::unordered_map<std::string, std::shared_ptr<QueueItem>> items = initTimeRefinement(handle, graph, &arrivalTimesFunc, calendarDates, &queue, vs, ts, startingPeriod);

    while(queue.size() >= 2) {
      std::shared_ptr<QueueItem> qi = queue.top();
      std::shared_ptr<Vertice> vi = getVerticeFromGraph(graph, verticesCache, qi->stopId);
      queue.pop();

      //printf("@@@@@@@@@@@@@@@@ Trip %s Stop %s - %s\n", qi->tripId.c_str(), qi->stopId.c_str(), formatDateTime(qi->gi).c_str());

      if(!isQueueItemOutdated(&uptodate, qi)) {
        std::shared_ptr<QueueItem> qk = queue.top();
        ArrivalTimeFunc giFunc = arrivalTimesFunc[vi->id];
        tm enlargedStartingTime = ts;
        if(!hasSameDateTime(ts, te)) {
          enlargedStartingTime = enlargeStartingTime(handle, calendarDates, graph, verticesCache, giFunc, qi, qk, vi, vsId, te);
        }
        for (auto iterator = vi->edges.begin(), end = vi->edges.end(); iterator != end; ++iterator) {
          std::string vjId = *iterator;
          std::shared_ptr<Vertice> vj = getVerticeFromGraph(graph, verticesCache, vjId);
          if(vj->id != vsId) {
            for (auto iterator = startingPeriod.begin(), end = startingPeriod.end(); iterator != end; ++iterator) {
              tm startingTime = *iterator;
              if(datetimeIsBeforeEq(qi->ti, startingTime) && datetimeIsBeforeEq(startingTime, enlargedStartingTime)) {
                ArrivalTime gi = giFunc[asTimestamp(startingTime)];
                updateArrivalTimeFunc(handle, graph, verticesCache, calendarDates, &arrivalTimesFunc, vi, &gi, vjId, startingTime, [&vjId, &queue, &uptodate, &items, &startingTime](StopTime vjStopTime) {
                    std::shared_ptr<QueueItem> updatedQj {new QueueItem};
                    updatedQj->stopId = vjId;
                    updatedQj->ti = items[vjId]->ti;
                    updatedQj->gi = vjStopTime.arrival;
                    updatedQj->tripId = vjStopTime.tripId;
                    queue.push(updatedQj);
                    uptodate[vjId] = vjStopTime.arrival;
                  });
              }
            }
          }
        }

        if(datetimeIsBeforeEq(te, enlargedStartingTime)) {
          if(vi->id == veId) {
            return arrivalTimesFunc;
          }
        } else {
          qi->ti = enlargedStartingTime;
          qi->gi = giFunc[asTimestamp(enlargedStartingTime)].arrival;
          queue.push(qi);
        }
      }
    };

    return arrivalTimesFunc;
  }

  time_t getOptimalStartingTime(ArrivalTimeFunc *geFunc, std::string veId) {
    std::pair<time_t, cheminotc::ArrivalTime> seed = *(geFunc->begin());
    auto best = std::accumulate(std::next(geFunc->begin()), geFunc->end(), seed, [](std::pair<time_t, cheminotc::ArrivalTime> acc, std::pair<time_t, cheminotc::ArrivalTime> point) {
      if(datetimeIsBeforeEq(point.second.arrival, acc.second.arrival)) {
        return point;
      } else {
        return acc;
      }
    });
    return best.first;
  }

  std::list<ArrivalTime> pathSelection(Graph *graph, VerticesCache *verticesCache, ArrivalTimesFunc *arrivalTimesFunc, tm ts, std::string vsId, std::string veId) {
    std::list<ArrivalTime> path;
    if(arrivalTimesFunc->empty()) {
      return path;
    }

    ArrivalTimeFunc geFunc = (*arrivalTimesFunc)[veId];
    time_t optimalStartingTime = getOptimalStartingTime(&geFunc, veId);
    std::shared_ptr<Vertice> vj = getVerticeFromGraph(graph, verticesCache, veId);

    std::function<bool (std::string, ArrivalTime*)> getArrivalTimeAt = [&arrivalTimesFunc, &optimalStartingTime](std::string viId, ArrivalTime *arrivalTime) {
      auto it = arrivalTimesFunc->find(viId);
      if(it != arrivalTimesFunc->end()) {
        ArrivalTimeFunc arrivalTimeFunc = (*arrivalTimesFunc)[viId];
        auto it = arrivalTimeFunc.find(optimalStartingTime);
        if(it != arrivalTimeFunc.end()) {
          *arrivalTime = it->second;
          return true;
        } else {
          return false;
        }
      } else {
        return false;
      }
    };

    ArrivalTime ge;
    if(getArrivalTimeAt(veId, &ge)) {
      path.push_back(ge);
    }

    while(vj->id != vsId) {
      ArrivalTime gj;
      if(getArrivalTimeAt(vj->id, &gj)) {
        for (auto iterator = vj->edges.begin(), end = vj->edges.end(); iterator != end; ++iterator) {
          std::string viId = *iterator;
          ArrivalTime gi;
          if(getArrivalTimeAt(viId, &gi)) {
            std::shared_ptr<Vertice> vi = getVerticeFromGraph(graph, verticesCache, viId);
            auto it = std::find_if(vi->stopTimes.begin(), vi->stopTimes.end(), [&gj](const StopTime &viStopTime) {
              return (viStopTime.tripId == gj.tripId) && (viStopTime.pos == (gj.pos - 1));
            });
            if(it != vi->stopTimes.end()) {
              if(viId == vsId) { // RECOVER DEPARTURE
                gi.tripId = gj.tripId;
                gi.departure = it->departure;
                gi.arrival = it->arrival;
                if(dateIsBeforeNotEq(gi.departure, gj.arrival) && timeIsBeforeNotEq(gi.departure, gj.arrival)) {
                  gi.departure.tm_mday = gj.arrival.tm_mday;
                }
                if(dateIsBeforeNotEq(gi.arrival, gi.departure) && timeIsBeforeEq(gi.departure, gj.arrival)) {
                  gi.arrival.tm_mday = gi.departure.tm_mday;
                }
              }
              path.push_front(gi);
              vj = getVerticeFromGraph(graph, verticesCache, gi.stopId);
              break;
            }
          }
        }
      }
    };

    return path;
  }

  std::list<ArrivalTime> lookForBestTrip(sqlite3 *handle, Graph *graph, VerticesCache *verticesCache, CalendarDates *calendarDates, std::string vsId, std::string veId, tm ts, tm te, int maxStartingTimes) {
    auto arrivalTimes = refineArrivalTimes(handle, graph, verticesCache, calendarDates, vsId, veId, ts, te, maxStartingTimes);
    return pathSelection(graph, verticesCache, &arrivalTimes, ts, vsId, veId);
  }
}
