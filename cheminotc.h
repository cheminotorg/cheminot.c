#include <sqlite3.h>
#include <map>
#include <list>
#include <memory>
#include "protobuf/cheminotBuf.pb.h"
#include <../jsoncpp/json/json.h>

namespace cheminotc {

  struct StopTime {
    std::string tripId;
    struct tm arrival;
    struct tm departure;
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
    struct tm arrival;
    struct tm departure;
    std::string tripId;
    Vertice vertice;
    int pos;
  };

  struct CalendarDate {
    std::string serviceId;
    struct tm date;
    int exceptionType;
  };

  struct Calendar {
    std::string serviceId;
    std::map<std::string, bool> week;
    struct tm startDate;
    struct tm endDate;
  };

  struct Trip {
    std::string id;
    std::unique_ptr<Calendar> calendar;
    std::string direction;
  };

  struct tm getNow();

  sqlite3* openConnection(std::string path);

  std::string getVersion(sqlite3 *handle);

  std::list<Trip> getTripsByIds(sqlite3 *handle, std::list<std::string> ids);

  m::cheminot::data::Graph parseGraph(std::string path);

  m::cheminot::data::CalendarDates parseCalendarDates(std::string path);

  std::list<ArrivalTime> lookForBestTrip(sqlite3 *handle, m::cheminot::data::Graph *graph, m::cheminot::data::CalendarDates *calendarDates, std::string vsId, std::string veId, struct tm at);

  std::string formatTime(struct tm time);

  std::string formatDate(struct tm time);

  struct tm asDateTime(time_t t);

  Json::Value serializeArrivalTimes(std::list<ArrivalTime> arrivalTimes);
}
