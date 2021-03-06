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
#include <unordered_map>
#include "cheminotc.h"

namespace cheminotc
{

    template <typename T>
    std::string to_string(T value)
    {
        std::ostringstream os ;
        os << value ;
        return os.str();
    }

    void oops(std::string message)
    {
        throw std::runtime_error(message);
    }

    double distance(const LatLng &a, const LatLng &b)
    {
        double radius = 6371;
        double nDLat = (b.lat - a.lat) * (M_PI/180);
        double nDLon = (b.lng - a.lng) * (M_PI/180);

        double nLatA =  a.lat * (M_PI/180);
        double nLatB =  b.lat * (M_PI/180);

        double nA = pow ( sin(nDLat/2), 2 ) +
            cos(nLatA) * cos(nLatB) *
            pow ( sin(nDLon/2), 2 );

        double nC = 2 * atan2( sqrt(nA), sqrt( 1 - nA ));
        return radius * nC;
    }

    bool isSubwayTrip(const std::string &stopId)
    {
        std::string subwayTripPrefix = "SUBWAY:";
        return stopId.find(subwayTripPrefix) == 0;
    }

    bool isParis(std::string id)
    {
        auto it = std::find_if(PARIS_STOP_IDS.begin(), PARIS_STOP_IDS.end(), [&id](std:: string vid)
        {
            return id == vid;
        });
        return it != PARIS_STOP_IDS.end();
    };

    tm getNow()
    {
        time_t rawtime;
        time(&rawtime);
        tm *info = localtime(&rawtime);
        return *info;
    }

    std::string formatTime(tm time)
    {
        return to_string(time.tm_hour) + ":" + to_string(time.tm_min);
    }

    std::string formatDate(tm time)
    {
        return to_string(time.tm_mday) + "/" + to_string(time.tm_mon + 1) + "/" + to_string(time.tm_year);
    }

    std::string formatDateTime(tm datetime)
    {
        return formatDate(datetime) + " " + formatTime(datetime);
    }

    tm parseTime(tm dateref, std::string datetime)
    {
        tm time;
        strptime(datetime.c_str(), "%H:%M", &time);
        dateref.tm_hour = time.tm_hour;
        dateref.tm_min = time.tm_min;
        dateref.tm_sec = 0;
        return dateref;
    }

    tm parseDate(std::string datetime)
    {
        tm date;
        strptime(datetime.c_str(), "%d/%m/%Y", &date);
        date.tm_hour = 0;
        date.tm_min = 0;
        date.tm_sec = 0;
        return date;
    }

    tm asDateTime(time_t t)
    {
        tm dateTime;
        return *(localtime (&t));
    }

    time_t asTimestamp(tm a)
    {
        time_t timestamp = mktime(&a);
        return timestamp;
    }

    tm addSeconds(tm datetime, int n)
    {
        datetime.tm_sec += n;
        mktime(&datetime);
        return datetime;
    }

    tm minusHours(tm datetime, int n)
    {
        datetime.tm_hour -= n;
        mktime(&datetime);
        return datetime;
    }

    tm addHours(tm datetime, int n)
    {
        datetime.tm_hour += n;
        mktime(&datetime);
        return datetime;
    }


    tm addDays(tm datetime, int n)
    {
        datetime.tm_mday += n;
        mktime(&datetime);
        return datetime;
    }

    bool hasSameTime(const tm &a, const tm &b)
    {
        return (a.tm_hour == b.tm_hour) && (a.tm_min == b.tm_min);
    }

    bool hasSameDate(const tm &a, const tm &b)
    {
        return (a.tm_year == b.tm_year) && (a.tm_mon == b.tm_mon) && (a.tm_mday == b.tm_mday);
    }

    bool hasSameDateTime(const tm &a, const tm &b)
    {
        return hasSameTime(a, b) && hasSameDate(a, b);
    }

    bool timeIsBeforeEq(const tm &a, const tm &b)
    {
        if(a.tm_hour > b.tm_hour)
        {
            return false;
        }
        else if(a.tm_hour < b.tm_hour)
        {
            return true;
        }
        else
        {
            if(a.tm_min > b.tm_min)
            {
                return false;
            }
            else
            {
                return true;
            }
        }
    }

    bool timeIsBeforeNotEq(const tm &a, const tm &b)
    {
        return timeIsBeforeEq(a, b) && !hasSameTime(a, b);
    }

    bool dateIsBeforeEq(const tm &a, const tm &b)
    {
        if(a.tm_year > b.tm_year)
        {
            return false;
        }
        else if(a.tm_year < b.tm_year)
        {
            return true;
        }
        else
        {
            if(a.tm_mon > b.tm_mon)
            {
                return false;
            }
            else if(a.tm_mon < b.tm_mon)
            {
                return true;
            }
            else
            {
                if(a.tm_mday > b.tm_mday)
                {
                    return false;
                }
                else
                {
                    return true;
                }
            }
        }
    }

    bool dateIsBeforeNotEq(const tm &a, const tm &b)
    {
        return dateIsBeforeEq(a, b) && !hasSameDate(a, b);
    }

    bool datetimeIsBeforeEq(const tm &a, const tm &b)
    {
        if(dateIsBeforeEq(a, b))
        {
            if(hasSameDate(a, b))
            {
                return timeIsBeforeEq(a, b);
            }
            else
            {
                return true;
            }
        }
        else
        {
            return false;
        }
    }

    bool datetimeIsBeforeNotEq(const tm &a, const tm &b)
    {
        return datetimeIsBeforeEq(a, b) && !hasSameDateTime(a, b);
    }

    Json::Value serializeVertice(const Vertice &vertice)
    {
        Json::Value json;
        json["id"] = vertice.id;
        json["name"] = vertice.name;
        json["lat"] = vertice.lat;
        json["lng"] = vertice.lng;
        return json;
    }

    Json::Value serializeArrivalTime(Graph &graph, Cache &cache, const ArrivalTime &arrivalTime)
    {
        Json::Value json;
        int arrival = asTimestamp(arrivalTime.arrival);
        int departure = asTimestamp(arrivalTime.departure);
        Vertice vi = cheminotc::getVerticeFromGraph(graph, cache, arrivalTime.stopId, nullptr, false);
        json["stopId"] = arrivalTime.stopId;
        json["stopName"] = vi.name;
        json["arrival"] = arrival;
        json["departure"] = departure;
        json["tripId"] = arrivalTime.tripId;
        json["pos"] = arrivalTime.pos;
        json["lat"] = arrivalTime.lat;
        json["lng"] = arrivalTime.lng;
        return json;
    }

    Json::Value serializeArrivalTimes(Graph &graph, Cache &cache, const std::list<ArrivalTime> &arrivalTimes)
    {
        Json::Value array = Json::Value(Json::arrayValue);
        for(const ArrivalTime &arrivalTime : arrivalTimes)
        {
            array.append(serializeArrivalTime(graph, cache, arrivalTime));
        }
        return array;
    }

