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

  void parseGraphFromFile(std::string path, Graph *graph);

  void parseGraph(std::string content, Graph *graph);

  void parseCalendarDatesFromFile(std::string content, CalendarDates *calendarDates);

  void parseCalendarDates(std::string content, CalendarDates *calendarDates);

  std::list<ArrivalTime> lookForBestTrip(sqlite3 *handle, Graph *graph, CalendarDates *calendarDates, std::string vsId, std::string veId, struct tm at);

  std::string formatTime(struct tm time);

  std::string formatDate(struct tm time);

  struct tm asDateTime(time_t t);

  Json::Value serializeArrivalTimes(std::list<ArrivalTime> arrivalTimes);
}
