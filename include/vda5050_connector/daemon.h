#ifndef DAEMON_H
#define DAEMON_H
#include <ros/ros.h>
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <ros/console.h>
#include "std_msgs/String.h"
#include "vda5050_msgs/Header.h"
#include <sstream>
#include <tf/tf.h>


#define DEFAULT_ERROR_TOPIC "/internal_errors"

/**
 * Model for all daemons. Every daemon provides some functionality to translate
 * messages between the robot's internal communication and the VDA-5050-based
 * client-server communication.
 */
class Daemon
{
	private:
	vda5050_msgs::Header messageHeader;
		/**< TODO: is this variable even used? */

	std::map<std::string,std::string> topicPublisherList;
		/**< All topics to publish on. Map from topic keys to user-defined
		 *   topic names.
		 */
		
	std::map<std::string,std::string> topicSubscriberList;
		/**< All topics to subscribe to. Map from topic keys to user-defined
		 *   topic names.
		 */

	std::string mqttTopicStructurePrefix;
		/**< Prefix that should be placed at the beginning of the MQTT topics.
		 */

	bool testMode;
		/**< Toggle for test/debugging mode. */
	

	protected:
	std::map<std::string,ros::Publisher> messagePublisher;
		/**< All publishers the daemon uses. Map from topic keys to ROS
		 *   Publisher objects.
		 */

	std::map<std::string,ros::Subscriber> subscribers;
		/**< All subscribers the daemon uses. Map from topic keys to ROS
		 *   Subscriber objects.
		 */

	ros::Publisher errorPublisher;
		/**< ROS Publisher for error messages. */

	ros::NodeHandle nh;
		/**< ROS node handle, needed to call ROS functions. */

	
	public:
	
	/**
	 * Default constructor for daemon objects. TODO: Do we need this?
	 */
	Daemon();

	/**
	 * Constructor for daemon objects.
	 * 
	 * @param nh          ROS node handle object.
	 * @param daemonName  Name of the daemon.
	 */
	Daemon(ros::NodeHandle *nh, std::string daemonName);

	/**
	 * Fetches the header message and publishes the state message. Updates
	 * timestamp since last publishing.
	 */
	void PublishState();

	/**
	 * Checks all the logic within the state daemon. For example, it checks
	 * if 30 seconds have passed without update.
	 */
	void UpdateState();

	/**
	 * Calculates the passed time between last update interval and now.
	 * 
	 * @return  Returns true if passed time since last publish is greater than
	 *          30 seconds, else returns false.
	 */
	bool CheckPassedTime();
	
	/**
	 * Empty comment.
	 * 
	 * @return	Empty return comment.
	 */
	std::map<std::string,std::string> GetTopicPublisherList();
	
	/**
	 * Empty comment.
	 * 
	 * @return	Empty return comment.
	 */
	std::map<std::string,std::string> GetTopicSubscriberList();
	
	/**
	 * Empty comment.
	 *
	 * @param param	Empty param comment.
	 * @return	Empty return comment.
	 */
	std::vector<std::string> GetMsgList(std::map<std::string,std::string>);
	
	/**
	 * Empty comment.
	 * 
	 * @param param	Empty param comment.
	 * @return	Empty return comment.
	 */
	std::string GetParameter(std::string param);
	
	/**
	 * Empty comment.
	 *
	 * @return	Empty return comment.
	 */
	std::string CreateTimestamp();
	
	/**
	 * Empty comment.
	 */
	void createTopicStructurePrefix();
	
	/**
	 * Empty comment.
	 * 
	 * @return	Empty return comment.
	 */
	std::string getTopicStructurePrefix();

	/**
	 * Checks if the second topic is a child topic of the first topic.
	 * 
	 * @param str1  Name of the first topic.
	 * @param str2  Name of the topic that might be a child topic of the other
	 *              topic.
	 */
	bool CheckTopic(std::string str1,std::string str2);

	/**
	 * Empty description.
	 */
	std::string GetTopic(std::string hierarchical_topic);

	/**
	 * Checks if the second string is contained in the first string.
	 * 
	 * @param str1  Some string.
	 * @param str2  Second string that might be a substring of the other string.
	 */
	bool CompareStrings(std::string str1, std::string str2);

	/**
	 * Empty description.
	 */
	void InitHeaderInfo();
	
	/**
	 * Empty description.
	 * 
	 * @param nh  Pointer to the ROS node handle.
	 */
	void LinkErrorTopics(ros::NodeHandle *nh);

	/**
	 * Empty description.
	 */
	void UpdateHeader();

	/**
	 * Read in the user-specified topic names. The user can specify names for
	 * the topics that contain the needed information. For mapping the contents
	 * to the topic names, this method scans the parameter server.
	 * 
	 * @param nh              Pointer to the ROS node handle.
	 * @param paramTopicName  Name of the param family to scan through.
	 * 
	 * @return                Map from topic descriptor to user-defined topic
	 *                        name.
	 */
	std::map<std::string,std::string> ReadTopicParams(ros::NodeHandle *nh,
		std::string paramTopicName);

	/**
	 * Get the header of a ROS message. TODO which message is processed?
	 * 
	 * @return  Header of the message.
	 */
	vda5050_msgs::Header GetHeader();

	/**
	 * Empty description.
	 * 
	 * @param lowerRange  Empty description.
	 * @param upperRange  Empty description.
	 * @param value       Empty description.
	 * @param msg_name    Empty description.
	 */
	bool CheckRange(double lowerRange, double upperRange, double value,
		std::string msg_name);
};

#endif

