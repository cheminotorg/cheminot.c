#include <cheminotc.h>
#include "../src/protobuf/cheminotBuf.pb.h"
#include <sqlite3.h>
#include <gtest/gtest.h>
#include <string>
#include <numeric>

namespace stops
{
    static std::string stmalo = "StopPoint:OCETrain TER-87478107";
    static std::string laval = "StopPoint:OCETrain TER-87478404";
    static std::string parisMont = "StopPoint:OCETrain TER-87391003";
    static std::string parisNord = "StopPoint:OCETrain TER-87271007";
    static std::string paris = "StopPoint:OCETrain TER-PARISXXX";
    static std::string chartres = "StopPoint:OCETrain TER-87394007";
    static std::string creil = "StopPoint:OCETrain TER-87276006";
    static std::string versailles = "StopPoint:OCETrain TER-87393009";
    static std::string cholet = "StopPoint:OCETrain TER-87484303";
    static std::string lemans = "StopPoint:OCETrain TER-87396002";
    static std::string angersstlaud = "StopPoint:OCETrain TER-87484006";
    static std::string lehavre = "StopPoint:OCETrain TER-87413013";
    static std::string avallon = "StopPoint:OCETrain TER-87683789";
    static std::string marseillestcharles = "StopPoint:OCETrain TER-87751008";
    static std::string stdizier = "StopPoint:OCETrain TER-87175000";
    static std::string orleans = "StopPoint:OCETrain TER-87543009";
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
        cheminotc::parseGraphFiles({"ter-graph", "trans-graph"}, &graph);
        cheminotc::parseCalendarDatesFiles({"ter-calendardates", "trans-calendardates"}, &calendarDates);
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
    auto results = cheminotc::lookForBestDirectTrip(connection, graph, cache, calendarDates, stops::laval, stops::paris, ts, te);
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
    auto results = cheminotc::lookForBestDirectTrip(connection, graph, cache, calendarDates, stops::chartres, stops::parisMont, ts, te);
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
    auto results = cheminotc::lookForBestDirectTrip(connection, graph, cache, calendarDates, stops::chartres, stops::paris, ts, te);
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
    auto results = cheminotc::lookForBestDirectTrip(connection, graph, cache, calendarDates, stops::paris, stops::chartres, ts, te);
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
