/****************************************************************\
|=== Made by Joffrey Panisset (Baoulettes) 12.03.2020 03:00pm ===|
|==== With the huge help ThievingSix from Unidokkan discord. ====|
|================ Do not forget to say thanks :) ================|
\****************************************************************/
#include <restclient-cpp/restclient.h>
#include <parallel_hashmap/phmap.h>
#include <unidokkan/database.h>
#include <nlohmann/json.hpp>
#include <unidokkan/hook.h>
#include <unidokkan/log.h>
#include <dokkan/crypto.h>
#include <string>

bool ShowEventsBanners(NetworkResponse *response) {
	using namespace std::string_view_literals;
	using json = nlohmann::json;	
	UD_LOGI("Translation Event List : Loading . . .");
	constexpr std::string_view kGetQuests = R"SQL(SELECT
		quests.id,
		quests.name
	FROM
		quests)SQL"sv;
	phmap::parallel_flat_hash_map<int, json> Quests_List_Keyed = UniDokkan::Database::selectKeyedInt(kGetQuests, 0);
	for (auto &EventList_Kagi : response->jsonBody["eventkagi_events"]) {
		std::string Event_ID_long 		= 	to_string(EventList_Kagi["id"]);
		std::string Event_ID 			= 	Event_ID_long.substr(0, 3);
		for (auto &Quests_Name_Kagi : EventList_Kagi["quests"]) {
			int Quest_ID 		= 	Quests_Name_Kagi["id"];
			std::string Quest_Name		=	Quests_List_Keyed[Quest_ID]["name"].get<std::string>();
			Quests_Name_Kagi["name"]	=	Quest_Name;
		}
	}
	for (auto &EventList : response->jsonBody["events"]) {
		std::string Event_ID_long 		= 	to_string(EventList["id"]);
		std::string Event_ID 			= 	Event_ID_long.substr(0, 3);
		for (auto &Quests_Name : EventList["quests"]) {
			int Quest_ID 			= 	Quests_Name["id"];
			std::string Quest_Name	=	Quests_List_Keyed[Quest_ID]["name"].get<std::string>();
			Quests_Name["name"]		=	Quest_Name;	
		}
	}
    return true;
}
extern "C" {
	int unidokkan_init_v2(HookLib* hook_lib) {
		hook_lib->addResponseHook("^/events$", ShowEventsBanners);
		if (!hook_lib) {
			UD_LOGI("Translation Event List : Not Loaded");
            return 1;
        }
        UD_LOGI("Translation Event List : Loaded");
        return 0;
	}
}