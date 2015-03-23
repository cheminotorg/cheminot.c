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
    struct tm ts = cheminotc::getNow();
    ts.tm_hour = 0;
    ts.tm_min = 0;
    tm te = cheminotc::getNow();

    te.tm_hour = 12;
    te.tm_min = 0;

    sqlite3 *handle = cheminotc::openConnection("/Users/sre/data/Projects/me/cheminot.c/cheminot.db");
    cheminotc::Graph graph;
    cheminotc::CalendarDates calendarDates;
    google::protobuf::Arena *arena = new google::protobuf::Arena();
    cheminotc::parseGraph("/Users/sre/data/Projects/me/cheminot.c/graph", arena, &graph);
    cheminotc::parseCalendarDates("/Users/sre/data/Projects/me/cheminot.c/calendardates", arena, &calendarDates);
    cheminotc::Cache cache;
    auto results = cheminotc::lookForBestTrip(handle, arena, &graph, &cache, &calendarDates, chartres, marseillestcharles, ts, te, 1);

    for (auto iterator = results.second.begin(), end = results.second.end(); iterator != end; ++iterator)
    {
        printf("%s - %s - %s\n", iterator->stopId.c_str() , cheminotc::formatDateTime(iterator->arrival).c_str(), iterator->tripId.c_str());
    }
}
