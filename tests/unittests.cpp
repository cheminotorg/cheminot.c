#include <cheminotc.h>
#include "../src/protobuf/cheminotBuf.pb.h"
#include <sqlite3.h>
#include <gtest/gtest.h>
#include <string>
#include <numeric>

namespace stops
{
    static std::string stmalo = "8747810";
    static std::string laval = "8747840";
    static std::string parisMont = "8739100";
    static std::string parisNord = "8727100";
    static std::string paris = "PARISXX";
    static std::string chartres = "8739400";
    static std::string creil = "8727600";
    static std::string versailles = "8739300";
    static std::string cholet = "8748430";
    static std::string lemans = "8739600";
    static std::string angersstlaud = "8748400";
    static std::string lehavre = "8741301";
    static std::string avallon = "8768378";
    static std::string marseillestcharles = "8775100";
    static std::string stdizier = "8717500";
    static std::string orleans = "8754300";
}

namespace utils
{
    static std::function<void (std::list<cheminotc::ArrivalTime>)> print = [](std::list<cheminotc::ArrivalTime> arrivalTimes)
    {
        for (cheminotc::ArrivalTime arrivalTime : arrivalTimes)
        {
            std::string arrival = cheminotc::formatDateTime(arrivalTime.arrival).c_str();
            std::string departure = cheminotc::formatDateTime(arrivalTime.departure).c_str();
            printf("%s - %s - %s || %s\n", arrivalTime.stopId.c_str() , arrivalTime.tripId.c_str(), arrival.c_str(), departure.c_str());
        }
    };
}

class GraphFixture : public testing::Test
{
    protected:

    cheminotc::CheminotDb connection;
    cheminotc::Graph graph;
    cheminotc::CalendarDates calendarDates;

    virtual void SetUp()
    {
        connection = cheminotc::openConnection("cheminot.db");
        cheminotc::parseGraphFiles({"ter-graph", "trans-graph", "inter-graph"}, graph);
        cheminotc::parseCalendarDatesFiles({"ter-calendardates", "trans-calendardates", "inter-calendardates"}, calendarDates);
    }
};

TEST_F(GraphFixture, lock_unlock)
{
    cheminotc::lock(connection);
    bool locked = false;
    cheminotc::isLocked(connection, &locked);
    EXPECT_EQ(true, locked);

    cheminotc::unlock(connection);
    cheminotc::isLocked(connection, &locked);
    EXPECT_EQ(false, locked);
}

TEST_F(GraphFixture, lookforbestdirecttrip_empty_notrip)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestDirectTrip(connection, { "TER", "TRANS", "INTER" }, graph, cache, calendarDates, stops::laval, stops::paris, ts, te);
    utils::print(results.second);
    EXPECT_EQ(0, results.second.size());
    EXPECT_EQ(false, results.first);
}

TEST_F(GraphFixture, lookforbestdirecttrip_chartres_parismont)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestDirectTrip(connection, { "TER", "TRANS", "INTER" }, graph, cache, calendarDates, stops::chartres, stops::parisMont, ts, te);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbestdirecttrip_chartres_paris)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestDirectTrip(connection, { "TER", "TRANS", "INTER" }, graph, cache, calendarDates, stops::chartres, stops::paris, ts, te);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbestdirecttrip_paris_chartres)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestDirectTrip(connection, { "TER", "TRANS", "INTER" }, graph, cache, calendarDates, stops::paris, stops::chartres, ts, te);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartre_parismont)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::chartres, stops::parisMont, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartre_paris)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::chartres, stops::paris, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_paris_chartres)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::paris, stops::chartres, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartres_avallon)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::chartres, stops::avallon, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartres_creil)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::chartres, stops::creil, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartres_marseilles)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::chartres, stops::marseillestcharles, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartres_stdizier)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::chartres, stops::stdizier, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartres_lehavre)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::chartres, stops::lehavre, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartres_orleans)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::chartres, stops::orleans, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_lehavre_chartres)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::lehavre, stops::chartres, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_creil_chartres)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, stops::creil, stops::chartres, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}
