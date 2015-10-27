#include "../src/cheminotc.h"
#include "../src/protobuf/cheminotBuf.pb.h"
#include <sqlite3.h>

int main(int argc, char **argv)
{
    std::string stmalo = "8747810";
    std::string laval = "8747840";
    std::string parisMont = "8739100";
    std::string parisNord = "8727100";
    std::string paris = "PARISXX";
    std::string chartres = "8739400";
    std::string creil = "8727600";
    std::string versailles = "8739300";
    std::string lehavre = "8741301";
    std::string avallon = "8768378";
    std::string marseillestcharles = "8775100";
    std::string vernon = "8741560";
    std::string parisLazard = "8738400";

    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 7;
    ts.tm_min = 0;
    tm te = cheminotc::addHours(ts, 12);

    // tm ts = cheminotc::asDateTime(1429599051);
    // tm te = cheminotc::asDateTime(1429642251);

    printf("%s %s\n", cheminotc::formatDateTime(ts).c_str(), cheminotc::formatDateTime(te).c_str());

    cheminotc::CheminotDb connection = cheminotc::openConnection("cheminot.db");

    cheminotc::Graph graph;
    cheminotc::parseGraphFiles({"ter-graph", "trans-graph", "inter-graph"}, graph);

    cheminotc::CalendarDates calendarDates;
    cheminotc::parseCalendarDatesFiles({"ter-calendardates", "trans-calendardates", "inter-calendardates"}, calendarDates);

    cheminotc::Cache cache;
    //auto results = cheminotc::lookForBestTrip(connection, graph, cache, calendarDates, chartres, paris, ts, te, 1);
    auto results = cheminotc::lookForBestDirectTrip(connection, { "TER", "TRANS", "INTER" }, graph, cache, calendarDates, paris, vernon, ts, te);

    for (auto result : results.second)
    {
        printf("%s - %s - %s\n", result.stopId.c_str() , cheminotc::formatDateTime(result.arrival).c_str(), result.tripId.c_str());
    }
}
