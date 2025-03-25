#ifndef DHS_ROUTER_H
#define DHS_ROUTER_H

#include "crow.h"
#include "controller.h"

#define ROUTER_PREFIX_QUEST "quest"

void define_routes(crow::SimpleApp& app)
{

    CROW_ROUTE(app, "/" ROUTER_PREFIX_QUEST "/<int>")
            .methods(crow::HTTPMethod::Get)(Questhandle);

    CROW_ROUTE(app, "/questions")
            ([]() {
                return QuestvueHandler();  // �������� ��� � ������
            });


//    // ������� ��� ������� ��������
//    CROW_ROUTE(app, "/quest/<int>")
//        ([](const crow::request& req, int id) {
//
//            // ������� JSON ������
//            crow::json::wvalue result;
//            result["post_id"] = id;
//            result["name"] = "Kirill";
//
//            QuestFilePath = "Questions\\1234.dgp";
//            DialogAPI* QuestAPI =
//            bool isOpenFile = QuestAPI->init(QuestFilePath);
//            if(! isOpenFile) {
//                Logger::log(Logger::LogLevel::log_ERROR, "Error open quest FILE %s ", QuestFilePath.c_str());
//                usSGP2_ML_MessagePutInt(pxDstResponseMessage, 0xE1);
//                return;
//            }
//
//
//            // ���������� JSON �����
//            return crow::response(result);
//    });

    //    CROW_ROUTE(app, "/post/<int>")
//            ([](const crow::request& req, int id) {
//                std::ostringstream os;
//                os << "Post ID: " << id;
//                return crow::response(os.str());  // ������ ID � ������
//            });

//==============Example=====================================
//    // ������� ��� ������� ��������
//    CROW_ROUTE(app, "/")([](){
//        return "Welcome to the server!";
//    });
//
//    // ������ ��������, ������� ���������� ����������
//    CROW_ROUTE(app, "/data")
//            ([]() {
//                return get_data();  // ����� ������� �� �����������
//            });
//
//    CROW_ROUTE(app, "/html_with_data/<string>")
//            ([](const crow::request& req, const std::string& name) {
//                return render_html_with_data(name);  // �������� ���, ������� ������� � ����
//            });
//
//    CROW_ROUTE(app, "/post/<int>")
//        .methods(crow::HTTPMethod::Get)(handlePostRequest);


//    EXAMPLES:
//    CROW_ROUTE(app, "/post/<int>")
//            ([](const crow::request& req, int id) {
//                std::ostringstream os;
//                os << "Post ID: " << id;
//                return crow::response(os.str());  // ������ ID � ������
//            });

//    CROW_ROUTE(app, "/user/<int>/post/<int>")
//            ([](const crow::request& req, int userId, int postId) {
//                std::ostringstream os;
//                os << "User ID: " << userId << ", Post ID: " << postId;
//                return crow::response(os.str());
//            });
//
//    CROW_ROUTE(app, "/html_with_data")
//            ([]() {
//                return render_html_with_data("John Doe");  // �������� ��� � ������
//            });
//

//
//    CROW_ROUTE(app, "/vue3")
//            ([]() {
//                return test_vue3("VUE 3");  // �������� ��� � ������
//            });


}

//void define_routes(crow::SimpleApp& app) {
//    // ������� ��������� �����������
//    DataController controller;
//
//    // ������� ��� ��������� ������
//    CROW_ROUTE(app, "/data")
//            ([&controller]() {
//                return controller.get_data();  // ����� ������ �����������
//            });
//
//    // ������� ��� ���������� HTML
//    CROW_ROUTE(app, "/html")
//            ([&controller]() {
//                return controller.render_html();  // ����� ������ ��� ���������� HTML
//            });
//}


#endif //DHS_ROUTER_H
