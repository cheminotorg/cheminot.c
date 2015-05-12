#include <sqlite3.h>
#include <map>
#include <list>
#include <memory>
#include "protobuf/cheminotBuf.pb.h"
#include <json/json.h>
#include <unordered_map>

namespace cheminotc
{

    typedef google::protobuf::Map< std::string,m::cheminot::data::Vertice> Graph;
    typedef google::protobuf::Map<std::string,m::cheminot::data::CalendarExceptions> CalendarDates;

    struct CheminotDb
    {
      sqlite3* file;
      sqlite3* inmemory;
    };

    struct StopTime
    {
        std::string tripId;
        tm arrival;
        tm departure;
        int pos;
    };

    struct Vertice
    {
        std::string id;
        std::string name;
        double lat;
        double lng;
        std::list<std::string> edges;
        std::list<StopTime> stopTimes;
    };

    struct ArrivalTime
    {
        std::string stopId;
        tm arrival;
        tm departure;
        std::string tripId;
        int pos;
        double lat;
        double lng;
    };

    struct CalendarDate
    {
        std::string serviceId;
        tm date;
        int exceptionType;
    };

    struct Calendar
    {
        std::string serviceId;
        std::unordered_map<std::string, bool> week;
        tm startDate;
        tm endDate;
    };

    struct Trip
    {
        std::string id;
        std::unique_ptr<Calendar> calendar;
        std::string direction;
        std::list<std::string> stopIds;
    };

    typedef std::map<time_t, ArrivalTime> ArrivalTimeFunc; //TODO unordered_map

    typedef std::unordered_map<std::string, ArrivalTimeFunc> ArrivalTimesFunc;

    typedef std::unordered_map<std::string, std::shared_ptr<Vertice>> VerticesCache;

    typedef std::unordered_map<std::string, std::list<std::shared_ptr<CalendarDate>>> CalendarDatesCache;

    typedef std::unordered_map<std::string, std::shared_ptr<Trip>> TripsCache;

    struct Cache
    {
        VerticesCache vertices;
        CalendarDatesCache calendarDates;
        TripsCache trips;
    };

    struct LatLng
    {
        double lat;
        double lng;
    };

    double distance(const LatLng &a, const LatLng &b);

    tm getNow();

    CheminotDb openConnection(std::string path);

    std::string getVersion(const CheminotDb &connection);

    std::string getLastTrace(const CheminotDb &connection);

    void cleanTrace(const CheminotDb &connection, int id);

    void resetTrace(const CheminotDb &connection);

    void traceVertice(const CheminotDb &connection, const Vertice &vertice);

    void lock(const CheminotDb &connection);

    void unlock(const CheminotDb &connection);

    bool isLocked(const CheminotDb &connection, bool *locked);

    void parseGraph(std::string path, Graph *graph);

    void parseCalendarDates(std::string content, CalendarDates *calendarDates);

    std::tuple<bool, ArrivalTimesFunc, std::string> refineArrivalTimes(const CheminotDb &connection, Graph *graph, Cache *cache, CalendarDates *calendarDates, std::string vsId, std::string veId, tm ts, tm te, int max);

    std::pair<bool, std::list<ArrivalTime>> lookForBestDirectTrip(const CheminotDb &connection, Graph *graph, Cache *cache, CalendarDates *calendarDates, std::string vsId, std::string veId, tm ts, tm te);

    std::pair<bool, std::list<ArrivalTime>> lookForBestTrip(const CheminotDb &connection, Graph *graph, Cache *cache, CalendarDates *calendarDates, std::string vsId, std::string veId, tm ts, tm te, int max);

    bool hasSameDateTime(const tm &a, const tm &b);

    bool datetimeIsBeforeEq(const tm &a, const tm &b);

    bool dateIsBeforeEq(const tm &a, const tm &b);

    bool timeIsBeforeEq(const tm &a, const tm &b);

    bool datetimeIsBeforeNotEq(const tm &a, const tm &b);

    std::string formatTime(tm time);

    std::string formatDate(tm time);

    std::string formatDateTime(tm datetime);

    tm asDateTime(time_t t);

    tm addMinutes(tm datetime, int n);

    tm addDays(tm datetime, int n);

    tm addHours(tm datetime, int n);

    Json::Value serializeArrivalTimes(Graph *graph, Cache *cache, std::list<ArrivalTime> arrivalTimes);

    bool verticeExists(Graph *graph, Cache *cache, std::string id);

    Vertice getVerticeFromGraph(const tm *dateref, Graph *graph, Cache *cache, std::string id);

    Json::Value getMeta(const CheminotDb &connection);

// -- PARIS
    static std::string parisStopId = "StopPoint:OCETrain TER-PARISXXX";

    static std::list<std::string> parisStopIds =
    {
        parisStopId,
        "StopPoint:OCETrain TER-87391102",
        "StopPoint:OCETrain TER-87391003",
        "StopPoint:OCETrain TER-87686667",
        "StopPoint:OCETrain TER-87686006",
        "StopPoint:OCETrain TER-87113001",
        "StopPoint:OCETrain TER-87271007",
        "StopPoint:OCETrain TER-87384008",
        "StopPoint:OCETrain TER-87547000"
    };
}
