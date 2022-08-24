# Simple HTTP
A small Linux HTTP server that can serve static files (HTML, TXT, etc.) and server-generated web pages from Python/PHP.
Usage:
./server -p <port> -r <server root>
You'd want the port to be 80, since that's the default port. You need to be ROOT to use it (so run `sudo ./server ...` instead).
The ideal server root would be `/srv`, or `/srv/http` if you're running an SFTP/FTP server (or any other protocol). Use `/srv/http/<website name>` if you have multiple ports/websites running.
You should probably know by now that you can access the data from your IP address (+ ":<port name>" if it's not 80).

## Python/PHP web apps
### Python
``` python
def http_main(origin, params, method):
  return("<p>Hello, world!</p>")
```
Where origin would be the URL without query parameters, params would be a dict of query parameters, and method would be `GET`/`POST` or whatever method is being used.
You can install MySQL, do a secure installation, configure users/whatever, then use the Python MySQL Connector to connect to the database.
### PHP
Though it's not fully supported yet, the server can run PHP files.
Be careful, though. If an error occurs in the script, it could leak dangerous information (that's intended for debbugging use), instead of returning error 500 (which means an internal server error occured).

## The source explained
The Linux socket part was easy, this is how it goes:
Listen for connections, respond, close.
The server directly communicates to PHP/Python via their API if it's responding with a dynamically-generated web page (a web app) without CGI/WSGI/FastCGI or anything similar. This is done with the Python C API and the PHP embed SAPI.
Since the PHP SAPI isn't documented, setting it up was a daunting task. I couldn't do it without the aid of <a href="https://github.com/KapitanOczywisty">KapitanOczywisty</a>, so thanks to him!
Make use of this source... there are very few projects out there which do the same.

## Contributions are welcome!
There are many things you can do to help:
- Full PHP support: `$_SERVER` environment variables aren't filled out by the server
- Run in background: As a service/thread/whatever. I've never done such thing before.
- Security: I haven't looked into security yet.
- Clean up the code: Seriously... I've mixed tabs and spaces together...
Or you can add a feature of your own, perhaps fix a bug, anything! You are welcome to open a pull request/issue.
