# Step 1 - Server Installation
`make` followed by `make install` or `sudo make install` if you have `sudo` installed and you're not `root` will install SimpleHTTP locally on your system.

# Step 2 - Building The Web App
A web app is the script that controls how the dynamic webpages are presented to the public. Web apps are the scripts (or executables) that power the website.
This can be in a server-side language such as PHP, Python, Java, or any other language as long as the server software supports it.

The scripts (which turn into webpages once the client tries to access them in the URL) are stored in the server root, so if you want the contents of `/home/USERNAME/website` to be accessible, you need to tell SimpleHTTP that that is the document root, using the `-r` command-line option.
If the user tries to access a folder, they'll get a 404 (until I add directory listing to the server), but you can make it result in a webpage by adding an `index.html`, `index.htm`, `index.php` or `index.py` in the folder, so that page will load up instead.
A valid option for the server root would be `/srv/http/WEBSITE_NAME`, but since you shouldn't be running the server as root anyway since that would cause a security risk, `/var/www/WEBSITE_NAME` is what you should chose. But it doesn't make much of a difference.

Start the server via `server` after doing an installation with whatever command-line options you would like to use.
Once starting the server, your webpages can be accessed via `localhos:<PORT_NAME>`, the user uses port `8080` by default, but `80` is the default in HTTP, you can change the default port via the `-p` command-line option.

A web app is normally composed of:
- A main server-side language or HTML for the pages (PHP/HTML)
- A language for the database (can be the same as the main language) (Python, PHP, Java, etc.)
- Database software (MySQL, MariaDB, etc.)

So the homepage could be `index.php` (in the serve root).
CSS can be stored in `assets/css`, JS could be stored in `assets/scripts`, images could be stored in `assets/images`, etc. Though user-generated images like their avatar should be stored in somewhere such as `db/avatars/USERNAME.jpg/jpeg/whatever` rather than in the site's `assets` folder.

If the database software is MySQL, then install MySQL on your system, do a secure installation and set up your database users.
Then, you can use the Python MySQL Connector (or similar software) to access the databse from Python.

You can have a sript named `op/comments/post.py` (`op` stands for operations) to add a comment to the database (in whatever table you desire) and return an exit code, this error can be displayed in JSON or in pure text for the script to interprete.
This script can be called via a JS URL fetch (when clicking a button, for example) so it gets executed when the client asks for it to.
Use URL querry parameters (e.g., `https://example.com/?bears=scary&tigers=20`, sets `bears` to `scary` and `tigers` to `20`) for input to such scripts.
Then you can have a script named `op/comments/read.py` to read the comments and return them in a JSON list (I highly reccomend using JSON), for the page to parse the JSON via JS and then display it in HTML. That way your UI can be altered with ease.
Don't forget error handling in such cases, in case the comment couldn't get posted or if a similar error occurs. (If you're doing a bad word detector, you should be checking for them twice: once in JS via the client, and another in the script in case they disable the detector.)

You can fetch the comments via JS when the page is loaded (or when the user gets to the comments, or when they click a button to display comments, to reduce server hustle).
When the user makes a comment, parse the error into pretty HTML just like how you did with the comments for the sake of the UI.

# Step 3 - Releasing The Website
This is the bit where you have to pay. People connected with the same internet as you can see your site via your IP address (followed by `:<PORT_NAME>` if it's not `80`), but people on the web can't.
You must contact your ISP (Internet Service Provider) for a public IP address. The ISP may or may not charge you for it, it depends.
After that buy a domain name (e.g., `beds.com`) from a domain registrar that points to your public IP address.
Then you'd want SEO (Search Engine Optimization), so Google, Bing and other seach engines know your website and you can get popular.
I haven't done such thing before, so I'm still not really sure how that works.

I'll be making an example with the source code of the above example soon.
Happy coding. :)