    Json::Value serializeStopTime(const StopTime &stopTime)
    {
        Json::Value json;
        int arrival = asTimestamp(stopTime.arrival);
        int departure = asTimestamp(stopTime.departure);
        json["tripId"] = stopTime.tripId;
        json["arrival"] = arrival;
        json["departure"] = departure;
        json["pos"] = stopTime.pos;
        return json;
    }

    Json::Value serializeStopTimes(std::list<StopTime> &stopTimes)
    {
        Json::Value array;
        for (const StopTime &stopTime : stopTimes)
        {
            array.append(serializeStopTime(stopTime));
        }
        return array;
    }

    std::string formatStopTime(const StopTime &stopTime)
    {
        Json::Value serialized = serializeStopTime(stopTime);
        return serialized.toStyledString();
    }

    std::string formatStopTimes(std::list<StopTime> stopTimes)
    {
        return serializeStopTimes(stopTimes).toStyledString();
    }

    bool isTerminus(const StopTime &a)
    {
        return hasSameTime(a.arrival, a.departure) && a.pos > 0;
    }

    std::list<std::shared_ptr<CalendarDate>> getCalendarDatesByServiceId(Cache &cache, CalendarDates &calendarDates, std::string serviceId)
    {
        auto it = cache.calendarDates.find(serviceId);
        if(it != cache.calendarDates.end())
        {
            return it->second;
        }
        else
        {
            std::list<std::shared_ptr<CalendarDate>> results;
            auto exceptions = calendarDates[serviceId].calendardates();
            for (const auto &calendarDateBuf : exceptions)
            {
                std::shared_ptr<CalendarDate> calendarDate {new CalendarDate};
                calendarDate->serviceId = calendarDateBuf.serviceid();
                calendarDate->date = parseDate(calendarDateBuf.date());
                calendarDate->exceptionType = calendarDateBuf.exceptiontype();
                results.push_back(calendarDate);
            }
            if(!cache.readonly) {
                cache.calendarDates[serviceId] = results;
            }
            return results;
        }
    }

    StopTime parseStopTime(const tm *dateref, const m::cheminot::data::StopTime &stopTimeBuf)
    {
        StopTime stopTime;
        stopTime.tripId = stopTimeBuf.tripid();
        stopTime.arrival = parseTime(*dateref, stopTimeBuf.arrival());
        stopTime.departure = parseTime(*dateref, stopTimeBuf.departure());
        stopTime.pos = stopTimeBuf.pos();
        if(isSubwayTrip(stopTime.tripId))
        {
            stopTime.departure = *dateref;
            stopTime.arrival = *dateref;
        }
        else if(timeIsBeforeEq(stopTime.departure, *dateref))
        {
            stopTime.arrival = addDays(stopTime.arrival, 1);
            stopTime.departure = addDays(stopTime.departure, 1);
        }
        return stopTime;
    }

    std::list<StopTime> parseStopTimes(const tm *dateref, const google::protobuf::RepeatedPtrField< ::m::cheminot::data::StopTime> &stopTimesBuf)
    {
        std::list<StopTime> stopTimes;
        for(const auto &stopTimeBuf : stopTimesBuf)
        {
            StopTime stopTime = parseStopTime(dateref, stopTimeBuf);
            stopTimes.push_back(stopTime);
        }
        return stopTimes;
    }

    std::list<std::string> parseTripStopIds(const m::cheminot::data::TripStopIds &tripStopIdsBuf)
    {
        std::list<std::string> result;
        auto stopIds = tripStopIdsBuf.stopids();
        for(const std::string &stopId : stopIds)
        {
            result.push_back(stopId);
        }
        return result;
    }

    std::list<std::string> parseEdges(const google::protobuf::RepeatedPtrField< std::string> &edgesBuf)
    {
        std::list<std::string> edges;
        for(const std::string &edge : edgesBuf)
        {
            edges.push_back(edge);
        }
        return edges;
    }

