#include <sqlite3.h>
#include <map>
#include <list>
#include <memory>
#include "protobuf/cheminotBuf.pb.h"
#include <json/json.h>
#include <unordered_map>
#include <iterator>

namespace cheminotc
{

    typedef google::protobuf::Map<std::string,m::cheminot::data::Vertice> GraphBuf;
    typedef google::protobuf::Map<std::string,m::cheminot::data::CalendarExceptions> CalendarDatesBuf;


    struct Graph {
        GraphBuf ter;
        GraphBuf trans;

        Graph() {
        }

        Graph(GraphBuf ter, GraphBuf trans) {
            this->ter = ter;
            this->trans = trans;
        }

        m::cheminot::data::Vertice& operator[](const std::string &key) {
            auto itTer = this->ter.find(key);
            if(itTer != this->ter.end()) {
                return itTer->second;
            } else {
                return this->trans[key];
            }
        }

        void foreach(std::function<void(const std::string&, const m::cheminot::data::Vertice&)> loop) {
            for(auto vertice : this->ter) {
                loop(vertice.first, vertice.second);
            }

            for(auto vertice : this->trans) {
                loop(vertice.first, vertice.second);
            }
        }

        GraphBuf::iterator end() {
            return this->trans.end();
        }

        GraphBuf::iterator find(const std::string &key) {
            auto itTer = this->ter.find(key);
            if(itTer != this->ter.end()) {
                return itTer;
            } else {
                return this->trans.find(key);
            }
        }

        bool empty() const {
            return ter.empty() && trans.empty();
        }

        size_t size() const {
            return this->ter.size() + this->trans.size();
        }
    };

    struct CalendarDates {
        CalendarDatesBuf ter;
        CalendarDatesBuf trans;

        CalendarDates() {
        }

        CalendarDates(CalendarDatesBuf ter, CalendarDatesBuf trans) {
            this->ter = ter;
            this->trans = trans;
        }

        m::cheminot::data::CalendarExceptions& operator[](const std::string &key) {
            auto itTer = this->ter.find(key);
            if(itTer != this->ter.end()) {
                return itTer->second;
            } else {
                return this->trans[key];
            }
        }

        void foreach(std::function<void(const std::string&, const m::cheminot::data::CalendarExceptions&)> loop) {
            for(auto exception : this->ter) {
                loop(exception.first, exception.second);
            }

            for(auto exception : this->trans) {
                loop(exception.first, exception.second);
            }
        }

        CalendarDatesBuf::iterator end() {
            return this->trans.end();
        }

        CalendarDatesBuf::iterator find(const std::string &key) {
            auto itTer = this->ter.find(key);
            if(itTer != this->ter.end()) {
                return itTer;
            } else {
                return this->trans.find(key);
            }
        }

        bool empty() const {
            return ter.empty() && trans.empty();
        }

        size_t size() const {
            return this->ter.size() + this->trans.size();
        }
    };

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
        bool readonly;
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

    void closeConnection(CheminotDb cheminotDb);

    std::string getVersion(const CheminotDb &connection);

    std::string getLastTrace(const CheminotDb &connection);

    void cleanTrace(const CheminotDb &connection, int id);

    void resetTrace(const CheminotDb &connection);

    void traceVertice(const CheminotDb &connection, const Vertice &vertice);

    void lock(const CheminotDb &connection);

    void unlock(const CheminotDb &connection);

    bool isLocked(const CheminotDb &connection, bool *locked);

    void parseGraphFiles(std::string terPath, std::string transPath, Graph *graph);

    void parseCalendarDatesFiles(std::string terPath, std::string transPath, CalendarDates *calendarDates);

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

    Vertice getVerticeFromGraph(Graph *graph, Cache *cache, std::string id, const tm *dateref, bool withStopTimes = true);

    Json::Value getMeta(const CheminotDb &connection);

    void fillCache(Cache *cache, CalendarDates *calendarDates, Graph *graph);

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
