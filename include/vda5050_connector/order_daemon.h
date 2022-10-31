#ifndef ORDER_DAEMON_H
#define ORDER_DAEMON_H
#include <ros/ros.h>
#include "daemon.h"
#include "vda5050_msgs/Order.h"
#include "vda5050_msgs/ActionState.h"
#include "vda5050_msgs/AGVPosition.h"
#include "vda5050_msgs/Edge.h"
#include "vda5050_msgs/Node.h"
#include <string>

using namespace std;

/**
 * Daemon for processing of VDA 5050 action messages. Currently, the action
 * daemon is only used for passing messages from an MQTT topic to a ROS topic.
 */
class ActiveOrder
{
	private:
	string orderId;
	int    orderUpdateId;
	string zoneSetId;
	vector<vda5050_msgs::Edge> edgeList;
	vector<vda5050_msgs::Node> nodeList;
	vector<vda5050_msgs::ActionState> actionList;

	public:
	ActiveOrder(const vda5050_msgs::Order* incomingOrder);

	bool compareOrderId(string orderIdToCompare);

	/**
	 * @brief Compares the incoming order update ID with the currently running order update ID.
	 * 
	 * @param orderUpdateIdToCompare the order upsate ID of the incoming order
	 * @return ["EQUAL", "HIGHER", "LOWER"] if the new order update ID is
	 * equal, higher or lower compared to the running order update ID
	 */
	string compareOrderUpdateId(int orderUpdateIdToCompare);

	/**
	 * @brief compares start of new base and end of current base
	 * 
	 * @param startOfNewBaseNodeId start of new base node ID
	 * @param startOfNewBaseSequenceId start of new base sequence ID
	 * @return true if start of new base equals end of current base
	 * @return false if start of new base is not equal to end of current base
	 */
	bool compareBase(string startOfNewBaseNodeId, int startOfNewBaseSequenceId);

	/**
	 * @brief decides whether or not the order is active
	 * 
	 * @return true 
	 * @return false 
	 */
	bool isActive();
};

class OrderDaemon: public Daemon
{
	private:
	vector<ActiveOrder> activeOrderList; /**List of  all active orders*/
	
	
	// Declare all ROS subscriber and publisher topics for internal communication
	ros::Subscriber orderCancelSub; 	/** ordinary order actions from order_daemon to action_daemon*/
	ros::Publisher orderActionPub; 		/** cancelled actions from action_daemon to order_daemon*/

	protected:

	public:
	/**
	 * Empty description.
	 * 
	 * @param nh	Empty parameter description
	 * @param daemonName	Empty parameter description
	 */
	OrderDaemon();

	/**
	 * Empty description.
	 * 
	 * @param nh	Empty parameter description
	 */
	void LinkPublishTopics(ros::NodeHandle *nh);

	/**
	 * Empty description.
	 * 
	 * @param nh	Empty parameter description
	 */
	void LinkSubscriptionTopics(ros::NodeHandle *nh);

	/**
	 * Empty description.
	 */
	void PublishOrderActions();

	void AddOrderToList(const vda5050_msgs::Order *incomingOrder);

	/**
	 * Empty description.
	 */
	void OrderCallback(const vda5050_msgs::Order::ConstPtr& msg);
    
    /**
	 * Empty description.
	 */
    void OrderCancelCallback(const std_msgs::String::ConstPtr& msg);

	/**
	 * Empty description.
	 */
    void ActionStateCallback(const vda5050_msgs::ActionState::ConstPtr& msg);

	/**
	 * Empty description.
	 */
    void AgvPositionCallback(const vda5050_msgs::AGVPosition::ConstPtr& msg);

	/**
	 * Empty description.
	 */
	string createPublishTopic();

	/**
	 * @brief loop actions
	 * 
	 * get order actions
	 * get instantAction topics
	 * calculate queue
	 * send queue to agv
	 * send order cancellations to order_daemon
	 * send action status to state_daemon
	 * 
	 */
	void UpdateOrders();

};

#endif

