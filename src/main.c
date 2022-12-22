#ifndef NO_PYTHON
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <frameobject.h>
#endif

#ifndef NO_PHP
#ifdef PHP_USE_EMBED
#include <sapi/embed/php_embed.h>
#endif
#endif

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <libgen.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <math.h>

#include "ansi-colors.h"
#include "log.h"

#ifndef STDOUT_FILENO
#define STDOUT_FILENO 1
#endif

#ifndef DEFAULT_PORT
#define DEFAULT_PORT "8080"
#endif

#ifndef CONMAX
#define CONNMAX 100
#endif

#define BYTES 1024

char *ROOT;
int listenfd, clients[CONNMAX];
void error(char *);
void start_server(char *);
void respond(int, int, char **);

void interrupt_handler() {
	printf(" %sExiting after interrupt...%s\n", ansi.green, ansi.reset);
#ifndef NO_PYTHON
	Py_Finalize();
#endif
	exit(0);
}

char * int_to_str(int number) {
	int n = log10(number) + 1;
	int i;
	char *numberArray = calloc(n, sizeof(char));
	for (i = n - 1; i >= 0; --i, number /= 10) {
		numberArray[i] = (number % 10) + '0';
	}
	return numberArray;
}

bool str_ends_with(char *string, const char *suffix) {
	int i;
	for (i = 0; ; i++) {
		if (string[i] == '\0') {
			break;
		}
	}
	int string_len = i;
	int suffix_len = strlen(suffix);
	for (i = 0; i < suffix_len; i++) {
		if (string[string_len - suffix_len + i] != suffix[i]) {
			return false;
		}
	}
	return true;
}
void prepend(char *s, const char *t) {
	size_t		len = strlen(t);
	memmove(s + len, s, strlen(s) + 1);
	memcpy(s, t, len);
}
char *client_addr;
char *client_port;
char *server_addr;
char *server_port;

int main(int argc, char *argv[]) {
	signal(SIGTSTP, SIG_IGN);
	signal(SIGINT, interrupt_handler);

	struct sockaddr_in clientaddr;
	struct sockaddr_in serveraddr;
	//struct hostent * server_name;
	socklen_t	addrlen;
	socklen_t	serveraddrlen;
	char c;
	//Default Values PATH = ~/and PORT = 8080
	char PORT[6];
	// Set default server root to current directory
	ROOT = getenv("PWD");
	// Set default port
	strcpy(PORT, DEFAULT_PORT);

	int slot = 0;

	// Parsing the command line arguments
	while ((c = getopt(argc, argv, "p:r:")) != -1) {
		switch (c) {
			case 'r':
				ROOT = malloc(strlen(optarg));
				strcpy(ROOT, optarg);
				break;
			case 'p':
				strcpy(PORT, optarg);
				break;
			case '?':
				fprintf(stderr, "Wrong arguments given!!!\n");
				exit(1);
			default:
				exit(1);
		}
	}

	// Remove that last / at the end if it exists
	if (str_ends_with(ROOT, "/")) {
		ROOT[strlen(ROOT) - 1] = '\0';
	}

	int i;
	for (i = 0; i < CONNMAX; i++) {
		clients[i] = -1;
	}
#ifndef NO_PYTHON
	// Initialize the Python interpreter
	Py_Initialize();
	PyRun_SimpleString("import sys");
#endif
	start_server(PORT);
	printf("Server listening on port %s%s%s with root directory as %s%s%s\n\n", ansi.green, PORT, ansi.reset, ansi.green, ROOT, ansi.reset);

	// Create cache folder if it doesn't exist
	struct stat	st = {0};
	if (stat("/var/cache/http-server", &st) == -1) {
		mkdir("/var/cache/http-server", 0700);
#ifndef NO_PYTHON
		PyRun_SimpleString("sys.pycache_prefix=\"/var/cache/http-server\"");
#endif
	}
	while (true) {
		addrlen = sizeof(clientaddr);
		clients[slot] = accept(listenfd, (struct sockaddr *)&clientaddr, &addrlen);
		serveraddrlen = sizeof(serveraddr);
		if (getsockname(clients[slot], (struct sockaddr *)&serveraddr, &serveraddrlen) != 0) {
			log_error("getsockname(): ", strerror(errno));
		}
		//server_name = gethostbyname()

		if (clients[slot] < 0) {
			log_error("accept(): ", strerror(errno));
		} else {
#ifndef SINGLE_PROC
			if (fork() == 0) {
				client_addr = inet_ntoa(clientaddr.sin_addr);
				client_port = int_to_str(ntohs(clientaddr.sin_port));
				server_addr = inet_ntoa(serveraddr.sin_addr);
				server_port = int_to_str(ntohs(serveraddr.sin_port));
				respond(slot, argc, argv);
				exit(0);
			}
#else
			client_addr = inet_ntoa(clientaddr.sin_addr);
			client_port = int_to_str(ntohs(clientaddr.sin_port));
			server_addr = inet_ntoa(serveraddr.sin_addr);
			server_port = int_to_str(ntohs(serveraddr.sin_port));
			respond(slot, argc, argv);
#endif
		}

		while (clients[slot] != -1) {
			slot = (slot + 1) % CONNMAX;
		}
	}
	return 0;
}

