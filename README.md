# Shutokou Battle Online Game Server
A Japanese only Online multiplayer Tokyo Xtreme Racer game was shutdown around 2005. With no packet logs or any documentation of how the game worked<br/><br/>

With only the client I started work on reverse engineering the game code to figure out how to write a server for the game. So far this project allows you to create an account, select a car and drive around the highway. There is also a DB server which handles user accounts and saved data.<br/><br/>

It currently only runs SQLite but will support MySQL and possibly more.<br/><br/>

Check out the [Wiki](http://wiki.sb-online.net) for packet documentation and the [Blog](http://sb-online.net/blog/) for any progress updates.