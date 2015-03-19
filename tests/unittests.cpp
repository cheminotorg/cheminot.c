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
        for (auto iterator = arrivalTimes.begin(), end = arrivalTimes.end(); iterator != end; ++iterator)
        {
            std::string arrival = cheminotc::formatDateTime(iterator->arrival).c_str();
            std::string departure = cheminotc::formatDateTime(iterator->departure).c_str();
            printf("%s - %s - %s || %s\n", iterator->stopId.c_str() , iterator->tripId.c_str(), arrival.c_str(), departure.c_str());
        }
    };
}

class GraphFixture : public testing::Test
{
protected:

    sqlite3 *handle;
    cheminotc::Graph graph;
    cheminotc::CalendarDates calendarDates;

    virtual void SetUp()
    {
        std::string path = "cheminot.db";
        handle = cheminotc::openConnection(path);
        cheminotc::parseGraph("graph", &graph);
        cheminotc::parseCalendarDates("calendardates", &calendarDates);
    }
};

TEST_F(GraphFixture, lookforbestdirecttrip_chartres_parismont)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestDirectTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::parisMont, ts, te);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbestdirecttrip_chartres_paris)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestDirectTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::paris, ts, te);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbestdirecttrip_paris_chartres)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestDirectTrip(handle, &graph, &cache, &calendarDates, stops::paris, stops::chartres, ts, te);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartre_parismont)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::parisMont, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_chartre_paris)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::paris, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_paris_chartres)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::paris, stops::chartres, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parismont_parisbercy)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::avallon, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parismont_parisnord)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::creil, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parismont_parislyon)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::marseillestcharles, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parismont_parisest)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::stdizier, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parismont_parislazare)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::lehavre, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parismont_parisorleans)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::orleans, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parisbercy_parismont)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::chartres, stops::avallon, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parislazare_parismont)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::lehavre, stops::chartres, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}

TEST_F(GraphFixture, lookforbesttrip_parisnord_parismont)
{
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 24);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, &graph, &cache, &calendarDates, stops::creil, stops::chartres, ts, te, 1);
    utils::print(results.second);
    EXPECT_EQ(false, results.second.empty());
}