void start_server(char *port) {
	struct addrinfo	hints, *res, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if (getaddrinfo(NULL, port, &hints, &res) != 0) {
		log_error("getaddrinfo()", strerror(errno));
		exit(1);
	}
	for (p = res; p != NULL; p = p->ai_next) {
		listenfd = socket(p->ai_family, p->ai_socktype, 0);
		const int	reuse = 1;
		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
			printf("%sERROR%s: setsockopt(SO_REUSEADDR): %s\n", ansi.red, ansi.reset, strerror(errno));
		}
		if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) < 0) {
			printf("%sERROR%s: setsockopt(SO_REUSEPORT): %s\n", ansi.red, ansi.reset, strerror(errno));
		}
		if (listenfd == -1) {
			continue;
		}
		if (bind(listenfd, p->ai_addr, p->ai_addrlen) == 0) {
			break;
		}
	}
	if (p == NULL) {
		log_error("bind() or socket()", strerror(errno));
		exit(1);
	}

	freeaddrinfo(res);

	// Listen for incoming connections
	if (listen(listenfd, 1000000) != 0) {
		log_error("listen()", strerror(errno));
		exit(1);
	}
}

bool try_index(char **path, char *index_file) {
	char *new_path = (char *)malloc(strlen(*path) + 1);
	strcpy(new_path, *path);
	if (!str_ends_with(new_path, "/")) {
		strcat(new_path, "/");
	}
	strcat(new_path, index_file);
	if (access(new_path, F_OK) == 0) {
		*path = (char *)malloc(strlen(new_path) + 1);
		strcpy(*path, new_path);
		return true;
	} else {
		return false;
	}
}
bool try_file_suffix(char **path, char *suffix) {
	char *new_path = (char *)malloc(strlen(*path) + 1);
	strcpy(new_path, *path);
	strcat(new_path, suffix);
	if (access(new_path, F_OK) == 0) {
		*path = (char *)malloc(strlen(new_path) + 1);
		strcpy(*path, new_path);
		return true;
	} else {
		return false;
	}
}

