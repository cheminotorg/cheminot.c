#include <sqlite3.h>
#include <map>
#include <list>
#include <memory>
#include "protobuf/cheminotBuf.pb.h"
#include <json/json.h>
#include <unordered_map>
#include <iterator>
#include <numeric>

namespace cheminotc
{

    typedef google::protobuf::Map<std::string,m::cheminot::data::Vertice> GraphBuf;
    typedef google::protobuf::Map<std::string,m::cheminot::data::CalendarExceptions> CalendarDatesBuf;

    struct Graph {
        std::list<std::shared_ptr<GraphBuf>> data;
        std::map<std::string, std::shared_ptr<m::cheminot::data::Vertice>> _data;

        Graph() {
        }

        Graph(std::list<std::shared_ptr<GraphBuf>> data) {
            this->data = data;
        }

        m::cheminot::data::Vertice& get(std::string key) {
            auto _it = this->_data.find(key);
            if(_it != this->_data.end()) {
                return *_it->second;
            } else {
                m::cheminot::data::Vertice *h = nullptr;
                for(auto &graphBuf : this->data) {
                    auto it = graphBuf->find(key);
                    if(it != graphBuf->end()) {
                        if(h == nullptr) {
                            h = &it->second;
                        } else {
                            h->MergeFrom(it->second);
                        }
                    }
                }

                if(h == nullptr) {
                    throw std::runtime_error("Unable to find vertice for " + key);
                } else {
                    this->_data[key] = std::make_shared<m::cheminot::data::Vertice>(*h);
                    return *h;
                }
            }
        }

        m::cheminot::data::Vertice& operator[](const std::string &key) {
            return this->get(key);
        }

        void foreach(std::function<void(const std::string&, const m::cheminot::data::Vertice&)> loop) {
            std::map<std::string, bool> xxx;
            for(auto &graphBuf : this->data) {
                for(auto &vertice : *graphBuf) {
                    auto it = xxx.find(vertice.first);
                    if(it == xxx.end()) {
                        loop(vertice.first, this->get(vertice.first));
                        xxx[vertice.first] = true;
                    }
                }
            }
        }

        GraphBuf::iterator end() {
            return this->data.back()->end();
        }

        GraphBuf::iterator find(const std::string &key) {
            for(auto &graphBuf : this->data) {
                auto it = graphBuf->find(key);
                if(it != graphBuf->end()) {
                    return it;
                }
            }
            return this->end();
        }

        bool empty() const {
            for(auto &graphBuf : this->data) {
                if(graphBuf->empty()) {
                    return false;
                }
            }
            return true;
        }

        size_t size() const {
            return std::accumulate(this->data.begin(), this->data.end(), 0, [](int acc, std::shared_ptr<GraphBuf> graphBuf) {
                return acc + graphBuf->size();
            });
        }
    };

    struct CalendarDates {
        std::list<std::shared_ptr<CalendarDatesBuf>> data;

        CalendarDates() {
        }

        CalendarDates(std::list<std::shared_ptr<CalendarDatesBuf>> data) {
            this->data = data;
        }

        m::cheminot::data::CalendarExceptions& operator[](const std::string &key) {
            for(auto &calendarDatesBuf : this->data) {
                auto it = calendarDatesBuf->find(key);
                if(it != calendarDatesBuf->end()) {
                    return it->second;
                }
            }
            throw std::runtime_error("Unable to find calendar exceptions for " + key);
        }

        void foreach(std::function<void(const std::string&, const m::cheminot::data::CalendarExceptions&)> loop) {
            for(auto &calendarDatesBuf : this->data) {
                for(auto &exceptions : *calendarDatesBuf) {
                    loop(exceptions.first, exceptions.second);
                }
            }
        }

        CalendarDatesBuf::iterator end() {
            return this->data.back()->end();
        }

        CalendarDatesBuf::iterator find(const std::string &key) {
            for(auto &calendarDatesBuf : this->data) {
                auto it = calendarDatesBuf->find(key);
                if(it != calendarDatesBuf->end()) {
                    return it;
                }
            }
            return this->end();
        }

        bool empty() const {
            for(auto &calendarDatesBuf : this->data) {
                if(calendarDatesBuf->empty()) {
                    return false;
                }
            }
            return true;
        }

        size_t size() const {
            return std::accumulate(this->data.begin(), this->data.end(), 0, [](int acc, std::shared_ptr<CalendarDatesBuf> calendarDatesBuf) {
                return acc + calendarDatesBuf->size();
            });
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

    void parseGraphFiles(std::list<std::string> paths, Graph &graph);

    void parseCalendarDatesFiles(std::list<std::string> paths, CalendarDates &calendarDates);

    std::tuple<bool, ArrivalTimesFunc, std::string> refineArrivalTimes(const CheminotDb &connection, Graph *graph, Cache *cache, CalendarDates *calendarDates, std::string vsId, std::string veId, tm ts, tm te, int max);

    std::pair<bool, std::list<ArrivalTime>> lookForBestDirectTrip(const CheminotDb &connection, const std::list<std::string> &subsets, Graph &graph, Cache &cache, CalendarDates &calendarDates, const std::string &vsId, const std::string &veId, const tm &ts, const tm &te);

    std::pair<bool, std::list<ArrivalTime>> lookForBestTrip(const CheminotDb &connection, Graph &graph, Cache &cache, CalendarDates &calendarDates, const std::string &vsId, const std::string &veId, const tm &ts, const tm &te, int max);

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

    Json::Value serializeArrivalTimes(Graph &graph, Cache &cache, const std::list<ArrivalTime> &arrivalTimes);

    Json::Value serializeArrivalTime(Graph &graph, Cache &cache, const ArrivalTime &arrivalTime);

    bool verticeExists(Graph *graph, Cache *cache, std::string id);

    Vertice getVerticeFromGraph(Graph &graph, Cache &cache, std::string id, const tm *dateref, bool withStopTimes = true);

    Json::Value getMeta(const CheminotDb &connection);

    void fillCache(Cache &cache, CalendarDates &calendarDates, Graph &graph);

// -- PARIS
    static std::string PARIS_STOP_ID = "PARISXX";

    static std::list<std::string> PARIS_STOP_IDS =
    {
        PARIS_STOP_ID,
        "8739110",
        "8739100",
        "8768666",
        "8768600",
        "8711300",
        "8727100",
        "8738400",
        "8754700"
    };
}
