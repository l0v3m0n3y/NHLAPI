#ifndef NHLAPI_H
#define NHLAPI_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class NHLAPI {
private:
    std::string api_base = "https://api-web.nhle.com/v1";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("api-web.nhle.com"));
        request.headers().add(U("Content-Type"), U("application/json"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        if (!data.empty() && (method == "POST" || method == "PUT")) {
            request.set_body(data);
        }
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                // Кодируем значения параметров
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method, data);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }
public:
    NHLAPI(){
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> get_schedule_now() {
        return make_api_call("/schedule/now", "GET");
    }

    pplx::task<json::value> get_schedule_date(const std::string& date) {
        return make_api_call("/schedule/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(date))), "GET");
    }

    pplx::task<json::value> get_club_schedule_season(const std::string& teamAbbr) {
        return make_api_call("/club-schedule-season/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(teamAbbr))) + "/now", "GET");
    }

    pplx::task<json::value> get_club_schedule_week(const std::string& teamAbbr) {
        return make_api_call("/club-schedule/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(teamAbbr))) + "/week/now", "GET");
    }

    pplx::task<json::value> get_club_schedule_month(const std::string& teamAbbr) {
        return make_api_call("/club-schedule/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(teamAbbr))) + "/month/now", "GET");
    }

    pplx::task<json::value> get_standings_now() {
        return make_api_call("/standings/now", "GET");
    }

    pplx::task<json::value> get_standings_season() {
        return make_api_call("/standings-season", "GET");
    }

    pplx::task<json::value> get_club_stats_season(const std::string& teamAbbr) {
        return make_api_call("/club-stats-season/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(teamAbbr))), "GET");
    }

    pplx::task<json::value> get_player_landing(const std::string& playerId) {
        return make_api_call("/player/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(playerId))) + "/landing", "GET");
    }
};

#endif