    std::unique_ptr<Calendar> parseCalendar(const m::cheminot::data::Calendar &calendarBuf)
    {
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

    std::list<StopTime> orderStopTimesBy(const std::list<StopTime> &stopTimes, const tm &t)
    {
        std::list<StopTime> stopTimesAt;
        for (StopTime stopTime : stopTimes)
        {
            stopTime.departure.tm_mday = t.tm_mday;
            stopTime.departure.tm_wday = t.tm_wday;
            stopTime.departure.tm_yday = t.tm_yday;
            stopTime.departure.tm_mon = t.tm_mon;
            stopTime.departure.tm_year = t.tm_year;
            stopTime.arrival.tm_mday = t.tm_mday;
            stopTime.arrival.tm_wday = t.tm_wday;
            stopTime.arrival.tm_yday = t.tm_yday;
            stopTime.arrival.tm_mon = t.tm_mon;
            stopTime.arrival.tm_year = t.tm_year;

            if(isSubwayTrip(stopTime.tripId))
            {
                stopTime.departure = t;
                stopTime.arrival = t;
            }
            else if(datetimeIsBeforeNotEq(stopTime.departure, t))
            {
                stopTime.departure = addDays(stopTime.departure, 1);
                stopTime.arrival = addDays(stopTime.arrival, 1);
            }
            stopTimesAt.push_back(stopTime);
        };

        stopTimesAt.sort([](const StopTime &a, const StopTime &b)
        {
            return datetimeIsBeforeEq(a.departure, b.departure);
        });

        return stopTimesAt;
    }

    bool verticeExists(Graph *graph, Cache *cache, std::string id)
    {
        auto cacheIt = cache->vertices.find(id);
        if(cacheIt != cache->vertices.end())
        {
            return true;
        }
        else
        {
            auto it = graph->find(id);
            return it != graph->end();
        }
    }

    Vertice getVerticeFromGraph(Graph &graph, Cache &cache, std::string id, const tm *dateref, bool withStopTimes)
    {
        tm datetime;
        if(dateref == nullptr) {
            datetime = getNow();
        } else {
            datetime = *dateref;
        }

        auto it = cache.vertices.find(id);
        if(it != cache.vertices.end())
        {
            Vertice vertice = *it->second;
            if(withStopTimes)
            {
                vertice.stopTimes = orderStopTimesBy(vertice.stopTimes, datetime);
            }
            return vertice;
        }
        else
        {
            std::shared_ptr<Vertice> vertice {new Vertice};
            auto verticeBuf = graph[id];
            vertice->id = verticeBuf.id();
            vertice->name = verticeBuf.name();
            vertice->lat = verticeBuf.lat();
            vertice->lng = verticeBuf.lng();
            vertice->edges = parseEdges(verticeBuf.edges());
            if(withStopTimes)
            {
                vertice->stopTimes = parseStopTimes(&datetime, verticeBuf.stoptimes());
                if(!cache.readonly) {
                    cache.vertices[id] = vertice;
                }
            }

            return *vertice;
        }
    }

    std::shared_ptr<Trip> parseTripRow(std::list< std::unordered_map<std::string, const void*> >::const_iterator it)
    {
        std::unordered_map<std::string, const void*> row = *it;
        std::shared_ptr<Trip> trip {new Trip};
        trip->id = (const char*)row["id"];
        trip->direction = (const char*)row["direction"];

        const char* stopIds = (const char*)row["stopIds"];
        if(stopIds != nullptr)
        {
            m::cheminot::data::TripStopIds tripStopIdsBuf;
            tripStopIdsBuf.ParseFromString(stopIds);
            trip->stopIds = parseTripStopIds(tripStopIdsBuf);
        }

        const char *calendar = (const char*)row["calendar"];
        if(calendar != nullptr)
        {
            m::cheminot::data::Calendar calendarBuf;
            calendarBuf.ParseFromString(calendar);
            trip->calendar = parseCalendar(calendarBuf);
        }
        return trip;
    }

    std::unordered_map< std::string, const void*> parseRow(sqlite3_stmt *stmt)
    {
        int cols = sqlite3_column_count(stmt);
        std::unordered_map<std::string, const void*> row;
        for(int col=0 ; col<cols; col++)
        {
            std::string name(sqlite3_column_name(stmt, col));
            const char *value = (const char *)sqlite3_column_text(stmt, col);
            if(value != nullptr)
            {
                row[name] = strdup(value);
            }
        }
        return row;
    }

    void executeUpdate(sqlite3 *handle, std::string query)
    {
        sqlite3_stmt *stmt;
        sqlite3_exec(handle, query.c_str(), nullptr, nullptr, nullptr);
    }

    std::list< std::unordered_map<std::string, const void*> > executeQuery(sqlite3 *handle, std::string query)
    {
        std::list< std::unordered_map <std::string, const void*> > results;
        sqlite3_stmt *stmt;
        sqlite3_prepare_v2(handle, query.c_str(),-1, &stmt, 0);
        int retval;
        while(1)
        {
            retval = sqlite3_step(stmt);
            if(retval == SQLITE_ROW)
            {
                results.push_back(parseRow(stmt));
            }
            else if(retval == SQLITE_DONE)
            {
                return results;
            }
            else
            {
                oops("Unexpected error [" + to_string(retval) + "] while executing this SQL query: " + query);
            }
        }
    }

    CheminotDb openConnection(std::string path)
    {
        sqlite3 *file;
        sqlite3_open_v2(path.c_str(), &file, SQLITE_OPEN_READWRITE, nullptr);

        sqlite3 *inmemory;
        sqlite3_open_v2(":memory:", &inmemory, SQLITE_OPEN_READWRITE, nullptr);

        executeQuery(inmemory, "CREATE TABLE meta (key TEXT PRIMARY KEY, value TEXT)");

        executeQuery(inmemory, "INSERT INTO meta (key, value) VALUES('aborted', '0')");

        executeQuery(inmemory, "CREATE TABLE trace (id INTEGER PRIMARY KEY, value TEXT)");

        return { file, inmemory };
    }

    void closeConnection(CheminotDb cheminotDb)
    {
        sqlite3_close(cheminotDb.inmemory);
        sqlite3_close(cheminotDb.file);
    }

    void parseGraphFile(const std::string &path, std::shared_ptr<GraphBuf> graphBuf)
    {
        std::ifstream in(path);
        if(in.is_open())
        {
            m::cheminot::data::Graph g;
            g.ParseFromIstream(&in);
            *graphBuf = *g.mutable_vertices();
            in.close();
        }
        else
        {
            throw std::runtime_error("Unexpected error while reading: " + path);
        }
    }

    void parseGraphFiles(std::list<std::string> paths, Graph &graph)
    {
        std::list<std::shared_ptr<GraphBuf>> data;
        for(const std::string &path : paths) {
            std::shared_ptr<GraphBuf> graphBuf(new GraphBuf());
            parseGraphFile(path, graphBuf);
            data.push_back(graphBuf);
        }
        graph.data = data;
    }

    void parseCalendarDatesFile(std::string path, std::shared_ptr<CalendarDatesBuf> calendarDatesBuf)
    {
        std::ifstream in(path);
        if(in.is_open())
        {
            m::cheminot::data::CalendarDates c;
            c.ParseFromIstream(&in);
            *calendarDatesBuf = c.exceptionsbyserviceid();
            in.close();
        }
        else
        {
            throw std::runtime_error("Unexpected error while reading: " + path);
        }
    }

    void parseCalendarDatesFiles(std::list<std::string> paths, CalendarDates &calendarDates)
    {
        std::list<std::shared_ptr<CalendarDatesBuf>> data;
        for(const std::string &path : paths) {
            std::shared_ptr<CalendarDatesBuf> calendarDatesBuf(new CalendarDatesBuf());
            parseCalendarDatesFile(path, calendarDatesBuf);
            data.push_back(calendarDatesBuf);
        }
        calendarDates.data = data;
    }

    std::string getLastTrace(const CheminotDb &connection)
    {
        std::string query = "SELECT * FROM TRACE ORDER BY id ASC";

        auto results = executeQuery(connection.inmemory, query);
        std::string array = "[";
        int id = -1;
        for (auto iterator = results.begin(), end = results.end(); iterator != end; ++iterator)
        {
            auto row = *iterator;
            id = atoi((const char*) row["id"]);
            std::string value = (const char*) row["value"];
            array.append(value);
            if(next(iterator) != results.end()) {
              array.append(",");
            }
        }
        array.append("]");
        cleanTrace(connection, id);
        return array;
    }

    void cleanTrace(const CheminotDb &connection, int id)
    {
        std::string query = "DELETE FROM TRACE WHERE id <= '" + to_string(id) + "'";
        executeUpdate(connection.inmemory, query);
    }

    void resetTrace(const CheminotDb &connection)
    {
        std::string query = "DELETE FROM TRACE";
        executeUpdate(connection.inmemory, query);
    }

    void traceVertice(const CheminotDb &connection, const std::string &tdsp, const Vertice &vertice)
    {
        Json::Value serialized = serializeVertice(vertice);
        serialized["tdsp"] = tdsp;
        Json::FastWriter* writer = new Json::FastWriter();
        std::string query = "INSERT INTO TRACE (value) VALUES('" + writer->write(serialized) + "')";
        executeUpdate(connection.inmemory, query);
    }

    void lock(const CheminotDb &connection)
    {
        std::string query = "UPDATE META SET VALUE = '1' WHERE key = 'aborted'";
        executeUpdate(connection.inmemory, query);
    }

    void unlock(const CheminotDb &connection)
    {
        std::string query = "UPDATE META SET VALUE = '0' WHERE key = 'aborted'";
        executeUpdate(connection.inmemory, query);
    }

    bool isLocked(const CheminotDb &connection, bool *locked)
    {
        std::string query = "SELECT value FROM META WHERE key = 'aborted'";
        std::list< std::unordered_map<std::string, const void*> > results = executeQuery(connection.inmemory, query);
        bool x = false;
        if(!results.empty())
        {
            x = strncmp((char *)results.front()["value"], "1", 1) == 0;
        }
        if(locked != nullptr)
        {
            *locked = x;
        }
        return x;
    }

    std::string getVersion(const CheminotDb &connection)
    {
        std::string query = "SELECT value FROM META WHERE key = 'version'";
        std::list< std::unordered_map<std::string, const void*> > results = executeQuery(connection.file, query);
        return (char *)results.front()["value"];
    }

    tm getCreationDate(const CheminotDb &connection)
    {
        std::string query = "SELECT value FROM META WHERE key = 'createdAt'";
        std::list< std::unordered_map<std::string, const void*> > results = executeQuery(connection.file, query);
        std::string date = (char *)results.front()["value"];
        return parseDate(date);
    }

    Json::Value getMeta(const CheminotDb &connection)
    {
        std::string version = getVersion(connection);
        int createdAt = asTimestamp(getCreationDate(connection));
        Json::Value json;
        json["version"] = version;
        json["createdAt"] = createdAt;
        return json;
    }

    std::list<std::shared_ptr<Trip>> getDirectTrips(const CheminotDb &connection, const std::list<std::string> &subsets, Cache &cache, std::string vsId, std::string veId)
    {

        std::string subsetsFilter = std::accumulate(subsets.begin(), subsets.end(), to_string(""), [](std::string acc, std::string subset) {
            if(acc == "") {
                acc += "a.type = '" + subset + "'";
            } else {
                acc += " OR a.type = '" + subset + "'";
            }
            return acc;
        });

        std::function<std::string (std::string, std::string)> subQuery = [&subsetsFilter](std::string vsId, std::string veId) {
            std::string clause = "b.stopId = '" + vsId + "' OR b.stopId ='" + veId + "'";
            std::string vsIdQuery = "b.stopId = '" + vsId + "'";
            std::string veIdQuery = "b.stopId = '" + veId + "'";
            std::string query = "SELECT a.* FROM TRIPS "
                                "a INNER JOIN TRIPS_STOPS b ON a.id = b.tripId "
                                "WHERE (" + subsetsFilter + ") AND (" + vsIdQuery + " OR " + veIdQuery + ") GROUP BY b.tripId HAVING COUNT(*) = 2";
            return query;
        };

        std::string query = subQuery(vsId, veId);

        std::list<std::shared_ptr<Trip>> trips;
        auto results = executeQuery(connection.file, query);
        for (auto iterator = results.begin(), end = results.end(); iterator != end; ++iterator)
        {
            std::shared_ptr<Trip> trip = parseTripRow(iterator);
            if(!cache.readonly && cache.trips.find(trip->id) == cache.trips.end())
            {
                cache.trips[trip->id] = trip;
            }
            trips.push_back(trip);
        }
        return trips;
    }

    std::list<std::shared_ptr<Trip>> getTripsByIds(const CheminotDb &connection, Cache &cache, const std::list<std::string> &ids)
    {
        std::list<std::shared_ptr<Trip>> results;
        std::list<std::string> toCache;
        for (const std::string &id : ids)
        {
            auto it = cache.trips.find(id);
            if(it != cache.trips.end())
            {
                results.push_back(it->second);
            }
            else
            {
                toCache.push_back(id);
            }
        }

        if(!toCache.empty())
        {
            std::string values = "";
            for (const std::string &id : toCache)
            {
                std::string quoted = "'" + id + "'";
                values = (values == "") ? quoted : (values + ", " + quoted);
            }

            std::string query = "SELECT * FROM TRIPS WHERE id IN (" + values + ")";
            auto fromSqlite = executeQuery(connection.file, query);
            for (auto iterator = fromSqlite.begin(), end = fromSqlite.end(); iterator != end; ++iterator)
            {
                std::shared_ptr<Trip> trip = parseTripRow(iterator);
                if(!cache.readonly) {
                    cache.trips[trip->id] = trip;
                }
                results.push_back(trip);
            }
        }

        return results;
    }

    class CompareArrivalTime
    {
    public:
        bool operator()(const ArrivalTime& gi, const ArrivalTime& gj)
        {
            return datetimeIsBeforeEq(gj.departure, gi.departure);
        }
    };

    bool isTripRemovedOn(Cache &cache, std::shared_ptr<Trip> trip, CalendarDates &calendarDates, const tm &when)
    {
        if(trip->calendar->serviceId != "") {
            auto exceptions = getCalendarDatesByServiceId(cache, calendarDates, trip->calendar->serviceId);
            auto it = std::find_if(exceptions.begin(), exceptions.end(), [&when](std::shared_ptr<CalendarDate> calendarDate)
            {
                return hasSameDate(calendarDate->date, when) && (calendarDate->exceptionType == 2);
            });
            return it != exceptions.end();
        } else {
            return false;
        }
    }

    bool isTripAddedOn(Cache &cache, std::shared_ptr<Trip> trip, CalendarDates &calendarDates, const tm &when)
    {
        if(trip->calendar->serviceId != "") {
            auto exceptions = getCalendarDatesByServiceId(cache, calendarDates, trip->calendar->serviceId);
            auto it = std::find_if(exceptions.begin(), exceptions.end(), [&when](std::shared_ptr<CalendarDate> exception)
            {
                return hasSameDate(exception->date, when) && (exception->exceptionType == 1);
            });
            return it != exceptions.end();
        } else {
            return false;
        }
    }

    static std::unordered_map<int, std::string> week { {1, "monday"}, {2, "tuesday"}, {3, "wednesday"}, {4, "thursday"}, {5, "friday"}, {6, "saturday"}, {0, "sunday"}};

    bool isTripValidToday(std::shared_ptr<Trip> trip, const tm &when)
    {
        return trip->calendar->week[week[when.tm_wday]];
    }

    bool isTripInPeriod(std::shared_ptr<Trip> trip, const tm &when)
    {
        tm startDate = trip->calendar->startDate;
        tm endDate = trip->calendar->endDate;
        bool before = dateIsBeforeEq(startDate, when);
        bool after = dateIsBeforeEq(when, endDate);
        return before && after;
    }

    bool isTripValidOn(Cache &cache, std::shared_ptr<Trip> trip, CalendarDates &calendarDates, const tm &when)
    {
        if(trip->calendar != nullptr)
        {
            bool removed = isTripRemovedOn(cache, trip, calendarDates, when);
            bool added = isTripAddedOn(cache, trip, calendarDates, when);
            bool availableToday = isTripValidToday(trip, when);
            bool inPeriod = isTripInPeriod(trip, when);
            return (!removed && inPeriod && availableToday) || added;
        }
        else
        {
            return true;
        }
        return false;
    }

    std::unordered_map<std::string, bool> tripsAvailability(const CheminotDb &connection, Cache &cache, const std::list<std::string> &ids, CalendarDates &calendarDates, const tm &when)
    {
        std::unordered_map<std::string, bool> availablities;
        std::list<std::shared_ptr<Trip>> trips = getTripsByIds(connection, cache, ids);
        for (const std::shared_ptr<Trip> trip : trips)
        {
            availablities[trip->id] = isTripValidOn(cache, trip, calendarDates, when);
        }
        return availablities;
    }

    std::list<ArrivalTime> orderArrivalTimesBy(std::list<ArrivalTime> arrivalTimes, const tm &t)
    {
        std::list<ArrivalTime> arrivalTimesAt;
        for (ArrivalTime arrivalTime : arrivalTimes)
        {
            arrivalTime.departure.tm_mday = t.tm_mday;
            arrivalTime.departure.tm_wday = t.tm_wday;
            arrivalTime.departure.tm_yday = t.tm_yday;
            arrivalTime.departure.tm_mon = t.tm_mon;
            arrivalTime.departure.tm_year = t.tm_year;
            arrivalTime.arrival.tm_mday = t.tm_mday;
            arrivalTime.arrival.tm_wday = t.tm_wday;
            arrivalTime.arrival.tm_yday = t.tm_yday;
            arrivalTime.arrival.tm_mon = t.tm_mon;
            arrivalTime.arrival.tm_year = t.tm_year;

            if(datetimeIsBeforeNotEq(arrivalTime.departure, t))
            {
                arrivalTime.departure = addDays(arrivalTime.departure, 1);
                arrivalTime.arrival = addDays(arrivalTime.arrival, 1);
            }
            arrivalTimesAt.push_back(arrivalTime);
        };

        arrivalTimesAt.sort([](const ArrivalTime &a, const ArrivalTime &b)
        {
            return datetimeIsBeforeEq(a.arrival, b.arrival);
        });

        return arrivalTimesAt;
    }

    std::list<StopTime> getAvailableDepartures(const CheminotDb &connection, Cache &cache, CalendarDates &calendarDates, const tm &arrivalTime, const Vertice *vi)
    {
        std::list<StopTime> departures(vi->stopTimes);
        departures.remove_if([&arrivalTime] (const StopTime &stopTime)
        {
            return !(isSubwayTrip(stopTime.tripId) || (datetimeIsBeforeEq(arrivalTime, stopTime.departure) && !isTerminus(stopTime)));
        });

        std::list<std::string> tripIds;
        for (auto iterator = departures.begin(), end = departures.end(); iterator != end; ++iterator)
        {
            if(!isSubwayTrip(iterator->tripId))
            {
                tripIds.push_back(iterator->tripId);
            }
        }

        std::unordered_map<std::string, bool> availablities = tripsAvailability(connection, cache, tripIds, calendarDates, arrivalTime);
        departures.remove_if([&availablities, &arrivalTime] (StopTime& stopTime)
        {
            return !(isSubwayTrip(stopTime.tripId) || availablities[stopTime.tripId]);
        });

        departures.sort([](const StopTime &a, const StopTime &b)
        {
            return datetimeIsBeforeEq(a.departure, b.departure);
        });

        return departures;
    }

    static tm INFINITE = {
        INT_MAX,
        INT_MAX,
        INT_MAX,
        INT_MAX,
        INT_MAX,
        INT_MAX,
        INT_MAX,
        INT_MAX,
        INT_MAX
    };

    struct QueueItem
    {
        std::string stopId;
        std::string tripId;
        tm ti;
        tm gi;
        tm estimatedArrival;
    };

    class CompareQueueItem
    {
    public:
        bool operator()(const std::shared_ptr<QueueItem> &itemA, const std::shared_ptr<QueueItem> &itemB)
        {
            return datetimeIsBeforeEq(itemB->estimatedArrival, itemA->estimatedArrival);
        }
    };

    typedef std::priority_queue<std::shared_ptr<QueueItem>, std::vector<std::shared_ptr<QueueItem>>, CompareQueueItem> Queue;

    std::unordered_map<std::string, std::shared_ptr<QueueItem>> initTimeRefinement(const CheminotDb &connection, Graph &graph, ArrivalTimesFunc &arrivalTimesFunc, CalendarDates &calendarDates, Queue &queue, const Vertice &vs, const Vertice &ve, const tm &ts, const std::list<tm> &startingPeriod)
    {

        std::unordered_map<std::string, std::shared_ptr<QueueItem>> items;

        ArrivalTimeFunc gsFunc;
        for (const tm &departureTime : startingPeriod)
        {
            ArrivalTime gs;
            gs.stopId = vs.id;
            gs.departure = departureTime;
            gs.arrival = departureTime;
            gs.lat = vs.lat;
            gs.lng = vs.lng;
            gsFunc[asTimestamp(departureTime)] = gs;
        }

        arrivalTimesFunc[vs.id] = gsFunc;

        std::shared_ptr<QueueItem> qs {new QueueItem};
        qs->stopId = vs.id;
        qs->gi = ts;
        qs->tripId = "<trip>";
        qs->ti = ts;
        qs->estimatedArrival = ts;
        queue.push(qs);
        items[vs.id] = qs;

        graph.foreach([&vs, &ts, &queue, &items](const std::string &stopId, const m::cheminot::data::Vertice &verticeBuf) {
            if(stopId != vs.id)
            {
                std::shared_ptr<QueueItem> qi {new QueueItem};
                qi->stopId = stopId;
                qi->gi = INFINITE;
                qi->tripId = "<trip>";
                qi->ti = ts;
                qi->estimatedArrival = INFINITE;
                queue.push(qi);
                items[stopId] = qi;
            }
        });

        return items;
    }

    tm enlargeStartingTime(const CheminotDb &connection, Cache &cache, CalendarDates &calendarDates, Graph &graph, ArrivalTimeFunc &giFunc, std::shared_ptr<QueueItem> qi, std::shared_ptr<QueueItem> qk, const Vertice &vi, std::string vsId, const tm &ts, const tm &te)
    {
        tm t = minusHours(qk->gi, 2);
        if(qi->stopId == vsId)
        {
            return te;
        }
        else
        {
            std::list<StopTime> viArrivalTimes = orderStopTimesBy(vi.stopTimes, t);
            time_t wfi = LONG_MAX;
            for (const std::string &edge : vi.edges)
            {
                Vertice vf = getVerticeFromGraph(graph, cache, edge, &ts);
                std::list<StopTime> vfDepartureTimes = getAvailableDepartures(connection, cache, calendarDates, t, &vf);
                for (const StopTime &vfDepartureTime : vfDepartureTimes)
                {
                    for (const StopTime &viArrivalTime : viArrivalTimes)
                    {
                        if((vfDepartureTime.tripId == viArrivalTime.tripId) && (vfDepartureTime.pos == (viArrivalTime.pos - 1)) && datetimeIsBeforeNotEq(vfDepartureTime.departure, viArrivalTime.arrival))
                        {
                            time_t travelTime = difftime(asTimestamp(viArrivalTime.arrival), asTimestamp(vfDepartureTime.departure));
                            wfi = travelTime < wfi ? travelTime : wfi;
                        }
                    }
                }
            }

            if(wfi == LONG_MAX)
            {
                oops("Unable to compute travel time between vf to vi");
            }

            tm nextEarliestArrivalTime = asDateTime(asTimestamp(qk->gi) + wfi);

            std::pair<tm, tm> enlarged = { qi->ti, qi->gi };
            for (const auto &f : giFunc)
            {
                time_t ti = f.first;
                tm gi = f.second.arrival;
                if(datetimeIsBeforeEq(gi, nextEarliestArrivalTime) && datetimeIsBeforeEq(enlarged.second, gi))
                {
                    enlarged = { asDateTime(ti), gi };
                }
            }

            if(hasSameDateTime(enlarged.second, qi->gi))
            {
                auto last = std::prev(giFunc.end());
                return asDateTime(last->first);
            }

            return enlarged.first;
        }
    }

    ArrivalTime stopTime2ArrivalTime(const Vertice &vertice, const StopTime &stopTime)
    {
        ArrivalTime arrivalTime;
        arrivalTime.stopId = vertice.id;
        arrivalTime.arrival = stopTime.arrival;
        arrivalTime.departure = stopTime.departure;
        arrivalTime.tripId = stopTime.tripId;
        arrivalTime.pos = stopTime.pos;
        arrivalTime.lat = vertice.lat;
        arrivalTime.lng = vertice.lng;
        return arrivalTime;
    }

    std::list<tm> getStartingPeriod(const CheminotDb &connection, Cache &cache, CalendarDates &calendarDates, const Vertice *vs, const tm &ts, const tm &te, int max)
    {
        auto departures = getAvailableDepartures(connection, cache, calendarDates, ts, vs);
        departures.sort([](const StopTime &a, const StopTime &b)
        {
            return datetimeIsBeforeEq(a.departure, b.departure);
        });

        std::list<tm> startingPeriod;
        for (const StopTime &departureTime : departures)
        {
            if(datetimeIsBeforeEq(departureTime.departure, te))
            {
                if(startingPeriod.size() < max)
                {
                    startingPeriod.push_back(departureTime.departure);
                }
                else
                {
                    break;
                }
            }
        }

        return startingPeriod;
    }

    void fillCache(Cache &cache, CalendarDates &calendarDates, Graph &graph)
    {
        tm dateref = getNow();
        graph.foreach([&dateref, &cache](const std::string &stopId, const m::cheminot::data::Vertice &verticeBuf) {
            std::shared_ptr<Vertice> vertice {new Vertice};
            std::string id = verticeBuf.id();
            vertice->id = id;
            vertice->name = verticeBuf.name();
            vertice->lat = verticeBuf.lat();
            vertice->lng = verticeBuf.lng();
            vertice->edges = parseEdges(verticeBuf.edges());
            vertice->stopTimes = parseStopTimes(&dateref, verticeBuf.stoptimes());
            cache.vertices[id] = vertice;
        });

        calendarDates.foreach([&cache](const std::string &serviceId, const m::cheminot::data::CalendarExceptions &exceptionsBuf) {
            std::list<std::shared_ptr<CalendarDate>> calendarDatesByServiceId;
            for(const auto &calendarDateBuf : exceptionsBuf.calendardates())
            {
                std::shared_ptr<CalendarDate> calendarDate {new CalendarDate};
                calendarDate->serviceId = calendarDateBuf.serviceid();
                calendarDate->date = parseDate(calendarDateBuf.date());
                calendarDate->exceptionType = calendarDateBuf.exceptiontype();
                calendarDatesByServiceId.push_back(calendarDate);
            }
            cache.calendarDates[serviceId] = calendarDatesByServiceId;
        });

        cache.readonly = true;
    }

    bool isQueueItemOutdated(std::unordered_map<std::string, tm> *uptodate, std::shared_ptr<QueueItem> item)
    {
        auto last = uptodate->find(item->stopId);
        if(last != uptodate->end())
        {
            return !datetimeIsBeforeEq(item->gi, last->second);
        }
        else
        {
            return false;
        }
    }

    StopTime getEarliestArrivalTime(const CheminotDb &connection, Cache &cache, CalendarDates &calendarDates, const Vertice &vi, const Vertice &vj, const ArrivalTime &gi)
    {
        std::list<StopTime> viDepartures = getAvailableDepartures(connection, cache, calendarDates, gi.arrival, &vi);
        StopTime earliestArrivalTime;
        earliestArrivalTime.arrival = INFINITE;
        for(const StopTime &viDepartureTime : viDepartures)
        {
            for(const StopTime &vjStopTime : vj.stopTimes)
            {
                if(viDepartureTime.tripId == vjStopTime.tripId && datetimeIsBeforeEq(viDepartureTime.departure, vjStopTime.arrival) && datetimeIsBeforeEq(gi.arrival, viDepartureTime.departure) && (viDepartureTime.pos == vjStopTime.pos - 1))
                {
                    if(datetimeIsBeforeNotEq(vjStopTime.arrival, earliestArrivalTime.arrival))
                    {
                        earliestArrivalTime = vjStopTime;
                    }
                }
            }
        }
        return earliestArrivalTime;
    }

    void updateArrivalTimeFunc(const CheminotDb &connection, Graph &graph, Cache &cache, CalendarDates &calendarDates, ArrivalTimesFunc &arrivalTimesFunc, const Vertice &vi, const ArrivalTime &gi, const Vertice &vj, const tm &startingTime, std::function<void(StopTime)> done)
    {
        StopTime vjStopTime = getEarliestArrivalTime(connection, cache, calendarDates, vi, vj, gi);
        if(!hasSameDateTime(vjStopTime.arrival, INFINITE))   // MAYBE TODAY, ONE EDGE ISN'T AVAILABLE
        {
            ArrivalTimeFunc gjFunc;
            time_t t = asTimestamp(startingTime);
            auto it = arrivalTimesFunc.find(vj.id);
            if(it != arrivalTimesFunc.end())
            {
                gjFunc = it->second;
                auto currentGj = gjFunc.find(t);
                if(currentGj != gjFunc.end())
                {
                    if(datetimeIsBeforeNotEq(vjStopTime.arrival, currentGj->second.arrival))   // UPDATING IF BETTER FOUND
                    {
                        gjFunc[t] = stopTime2ArrivalTime(vj, vjStopTime);
                        done(vjStopTime);
                    }
                }
                else
                {
                    gjFunc[t] = stopTime2ArrivalTime(vj, vjStopTime); // NEW VALUE
                    done(vjStopTime);
                }
            }
            else
            {
                gjFunc[t] = stopTime2ArrivalTime(vj, vjStopTime); // NEW FUNC
                arrivalTimesFunc[vj.id] = gjFunc;
                done(vjStopTime);
            }
            arrivalTimesFunc[vj.id] = gjFunc;
        }
    }

    tm computeEstimatedArrival(Vertice *vj, Vertice *vs, Vertice *ve, tm ts, tm gj)
    {
        double distanceVsVj = distance({vs->lat, vs->lng}, {vj->lat, vj->lng});
        double durationVsVj = asTimestamp(gj) - asTimestamp(ts); //seconds
        double distanceVjVe = distance({vj->lat, vj->lng}, {ve->lat, ve->lng});
        double durationVjVe = (distanceVjVe * durationVsVj) / distanceVsVj;
        return addSeconds(gj, durationVjVe);
    }

    std::string tdspHash(std::string vsId, std::string veId, tm ts, tm te, int max)
    {
        return vsId + "|" + veId + "|" + to_string(asTimestamp(ts) * 1000) + "|" + to_string(asTimestamp(te) * 1000) + "|" + to_string(max);
    }

    std::tuple<bool, ArrivalTimesFunc, std::string> refineArrivalTimes(const CheminotDb &connection, Graph &graph, Cache &cache, CalendarDates &calendarDates, std::string vsId, std::string veId, const tm &tstart, const tm &tend, int max)
    {
        std::string tdsp = tdspHash(vsId, veId, tstart, tend, max);

        Queue queue;
        ArrivalTimesFunc arrivalTimesFunc;
        std::unordered_map<std::string, tm> uptodate;
        Vertice vs = getVerticeFromGraph(graph, cache, vsId, &tstart);
        std::list<tm> startingPeriod = getStartingPeriod(connection, cache, calendarDates, &vs, tstart, tend, max);
        if(startingPeriod.empty())
        {
            return std::make_tuple(false, arrivalTimesFunc, veId);
        }

        const tm ts = *startingPeriod.begin();
        const tm te = *(std::prev(startingPeriod.end()));

        Vertice ve = getVerticeFromGraph(graph, cache, veId, &ts);
        std::unordered_map<std::string, std::shared_ptr<QueueItem>> items = initTimeRefinement(connection, graph, arrivalTimesFunc, calendarDates, queue, vs, ve, ts, startingPeriod);

        bool locked = false;
        while(!isLocked(connection, &locked) && queue.size() >= 2)
        {
            std::shared_ptr<QueueItem> qi = queue.top();
            Vertice vi = getVerticeFromGraph(graph, cache, qi->stopId, &qi->gi);
            traceVertice(connection, tdsp, vi);
            queue.pop();

            if(!isQueueItemOutdated(&uptodate, qi))
            {
                std::shared_ptr<QueueItem> qk = queue.top();
                ArrivalTimeFunc giFunc = arrivalTimesFunc[vi.id];
                tm enlargedStartingTime = ts;
                if(!hasSameDateTime(ts, te))
                {
                    enlargedStartingTime = enlargeStartingTime(connection, cache, calendarDates, graph, giFunc, qi, qk, vi, vsId, ts, te);
                }
                for (const std::string &vjId : vi.edges)
                {
                    if(vjId != vsId)
                    {
                        for (const tm &startingTime : startingPeriod)
                        {
                            if(datetimeIsBeforeEq(qi->ti, startingTime) && datetimeIsBeforeEq(startingTime, enlargedStartingTime))
                            {
                                ArrivalTime gi = giFunc[asTimestamp(startingTime)];
                                Vertice vj = getVerticeFromGraph(graph, cache, vjId, &gi.arrival);
                                updateArrivalTimeFunc(connection, graph, cache, calendarDates, arrivalTimesFunc, vi, gi, vj, startingTime, [&](StopTime vjStopTime)
                                {
                                    std::shared_ptr<QueueItem> updatedQj {new QueueItem};
                                    updatedQj->stopId = vjId;
                                    updatedQj->ti = items[vjId]->ti;
                                    updatedQj->gi = vjStopTime.arrival;
                                    updatedQj->tripId = vjStopTime.tripId;
                                    updatedQj->estimatedArrival = computeEstimatedArrival(&vj, &vs, &ve, startingTime, vjStopTime.arrival);
                                    queue.push(updatedQj);
                                    uptodate[vjId] = vjStopTime.arrival;
                                });
                            }
                        }
                    }
                }

                if(datetimeIsBeforeEq(te, enlargedStartingTime))
                {
                    if(vi.id == veId || (veId == PARIS_STOP_ID && isParis(vi.id)))
                    {
                        return std::make_tuple(false, arrivalTimesFunc, vi.id);
                    }
                }
                else
                {
                    qi->ti = enlargedStartingTime;
                    qi->gi = giFunc[asTimestamp(enlargedStartingTime)].arrival;
                    queue.push(qi);
                }
            }
        };

        return std::make_tuple(locked, arrivalTimesFunc, veId);
    }

    time_t getOptimalStartingTime(ArrivalTimeFunc *geFunc, std::string veId)
    {
        std::pair<time_t, cheminotc::ArrivalTime> best = *(geFunc->begin());
        for (auto iterator = std::next(geFunc->begin()), end = geFunc->end(); iterator != end; ++iterator)
        {
            std::pair<time_t, ArrivalTime> point = *iterator;
            if(datetimeIsBeforeEq(point.second.arrival, best.second.arrival))
            {
                best = point;
            }
        }
        return best.first;
    }

    std::list<ArrivalTime> pathSelection(Graph &graph, Cache &cache, ArrivalTimesFunc &arrivalTimesFunc, const tm &ts, std::string vsId, std::string veId)
    {
        std::list<ArrivalTime> path;
        if(arrivalTimesFunc.empty())
        {
            return path;
        }

        ArrivalTimeFunc geFunc = arrivalTimesFunc[veId];
        time_t optimalStartingTime = getOptimalStartingTime(&geFunc, veId);
        Vertice vj = getVerticeFromGraph(graph, cache, veId, &ts);

        std::function<bool (std::string, ArrivalTime*)> getArrivalTimeAt = [&arrivalTimesFunc, &optimalStartingTime](std::string viId, ArrivalTime *arrivalTime)
        {
            auto it = arrivalTimesFunc.find(viId);
            if(it != arrivalTimesFunc.end())
            {
                ArrivalTimeFunc arrivalTimeFunc = arrivalTimesFunc[viId];
                auto it = arrivalTimeFunc.find(optimalStartingTime);
                if(it != arrivalTimeFunc.end())
                {
                    *arrivalTime = it->second;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        };

        ArrivalTime ge;
        if(getArrivalTimeAt(veId, &ge))
        {
            path.push_back(ge);
        }

        std::function<bool (std::string, std::string)> running = [](std::string vjId, std::string vsId)
        {
            return !(vjId == vsId || (vsId == PARIS_STOP_ID && isParis(vjId)));
        };

        while(running(vj.id, vsId))
        {
            ArrivalTime gj;
            if(getArrivalTimeAt(vj.id, &gj))
            {
                for (const std::string &viId :vj.edges)
                {
                    ArrivalTime gi;
                    if(getArrivalTimeAt(viId, &gi))
                    {
                        Vertice vi = getVerticeFromGraph(graph, cache, viId, &ts);
                        auto it = std::find_if(vi.stopTimes.begin(), vi.stopTimes.end(), [&gj](const StopTime &viStopTime)
                        {
                            return (viStopTime.tripId == gj.tripId) && (viStopTime.pos == (gj.pos - 1));
                        });
                        if(it != vi.stopTimes.end())
                        {
                            if(viId == vsId || (vsId == PARIS_STOP_ID && isParis(viId)))   // RECOVER DEPARTURE
                            {
                                gi.tripId = gj.tripId;
                                gi.departure = it->departure;
                                gi.arrival = it->arrival;
                                if(dateIsBeforeNotEq(gi.departure, gj.arrival) && timeIsBeforeNotEq(gi.departure, gj.arrival))
                                {
                                    gi.departure.tm_mday = gj.arrival.tm_mday;
                                }
                                if(dateIsBeforeNotEq(gi.arrival, gi.departure) && timeIsBeforeEq(gi.departure, gj.arrival))
                                {
                                    gi.arrival.tm_mday = gi.departure.tm_mday;
                                }
                            }
                            path.push_front(gi);
                            vj = getVerticeFromGraph(graph, cache, gi.stopId, &ts);
                            break;
                        }
                    }
                }
            }
        };

        return path;
    }

    std::pair<bool, std::list<ArrivalTime>> lookForBestDirectTrip(const CheminotDb &connection, const std::list<std::string> &subsets, Graph &graph, Cache &cache, CalendarDates &calendarDates, const std::string &vsId, const std::string &veId, const tm &ts, const tm &te)
    {
        Vertice vs = getVerticeFromGraph(graph, cache, vsId, &ts);
        Vertice ve = getVerticeFromGraph(graph, cache, veId, &ts);
        std::list<std::shared_ptr<Trip>> trips = getDirectTrips(connection, subsets, cache, vsId, veId);
        std::pair<std::shared_ptr<Trip>, tm> bestTrip;
        bool hasBestTrip = false;
        for(std::shared_ptr<Trip> trip : trips)
        {
            auto veIt = std::find_if(ve.stopTimes.begin(), ve.stopTimes.end(), [&trip](const StopTime &stopTime)
            {
                return stopTime.tripId == trip->id;
            });

            auto vsIt = std::find_if(vs.stopTimes.begin(), vs.stopTimes.end(), [&trip](const StopTime &stopTime)
            {
                return stopTime.tripId == trip->id;
            });

            if(vsIt != vs.stopTimes.end() && veIt != ve.stopTimes.end())
            {
                StopTime stopTimeVs = *vsIt;
                StopTime stopTimeVe = *veIt;
                tm departureVs = stopTimeVs.departure;
                tm arrivalVe = stopTimeVe.arrival;
                if(isTripValidOn(cache, trip, calendarDates, departureVs))
                {
                    if(stopTimeVs.pos < stopTimeVe.pos && datetimeIsBeforeEq(ts, departureVs) && datetimeIsBeforeEq(departureVs, te))
                    {
                        if(!hasBestTrip || datetimeIsBeforeNotEq(arrivalVe, bestTrip.second))
                        {
                            bestTrip = {trip, arrivalVe};
                            hasBestTrip = true;
                        }
                    }
                }
            }
        }

        std::function<bool (std::string, std::shared_ptr<Trip>, ArrivalTime*)> getArrivalTime = [&ts, &graph, &cache](std::string viId, std::shared_ptr<Trip> trip, ArrivalTime *arrivalTime)
        {
            Vertice vi = getVerticeFromGraph(graph, cache, viId, &ts);
            auto it = std::find_if(vi.stopTimes.begin(), vi.stopTimes.end(), [&trip](const StopTime &stopTime)
            {
                return stopTime.tripId == trip->id;
            });
            if(it != vi.stopTimes.end())
            {
                StopTime stopTime = *it;
                *arrivalTime = stopTime2ArrivalTime(vi, stopTime);
                return true;
            }
            return false;
        };

        std::list<ArrivalTime> arrivalTimes;

        if(hasBestTrip)
        {
            std::shared_ptr<Trip> trip = bestTrip.first;
            for(const std::string &stopId : trip->stopIds)
            {
                if(arrivalTimes.empty())
                {
                    if(stopId == vsId || (vsId == PARIS_STOP_ID && isParis(stopId)))
                    {
                        ArrivalTime arrivalTime;
                        if(getArrivalTime(stopId, trip, &arrivalTime))
                        {
                            arrivalTimes.push_back(arrivalTime);
                        }
                    }
                }
                else
                {
                    ArrivalTime arrivalTime;
                    if(getArrivalTime(stopId, trip, &arrivalTime))
                    {
                        arrivalTimes.push_back(arrivalTime);
                    }
                    if(stopId == veId || (veId == PARIS_STOP_ID && isParis(stopId)))
                    {
                        break;
                    }
                }
            }
        }
        return { trips.size() > 0, orderArrivalTimesBy(arrivalTimes, ts) };
    }

    std::pair<bool, std::list<ArrivalTime>> lookForBestTrip(const CheminotDb &connection, Graph &graph, Cache &cache, CalendarDates &calendarDates, const std::string &vsId, const std::string &veId, const tm &ts, const tm &te, int max)
    {
        auto result = refineArrivalTimes(connection, graph, cache, calendarDates, vsId, veId, ts, te, max);
        ArrivalTimesFunc arrivalTimes = std::get<1>(result);
        bool locked = std::get<0>(result);
        const std::string vendId = std::get<2>(result);

        if(locked)
        {
            return { locked, {} };
        }
        else
        {
            return { false, pathSelection(graph, cache, arrivalTimes, ts, vsId, vendId) };
        }
    }
}
