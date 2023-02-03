# State Daemon

### Current Status
The State daemon is a work in progress. This interface documentation will get updated as more channels get added with their proper type.

### Incoming messages

* pose [nav_msgs::Odometry] : The current position of the AGV.
* velocity [nav_msgs::Odometry] : The current speed of the AGV.  
* mapId [std_msgs::String] : The ID of the map used by the AGV.  

### Outgoing messages

