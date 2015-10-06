#include "../src/cheminotc.h"
#include "../src/protobuf/cheminotBuf.pb.h"
#include <sqlite3.h>

int main(int argc, char **argv)
{
    std::string stmalo = "StopPoint:OCETrain TER-87478107";
    std::string laval = "StopPoint:OCETrain TER-87478404";
    std::string parisMont = "StopPoint:OCETrain TER-87391003";
    std::string parisNord = "StopPoint:OCETrain TER-87271007";
    std::string paris = "StopPoint:OCETrain TER-PARISXXX";
    std::string chartres = "StopPoint:OCETrain TER-87394007";
    std::string creil = "StopPoint:OCETrain TER-87276006";
    std::string versailles = "StopPoint:OCETrain TER-87393009";
    std::string lehavre = "StopPoint:OCETrain TER-87413013";
    std::string avallon = "StopPoint:OCETrain TER-87683789";
    std::string marseillestcharles = "StopPoint:OCETrain TER-87751008";
    std::string vernon = "StopPoint:OCETrain TER-87415604";
    std::string parisLazard = "StopPoint:OCETrain TER-87384008";

    struct tm ts = cheminotc::getNow();
    // ts.tm_hour = 7;
    // ts.tm_min = 47;
    tm te = cheminotc::addHours(ts, 12);

    // tm ts = cheminotc::asDateTime(1429599051);
    // tm te = cheminotc::asDateTime(1429642251);

    printf("%s %s\n", cheminotc::formatDateTime(ts).c_str(), cheminotc::formatDateTime(te).c_str());

    cheminotc::CheminotDb connection = cheminotc::openConnection("cheminot.db");

    cheminotc::Graph graph;
    cheminotc::parseGraphFiles({"ter-graph", "trans-graph"}, graph);

    cheminotc::CalendarDates calendarDates;
    cheminotc::parseCalendarDatesFiles({"ter-calendardates", "trans-calendardates"}, calendarDates);

    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, chartres, paris, ts, te, 1);

    for (auto result : results.second)
    {
        printf("%s - %s - %s\n", result.stopId.c_str() , cheminotc::formatDateTime(result.arrival).c_str(), result.tripId.c_str());
    }
}
