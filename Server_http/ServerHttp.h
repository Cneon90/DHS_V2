//
// Created by Kirill on 24.03.2025.
//
#ifndef DHS_SERVERHTTP_H
#define DHS_SERVERHTTP_H
#include "crow.h"
#include "ThreadBase.h"
#include "router.h"

class ServerHttp:public ThreadBase {
private:
    std::thread thServerhttp;
    std::string FQuestPath;
public:
    void StartExecute() override;

    void setQuestPath(std::string _path) {
        FQuestPath = _path;
    }
};


void ServerHttp::StartExecute() {
    crow::SimpleApp app;
//    crow::mustache::set_base(".");
//    CROW_ROUTE(app, "/")([](){
//        return FQuestPath;
//    });

//    CROW_ROUTE(app, "/")([this]() {
//        return FQuestPath; // Теперь мы можем вернуть значение FQuestPath
//    });

    // Определяем маршруты в отдельной функции
    define_routes(app);

    app.loglevel(crow::LogLevel::Debug);
    app.port(18080).multithreaded().run();
}


#endif //DHS_SERVERHTTP_H
