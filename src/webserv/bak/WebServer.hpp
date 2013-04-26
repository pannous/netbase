/*
 * Modification History
 *
 * 2002-March-12   Jason Rohrer
 * Created.
 *
 * 2002-April-5    Jason Rohrer
 * Changed to extend StopSignalThread.
 * Added timeouts to socket accept to support checking for stop signal.
 *
 * 2002-August-2   Jason Rohrer
 * Added use of ConnectionPermissionHandler.
 *
 * 2002-August-6   Jason Rohrer
 * Changed member init order.
 *
 * 2002-September-17   Jason Rohrer
 * Removed argument to ConnectionPermissionHandler constructor.
 *
 * 2003-September-5   Jason Rohrer
 * Moved into minorGems.
 */


WebServer::WebServer(int inPort, PageGenerator *inGenerator );
WebServer::~WebServer();
void WebServer::run();