int cstdout = STDOUT_FILENO;
void respond(int n, int argc, char **argv) {
	char mesg[99999], *reqline[3], data_to_send[BYTES];
	int rcvd, fd, bytes_read;

	memset((void *)mesg, (int)'\0', 99999);

	rcvd = recv(clients[n], mesg, 99999, 0);

	const char *resp_200_header = "HTTP/1.0 200 OK\n\n";

	const char *resp_400_header = "HTTP/1.0 400 Bad Request\n\n";
	const char *default_resp_400_page = "<html><body><center><b>400</b><p>Bad Request</p></center></html></body>\n";

	const char *resp_404_header = "HTTP/1.0 404 Not Found\n\n";
	const char *default_resp_404_page = "<html><body><center><b>404</b><p>Not Found</p></center></html></body>\n";

	const char *resp_500_header = "HTTP/1.0 500 Internal Server Error\n\n";
	const char *default_resp_500_page = "<html><body><center><b>500</b><p>Internal Server Error</p></center></html></body>\n";

	if (rcvd < 0) {
		printf("%sERROR%s: recv(): %s\n", ansi.red, ansi.reset, strerror(errno));
	} else if (rcvd == 0) {
		log_error("recv()", "Client disconnected unexpectedly");
	} else if (!strstr(reqline[1], "..")) {
		reqline[0] = strtok(mesg, " \t\n");
		reqline[1] = strtok(NULL, " \t");
		reqline[2] = strtok(NULL, " \t\n");
		if (strncmp(reqline[2], "HTTP/1.0", 8) != 0 && strncmp(reqline[2], "HTTP/1.1", 8) != 0) {
			write(clients[n], resp_400_header, strlen(resp_400_header));
			write(clients[n], default_resp_400_page, strlen(default_resp_400_page));
		} else {
			prepend(reqline[1], ROOT);
			char *url = "";
			char *url_params_str = "";
			char *qmark = strchr(reqline[1], '?');
			char params[100] = "";
			if (qmark != NULL) {
				url = strtok(reqline[1], "?");
				if (reqline[1][qmark - reqline[1] + 1] != '\0') {
					url_params_str = strtok(NULL, "");
					strcpy(params, url_params_str);
				}
				strcpy(reqline[1], url);
			} else {
				url = reqline[1];
			}
			char *token = strtok(url_params_str, "&");
			char *url_params[100] = {0};
			int url_params_count = 0;
			int i = 0;
			while (token != NULL) {
				url_params[i] = token;
				token = strtok(NULL, "&");
				url_params_count++;
				i++;
			}
			//going to turn this into a list of possible page indexes, and then iterate through them.
			DIR *folder;
			if ((folder = opendir(reqline[1])) != NULL) {
				if (try_index(&reqline[1], "index.html") != true) {
					if (try_index(&reqline[1], "index.php") != true) {
						if (try_index(&reqline[1], "index.py") != true) {
						}
					}
				}
			}   /* else if
				 * (try_file_suffix(&reqline[1],
				 * ".html") != true) { if
				 * (try_file_suffix(&reqline[1],
				 * ".php") != true) { if
				 * (try_file_suffix(&reqline[1],
				 * ".py") != true) { } } } */
			printf("Client accessed file: %s%s%s\n\n", ansi.blue, reqline[1], ansi.reset);

			if ((fd = open(reqline[1], O_RDONLY)) != -1) {
				if (false) {}
#ifndef NO_PYTHON
				else if (str_ends_with(reqline[1], ".py")) {
					char import_path[999] = "";
					sprintf(import_path, "sys.path.append(\"%s\")", ROOT);
					PyRun_SimpleString(import_path);
					PyObject *page_module = PyImport_ImportModule(strtok(basename(reqline[1]), "."));
					PyObject *type, *value, *traceback;
					PyErr_Fetch(&type, &value, &traceback);
					if (type != NULL) {
						PyErr_Restore(type, value, traceback);
						PyErr_Print();
						printf("\n");
						write(clients[n], resp_500_header, strlen(resp_500_header));
						write(clients[n], default_resp_500_page, strlen(default_resp_500_page));
						shutdown(clients[n], SHUT_RDWR);
						close(clients[n]);
						clients[n] = -1;
						return;
					}
					PyObject *page_main_func = PyObject_GetAttrString(page_module, "http_main");
					PyObject *page_url_params = PyDict_New();
					PyObject *param = NULL;
					char *param_name = "";
					char *param_val = "";
					for (int i = 0; i != url_params_count; ++i) {
						param_name = strtok(url_params[i], "=");
						param_val = strtok(NULL, "=");
						param = Py_BuildValue("s", (const char *) param_val);
						PyDict_SetItemString(page_url_params, param_name, param);
					}
					PyErr_Fetch(&type, &value, &traceback);
					if (type != NULL) {
						PyErr_Restore(type, value, traceback);
						PyErr_Print();
						write(clients[n], resp_500_header, strlen(resp_500_header));
						write(clients[n], default_resp_500_page, strlen(default_resp_500_page));
						shutdown(clients[n], SHUT_RDWR);
						close(clients[n]);
						clients[n] = -1;
						return;
					}
					PyObject *result = PyObject_CallObject(page_main_func, Py_BuildValue("(sOs)", reqline[1], page_url_params, reqline[0]));
					PyErr_Fetch(&type, &value, &traceback);
					if (type != NULL) {
						PyErr_Restore(type, value, traceback);
						PyErr_Print();
						printf("\n");
						write(clients[n], resp_500_header, strlen(resp_500_header));
						write(clients[n], default_resp_500_page, strlen(default_resp_500_page));
					} else {
						char *page_content = (char *) PyUnicode_AsUTF8(result);
						write(clients[n], resp_200_header, strlen(resp_200_header));
						write(clients[n], page_content, strlen(page_content));
					}
				}
#endif

#ifndef NO_PHP
				else if (str_ends_with(reqline[1], ".php")) {
					// PHP $_SERVER indices, incomplete
					setenv("SERVER_ADDR", server_addr, 1);
					setenv("REMOTE_ADDR", client_addr, 1);
					setenv("SERVER_SOFTWARE", "SimpleHTTP", 1);
					setenv("SERVER_PROTOCOL", "HTTP/1.0", 1);
					setenv("REQUEST_METHOD", reqline[0], 1);
					setenv("SERVER_PORT", server_port, 1);
					setenv("QUERY_STRING", params, 1);
					setenv("DOCUMENT_ROOT", ROOT, 1);
					//setenv("SCRIPT_NAME", test, 1);

#ifndef PHP_USE_EMBED
					write(clients[n], resp_200_header, strlen("HTTP/1.0 200 OK\n"));
					write(clients[n], "Content-Type:text/html\n\n", strlen("Content-Type:text/html\n\n"));
					int fd[2];
					pipe(fd);
					if (fork() == 0) {
						dup2(fd[0], STDOUT_FILENO);
						execlp("php", "php", reqline[1], NULL);
					} else {
						char buffer[1000];
						size_t size = read(fd[1], buffer, 1000);
						if ((size > 0) && (size < sizeof(buffer))) {
							buffer[size] = '\0';
							write(clients[n], buffer, strlen(buffer));
						}
					}
#else
					int pipefd[2];
					// Pipe into PHP's stdout buffer
					if (pipe2(pipefd, O_NONBLOCK) == -1) {
						perror("pipe");
						exit(EXIT_FAILURE);
					}

					// Copy stdout before overriding
					cstdout = dup(STDOUT_FILENO);

					// Override stdout with pipe
					if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
						perror("dup2");
						exit(EXIT_FAILURE);
					}

					char *no_args[0] = {};
					PHP_EMBED_START_BLOCK(0, no_args)

					zend_file_handle file_handle;
					zend_stream_init_filename(&file_handle, reqline[1]);

					if (php_execute_script(&file_handle) != 1) {
						php_printf("Failed to execute PHP script.\n");
					}

					PHP_EMBED_END_BLOCK()

					// Restore stdout and close pipe input
					if (dup2(cstdout, STDOUT_FILENO) == -1) {
						perror("dup2 restore");
						exit(EXIT_FAILURE);
					}
					char buf[2048];
					int bytes;
					write(clients[n], "HTTP/1.0 200 OK\n", strlen("HTTP/1.0 200 OK\n"));
					write(clients[n], "Content-Type:text/html\n\n", strlen("Content-Type:text/html\n\n"));
					while ((bytes = read(pipefd[0], &buf, 2048)) != -1) {
						write(clients[n], (char *)&buf, strlen((char *)&buf));
					}
					close(pipefd[0]);
#endif
					}
#endif
				else {
					// Serve as static
					char *content_type;
					if (str_ends_with(reqline[1], ".html") || str_ends_with(reqline[1], ".htm")) {
						content_type = "Content-Type:text/html\n\n";
					} else if (str_ends_with(reqline[1], ".txt")) {
						content_type = "Content-Type:text/plain\n\n";
					} else if (str_ends_with(reqline[1], ".js")) {
						content_type = "Content-Type:text/javascript\n\n";
					} else if (str_ends_with(reqline[1], ".css")) {
						content_type = "Content-Type:text/css\n\n";
					} else if (str_ends_with(reqline[1], ".png")) {
						content_type = "Content-Type:image/png\n\n";
					} else if (str_ends_with(reqline[1], ".jpeg") || str_ends_with(reqline[1], ".jpg")) {
						content_type = "Content-Type:image/jpeg\n\n";
					} else if (str_ends_with(reqline[1], ".webp")) {
						content_type = "Content-Type:image/webp\n\n";
					} else {
						// Download file without knowing what it is
						content_type = "Content-Disposition:attachment\n\n";
					}
					write(clients[n], "HTTP/1.0 200 OK\n", strlen("HTTP/1.0 200 OK\n"));
					write(clients[n], content_type, strlen(content_type));
					while ((bytes_read = read(fd, data_to_send, BYTES)) > 0) {
						write(clients[n], data_to_send, bytes_read);
					}
				}
			} else {
				write(clients[n], resp_404_header, strlen(resp_404_header));
				write(clients[n], default_resp_404_page, strlen(default_resp_404_page));
			}
		}

		shutdown(clients[n], SHUT_RDWR);
		close(clients[n]);
		clients[n] = -1;
	}
}
