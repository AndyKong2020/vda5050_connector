#include "vda5050_connector/action_daemon.h"
#include "std_msgs/String.h"
#include <iostream>
#include <vector>
#include <string>
#include "vda5050_msgs/ActionStates.h"

using namespace std;

/*
 * Currently, the action daemon is only used for passing messages from mqtt topic to ros topic
 */
ActionDaemon::ActionDaemon() : Daemon(&(this->nh), "action_daemon")
{
	LinkPublishTopics(&(this->nh));
	LinkSubscriptionTopics(&(this->nh));

	// Initialize internal topics
	orderActionSub = nh.subscribe("orderAction", 1000, &ActionDaemon::OrderActionCallback, this);
	instantActionSub = nh.subscribe("instantAction", 1000, &ActionDaemon::InstantActionsCallback, this);

	actionStatesPub = nh.advertise<vda5050_msgs::ActionStates>("actionStates", 1000);
	orderCancelPub = nh.advertise<std_msgs::String>("orderCancel", 1000);
	actionToAgvPub = nh.advertise<std_msgs::String>("action_to_agv", 1000);
}


void ActionDaemon::LinkPublishTopics(ros::NodeHandle *nh)
{
	std::map<std::string,std::string>topicList = GetTopicPublisherList();
	for(const auto& elem : topicList)
	{
		if (CheckTopic(elem.first,"action"))
		{
			messagePublisher[elem.second] = nh->advertise<vda5050_msgs::InstantActions>(elem.second,1000);
		}
	}	
}

void ActionDaemon::LinkSubscriptionTopics(ros::NodeHandle *nh)
{
	std::map<std::string,std::string>topicList = GetTopicSubscriberList();
	for(const auto& elem : topicList)
	{
		if (CheckTopic(elem.first,"action"))
			subscribers[elem.first]=nh->subscribe(elem.second,1000,&ActionDaemon::InstantActionsCallback, this);
	}	
}

void ActionDaemon::PublishActions()
{	
	messagePublisher["/action"].publish(iActionMessage);	
}

void ActionDaemon::InstantActionsCallback(const vda5050_msgs::InstantActions::ConstPtr& msg)
{
	iActionMessage.headerId=msg->headerId;
	iActionMessage.timestamp=msg->timestamp;
	iActionMessage.version=msg->version;
	iActionMessage.manufacturer=msg->manufacturer;
	iActionMessage.serialNumber=msg->serialNumber;	
	iActionMessage.instantActions=msg->instantActions;
}

void ActionDaemon::OrderActionCallback(const std_msgs::String::ConstPtr& msg)
{
  //queue new Action (FIFO)
  ;
}

void ActionDaemon::UpdateActions()
{
	// get order actions
	// get instantAction topics
	// calculate queue
	// send queue to agv
	// send order cancellations to order_daemon
	// send action status to state_daemon
	;
}

int main(int argc, char **argv)
{	
	ros::init(argc, argv, "action_deamon");
	ros::NodeHandle nh;

	ActionDaemon actionDaemon;

	while(ros::ok())
	{
		actionDaemon.UpdateActions();
		ros::spinOnce();
	}
	return 0;
}