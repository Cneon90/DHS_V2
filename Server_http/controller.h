#ifndef DHS_CONTROLLER_H
#define DHS_CONTROLLER_H

#include <string>
#include <sstream>
#include "DialogAPI.h"

/***
 * Получение диалога (Вопрос с вариантами ответа, или решение)
 *
 * @param req  request
 * @param id  ID диалога (вопрос или решение) Если ID = 0, значит инициализация, возвращаем первый диалог из файла
 *
 * @return    JSON Вопрос или решение
 */
crow::response Questhandle(const crow::request& req, int id)
{
    crow::json::wvalue result;
    DialogAPI QuestAPI;
    const std::string QuestFilePath = DialogAPI::FileQuestPath;
    // Try to initialize QuestAPI with the specified file path
    if (!QuestAPI.init(QuestFilePath)) {
        Logger::log(Logger::LogLevel::log_ERROR, "API Error opening quest file: %s", QuestFilePath.c_str());
        result["Error"] = "File not found";
        crow::response resp(result);
        resp.code = 500;  // Set status code to 500 for internal server error
        resp.set_header("Content-Type", "application/json; charset=windows-1251");  // Set content-type header
        return resp;  // Return error response
    }

    unsigned int DialogID = (id == 0) ? QuestAPI.getQuestID_First() : id;
    // Get the dialog item by its ID
    strSgDialogItem DialogItem = QuestAPI.getDialogItemByID(DialogID);

    switch (DialogItem.eType) {
        case SDIT_DIALOG: {
            Logger::log(Logger::LogLevel::log_DEBUG, "Handling QUEST");
            TDialogDTO dialog = QuestAPI.getDialogDTO(DialogItem);

            result["Question"] = dialog.QuestText.c_str();

            for (int i = 0; i < dialog.AnswerCount; i++) {
                result["Answer"][i]["id"] = i;
                result["Answer"][i]["Text"] = dialog.AnswerText[i].c_str();
                result["Answer"][i]["NextActID"] = dialog.NextActID[i];
            }
            break;
        }

        case SDIT_SOLUTION: {
            Logger::log(Logger::LogLevel::log_DEBUG, "Handling SOLUTION");
            result["Solution"] = QuestAPI.getSolutionText(DialogItem.uiID);
            break;
        }

        default: {
            Logger::log(Logger::LogLevel::log_ERROR, "ERROR | Dialog not found for ID: %u", DialogID);
            result["Error"] = "Dialog not found";
            crow::response resp(result);
            resp.code = 404;  // Set status code to 404 if dialog is not found
            resp.set_header("Content-Type", "application/json; charset=windows-1251");  // Set content-type header
            return resp;  // Return 404 if dialog is not found
        }
    }

    // Create a response and set headers after handling the dialog
    crow::response resp(result);
    resp.set_header("Content-Type", "application/json; charset=windows-1251");  // Set content-type header
    return resp;
}

//crow::response Questhandle(const crow::request& req, int id)
//{
//    crow::json::wvalue result;
//
//    std::string QuestFilePath = "Questions\\1234.dgp";
////    auto* QuestAPI = new DialogAPI();
//    DialogAPI QuestAPI;
//    bool isOpenFile = QuestAPI.init(QuestFilePath);
//    if(! isOpenFile) {
//        Logger::log(Logger::LogLevel::log_ERROR, "API Error open quest FILE %s ", QuestFilePath.c_str());
//        result["Error"] = " not file";
//
//    }
//
//    unsigned int RequestID = id;
//    unsigned int DialogID = 0;
//
//    if(RequestID == 0) {
//        DialogID = QuestAPI.getQuestID_First();	// Init Dialog (search first diaolog)
//    } else {
//        DialogID = RequestID;
//    }
//
//    // Узнаем тип запрашиваемого item
//    strSgDialogItem DialogItem = QuestAPI.getDialogItemByID(DialogID);
//
//    switch (DialogItem.eType) {
//        case SDIT_DIALOG: {
//            Logger::log(Logger::LogLevel::log_DEBUG, "QUEST");
//            TDialogDTO dialog = QuestAPI.getDialogDTO(DialogItem);
//
//            result["Question"] = dialog.QuestText.c_str();
//
//            for(int i=0; i < dialog.AnswerCount; i++) {
//                result["Answer"][i]["Text"] = dialog.AnswerText[i].c_str();
//                result["Answer"][i]["NextActID"] = dialog.NextActID[i];
//            }
//            break;
//        }
//
//        case SDIT_SOLUTION: {
//            Logger::log(Logger::LogLevel::log_DEBUG, "SOLUTION");
//            result["Solution"] = QuestAPI.getSolutionText(DialogItem.uiID);
//            break;
//        }
//
//        default: {
//            Logger::log(Logger::LogLevel::log_ERROR, "ERROR | Dialog not found");
//            result["Error"] = "Not found";
//            // Устанавливаем заголовок ответа и статус 404
//            crow::response res;
//            res.code = 404;  // Устанавливаем статус код 404
//            res.body = result.dump();
//
//            return res;
//            break;
//        }
//    }
//
//    crow::response resp(result);
////    delete QuestAPI;
//    resp.set_header("Content-Type", "application/json; charset=windows-1251");  // Указываем кодировку
//    return resp;
//}




//std::string get_data() {
//    return "This is some data from the controller.";
//}
//
//crow::response render_html_with_data(const std::string& name) {
//    crow::mustache::template_t tmpl = crow::mustache::load("template.html");
//    crow::mustache::context ctx;
//    ctx["name"] = name;  // Передача данных в шаблон
//    printf("TEST!");
//
////    crow::mustache::set_base("");
//    return crow::response(tmpl.render(ctx));  // Рендеринг шаблона
//}
//
//crow::response handlePostRequest(const crow::request& req, int id) {
//    std::ostringstream os;
//    os << "Post ID: " << id;
//    return crow::response(os.str());
//}
//

crow::response QuestvueHandler() {
    crow::mustache::template_t tmpl = crow::mustache::load("questv2.html");
    crow::mustache::context ctx;

    return crow::response(tmpl.render(ctx));  // Рендеринг шаблона
}
//
//crow::response test_vue3(const std::string& name) {
//    crow::mustache::template_t tmpl = crow::mustache::load("testvue_3.html");
//    crow::mustache::context ctx;
//
//    ctx["name"] = name;  // Передача данных в шаблон
//    return crow::response(tmpl.render(ctx));  // Рендеринг шаблона
//}




#endif //DHS_CONTROLLER_H
