#ifndef DAEMON_H
#define DAEMON_H
#include <ros/ros.h>
#include <string>
#include "boost/date_time/posix_time/posix_time.hpp"
#include <ros/console.h>
#include "std_msgs/String.h"
#include "vda5050_msgs/Header.h"


#define DEFAULT_ERROR_TOPIC "/internal_errors"


class Daemon
{
	private:
	vda5050_msgs::Header messageHeader;
	std::map<std::string,std::string> topicPublisherList;
	std::map<std::string,std::string> topicSubscriberList;

	
	protected:
	std::map<std::string,ros::Publisher> messagePublisher;
	std::map<std::string,ros::Subscriber> subscriber;
	ros::Publisher errorPublisher;
	
	public:
	Daemon(ros::NodeHandle *nh, std::string daemonName);
	std::map<std::string,std::string> GetTopicPublisherList();
	std::map<std::string,std::string> GetTopicSubscriberList();
	std::vector<std::string> GetMsgList(std::map<std::string,std::string>);
	std::string GetParameter(std::string param);
	std::string CreateTimestamp();
	void InitHeaderInfo();
	void LinkErrorTopics(ros::NodeHandle *nh);
	void UpdateHeader();
	std::map<std::string,std::string> ReadTopicParams(std::string paramTopicName);
	vda5050_msgs::Header GetHeader();
};

#endif

