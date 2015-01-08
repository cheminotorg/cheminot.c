#include <sqlite3.h>
#include <map>
#include <list>
#include <memory>
#include "protobuf/cheminotBuf.pb.h"
#include <json/json.h>

namespace cheminotc {

  typedef google::protobuf::Map< std::string,m::cheminot::data::Vertice> Graph;
  typedef google::protobuf::Map<std::string,m::cheminot::data::CalendarExceptions> CalendarDates;

  struct StopTime {
    std::string tripId;
    tm arrival;
    tm departure;
    int pos;
  };

  struct Vertice {
    std::string id;
    std::string name;
    std::list<std::string> edges;
    std::list<StopTime> stopTimes;
  };

  struct ArrivalTime {
    std::string stopId;
    tm arrival;
    tm departure;
    std::string tripId;
    int pos;
  };

  struct CalendarDate {
    std::string serviceId;
    tm date;
    int exceptionType;
  };

  struct Calendar {
    std::string serviceId;
    std::map<std::string, bool> week;
    tm startDate;
    tm endDate;
  };

  struct Trip {
    std::string id;
    std::unique_ptr<Calendar> calendar;
    std::string direction;
  };

  tm getNow();

  sqlite3* openConnection(std::string path);

  std::string getVersion(sqlite3 *handle);

  std::list<Trip> getTripsByIds(sqlite3 *handle, std::list<std::string> ids);

  void parseGraph(std::string path, Graph *graph);

  void parseCalendarDates(std::string content, CalendarDates *calendarDates);

  std::list<ArrivalTime> lookForBestTrip(sqlite3 *handle, Graph *graph, CalendarDates *calendarDates, std::string vsId, std::string veId, tm at);

  std::string formatTime(tm time);

  std::string formatDate(tm time);

  std::string formatDateTime(tm datetime);

  tm asDateTime(time_t t);

  Json::Value serializeArrivalTimes(std::list<ArrivalTime> arrivalTimes);

  Json::Value serializeArrivalTime(ArrivalTime arrivalTime);

  Json::Value serializeStopTimes(std::list<StopTime> stopTimes);

  Json::Value serializeEdges(std::list<std::string> edges);

  std::map<std::string, std::string> f();
}
