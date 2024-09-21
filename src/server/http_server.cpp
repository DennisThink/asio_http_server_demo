#include <client_http.hpp>
#include <future>
#include <server_http.hpp>

// Added for the default_resource example
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <vector>
#ifdef HAVE_OPENSSL
#include "crypto.hpp"
#endif


using HttpServer = SimpleWeb::Server<SimpleWeb::HTTP>;
using Response_SHARED_PTR = std::shared_ptr<HttpServer::Response>;
using Request_SHARED_PTR = std::shared_ptr<HttpServer::Request>;
class CWordTranslateServer
{

//Function
public:
    CWordTranslateServer();
    virtual ~CWordTranslateServer();
public:
    bool Start();
    bool Stop();
protected:
    void OnVersion(Response_SHARED_PTR response,
        Request_SHARED_PTR request);
    void OnEnglishToChinese(Response_SHARED_PTR response,
        Request_SHARED_PTR request);

    void OnDefaultGet(Response_SHARED_PTR response,
        Request_SHARED_PTR request);

    void OnDefaultPost(Response_SHARED_PTR response,
        Request_SHARED_PTR request);
private:
    bool MatchUrlMethodAndFunction();
    void ShowRemotePeer(Request_SHARED_PTR request);

private:
    HttpServer m_server;
};

CWordTranslateServer::CWordTranslateServer()
{
    m_server.config.port = 8080;
}

CWordTranslateServer::~CWordTranslateServer()
{

}

void CWordTranslateServer::ShowRemotePeer(Request_SHARED_PTR request)
{
    std::cout<< request->remote_endpoint().address().to_string() << ":"
        << request->remote_endpoint().port();
}
bool CWordTranslateServer::Start()
{
    MatchUrlMethodAndFunction();
    std::promise<unsigned short> server_port;
    std::thread server_thread([this, &server_port]() {
        // Start server
        m_server.start(
            [&server_port](unsigned short port) { server_port.set_value(port); });
        });
    std::cout << "Server listening on port " << server_port.get_future().get()
        << std::endl
        << std::endl;
    server_thread.join();
    return false;
}

bool CWordTranslateServer::Stop()
{
    return false;
}

bool CWordTranslateServer::MatchUrlMethodAndFunction()
{
    m_server.resource["^/version$"]["GET"] = [this](Response_SHARED_PTR response,
        Request_SHARED_PTR request) {
            OnVersion(response, request);
        };

    m_server.resource["^/v1/english_to_chinese$"]["POST"] = [this](Response_SHARED_PTR response,
        Request_SHARED_PTR request) {
            OnEnglishToChinese(response, request);
        };
    m_server.default_resource["GET"] = [this](Response_SHARED_PTR response,
        Request_SHARED_PTR request) {
            OnDefaultGet(response, request);
        };
    m_server.default_resource["POST"] = [this](Response_SHARED_PTR response,
        Request_SHARED_PTR request) {
           OnDefaultPost(response, request);
        };
    return false;
}
void CWordTranslateServer::OnVersion(Response_SHARED_PTR response,
    Request_SHARED_PTR request)
{
    ShowRemotePeer(request);
    std::string strVersion = "VERSION 0.0.1";
    *response << "HTTP/1.1 200 OK\r\n"
        << "Content-Length: " << strVersion.length() << "\r\n\r\n"
        << strVersion;
}
void CWordTranslateServer::OnEnglishToChinese(Response_SHARED_PTR response,
    Request_SHARED_PTR request)
{
    ShowRemotePeer(request);
}

void CWordTranslateServer::OnDefaultGet(Response_SHARED_PTR response,
    Request_SHARED_PTR request)
{
    ShowRemotePeer(request);
}

void CWordTranslateServer::OnDefaultPost(Response_SHARED_PTR response,
    Request_SHARED_PTR request)
{
    ShowRemotePeer(request);
}

int main() {
    CWordTranslateServer server;
    server.Start();
}
