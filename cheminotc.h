#include <sqlite3.h>
#include <map>
#include <list>
#include <memory>
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

  struct CalendarException {
    std::string serviceId;
    struct tm date;
    int exceptionType;
  };

  struct tm getNow();

  Json::Value toJson(std::string value);

  sqlite3* openConnection(std::string path);

  std::string getVersion(sqlite3 *handle);

  Json::Value parseGraph(std::string path);

  Json::Value parseCalendarExceptions(std::string path);

  std::list<ArrivalTime> lookForBestTrip(sqlite3 *handle, Json::Value *graph, Json::Value *calendarExceptions, std::string vsId, std::string veId, struct tm at);

  Json::Value serializeArrivalTimes(std::list<ArrivalTime> arrivalTimes);

  std::string formatTime(struct tm time);

  struct tm asDateTime(time_t t);
}
