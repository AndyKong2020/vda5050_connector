#ifndef ACTION_DAEMON_H
#define ACTION_DAEMON_H
#include <string>
#include <list>
#include <deque>
#include <memory>
#include <ros/ros.h>
#include "daemon.h"
#include "vda5050_msgs/InstantActions.h"
#include "vda5050_msgs/ActionState.h"
#include "vda5050_msgs/OrderActions.h"
#include "std_msgs/Bool.h"
#include <string>
#include <list>
#include <deque>
#include <memory>

using namespace std;

/**
 * @brief Stores information about a single action
 *
 */
class ActionElement
{
private:
    string orderId;
    /**< unique ID to identify the related order */
    string actionId;
    /**< unique ID to identify the action */
    string actionType;
    /**< identifies the function of the action */
    string actionDescription;
    /**< additional information on the action */
    vector<vda5050_msgs::ActionParameter> actionParameters;
    /**< array of actionParameter objects */

public:
    string state;        
    /**< blocking type of the action */
    string blockingType; 
    /**< blocking type of the action, Enum {NONE, SOFT, HARD} */
    bool sentToAgv;
    /**< true if the action was sent to the AGV after being triggered*/

    bool operator==(const ActionElement &s) const { return actionId == s.actionId; }
    bool operator!=(const ActionElement &s) const { return !operator==(s); }

    /**
     * @brief Construct a new Action Element object
     *
     * @param incomingAction New incoming action
     * @param incomingOrderId ID of the related order
     * @param state State of the new action
     */
    ActionElement(const vda5050_msgs::Action *incomingAction, string incomingOrderId, string state);

    /**
     * @brief returns true if given action ID is equal to action ID of action element
     *
     * @param actionId2comp ID to compare
     * @return true if IDs are equal
     * @return false if IDs are not equal
     */
    bool compareActionId(string actionId2comp);

    /**
     * @brief returns true if given order ID is equal to order ID of action element
     *
     * @param orderId2comp ID to compare
     * @return true if IDs are equal
     * @return false if IDs are not equal
     */
    bool compareOrderId(string orderId2comp);

    /**
     * @brief Get the Action ID object
     *
     * @return string Action ID
     */
    string getActionId() const;

    /**
     * @brief Get the Action type object
     *
     * @return string Action type
     */
    string getActionType() const;

    /**
     * @brief Returns an action message composed of an ActionElement
     *
     * @return vda5050_msgs::Action Action message to return
     */
    vda5050_msgs::Action packAction();
};

/** Struct to connect actions to cancel with their respective order ID */
struct orderToCancel
{
    string orderIdToCancel;
        /**< Order (order ID) which should be deleted */
    string iActionId;
        /**< Id of the instant action containing the cancel action */
    vector<weak_ptr<ActionElement>> actionsToCancel;
        /**< List of active actions to cancel */
    bool allActionsCancelledSent;
        /**< flag to send all actions cancelled message only once */
};

/**
 * Daemon for processing of VDA 5050 action messages. Currently, the action
 * daemon is only used for passing messages from an MQTT topic to a ROS topic.
 */
class ActionDaemon : public Daemon
{
private:
    vector<shared_ptr<ActionElement>> activeActionsList;
    /**< list of actions to track all active actions */
    vector<orderToCancel> orderCancellations;
    /**< list of all orders to cancel and their respective order id */

    /** Declare all ROS subscriber and publisher topics for internal communication */
    ros::Subscriber orderActionSub;        
    /**< ordinary order actions from order_daemon to action_daemon */
    ros::Subscriber orderTriggerSub;       
    /**< order daemon triggers actions */
    ros::Subscriber orderCancelSub;        
    /**< order daemon sends response to order cancel request */
    ros::Publisher actionStatesPub;        
    /**< states of actions from action_daemon to state_daemon */
    ros::Publisher orderCancelPub;         
    /**< cancelled actions from action_daemon to order_daemon */
    ros::Publisher allActionsCancelledPub; 
    /**< all actions of one order to cancel cancelled from action_daemon to order_daemon */

    bool isDriving;
    /**< True, if the vehicle is driving */

protected:
    deque<vda5050_msgs::Action> orderActionQueue;   
    /**< queue for keeping track of order actions */
    deque<vda5050_msgs::Action> instantActionQueue; 
    /**< queue for keeping track of instant actions */
    vector<string> ordersSucCancelled;              
    /**< list of all orders cancelled by order daemon */

public:
    /**
     * Empty description.
     *
     * @param nh	Empty parameter description
     * @param daemonName	Empty parameter description
     */
    ActionDaemon();

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
     * @brief callback for order actions topic from order_daemon
     *
     * This callback is called when a new message arrives at the /orderAction topic.
     * Actions are queued into a FIFO queue.
     * The first element of that queue is sent to the AGV for exection.
     *
     * @param msg message including the incoming order action
     */
    void OrderActionsCallback(const vda5050_msgs::OrderActions::ConstPtr &msg);

    /**
     * @brief callback for order trigger topic from order daemon
     *
     * This callback is called when a new message arrives at the /orderTrigger topic.
     * A trigger contains the ID of an action and triggers
     * adding the corresponding action to the order action queue.
     *
     * @param msg message including the action ID to trigger
     */
    void OrderTriggerCallback(const std_msgs::String &msg);

    /**
     * @brief callback to process response to order cancel request from order daemon
     *
     * This callback is called when a new message arrives at the /orderCancelResponse topic.
     * When a order cancel request was sent to the order daemon,
     * it sends the corresponding order id to cancel via the /orderCancelResponse topic
     * back to the action daemon to confirm the cancellation.
     *
     * @param msg message including the cancelled order ID
     */
    void OrderCancelCallback(const std_msgs::String &msg);

    /**
     * @brief callback for instant Actions topic from master controll
     *
     * This callback is called when a new message arrives at the /instantActions topic.
     * Actions are queued into a FIFO queue.
     * The first element of that queue is sent to the AGV for exection.
     *
     * @param msg message including the incoming instant action
     */
    void InstantActionsCallback(const vda5050_msgs::InstantActions::ConstPtr &msg);

    /**
     * @brief callback for agvActionState topic from AGV
     *
     * This callback is called when a new message arrives at the /agvActionState topic.
     * The message contains the state of a single action.
     * The state is used to fill the actionStates sent to the state daemon
     * and to track the current state for action blocking evaluation.
     *
     * @param msg message including the state of an action
     */
    void AgvActionStateCallback(const vda5050_msgs::ActionState::ConstPtr &msg);

    /**
     * @brief callback for driving topic from AGV
     *
     * This callback is called when a new message arrives at the /driving topic.
     * The message contains the driving state of the AGV.
     * “true”: indicates that the AGV is driving and/or rotating.
     * Other movements of the AGV (e.g. lift movements) are not included here.
     * “false”: indicates that the AGV is neither driving nor rotating.
     *
     * @param msg message including the driving state of the AGV
     */
    void DrivingCallback(const std_msgs::Bool::ConstPtr &msg);

    /**
     * @brief Adds a new action to the activeActionsList list.
     *
     * @param incomingAction Incoming action
     * @param orderId ID of the related order
     * @param state State of the incoming action
     */
    void AddActionToList(const vda5050_msgs::Action *incomingAction, string orderId, string state);

    /**
     * @brief checks if the AGV is driving and stops driving if so
     *
     * @return true if vehicle is not driving
     * @return false if vehicle is driving
     */
    bool checkDriving();

    /**
     * @brief Return all running actions
     *
     * @return vector<shared_ptr<ActionElement>> List of running actions
     */
    vector<shared_ptr<ActionElement>> GetRunningActions();

    /**
     * @brief Get all running or paused actions
     *
     * @return vector<shared_ptr<ActionElement>> List of running or paused actions
     */
    vector<shared_ptr<ActionElement>> GetRunningPausedActions();

    /**
     * @brief Get all actions from activeActionsList which should be cancelled
     *
     * @param orderIdToCancel ID of the order to cancel
     * @return vector<shared_ptr<ActionElement>>  List of pointers to actions to cancel
     */
    vector<shared_ptr<ActionElement>> GetActionsToCancel(string orderIdToCancel);

    /**
     * @brief Finds and returns the action with the requested ID
     *
     * Finds the action in the activeActionsList list which has the requested ID.
     * The corresponding action is returned.
     *
     * @param actionId ID of the action to find within the active Actions
     * @return shared_ptr<ActionElement> Shared pointer to found action element
     */

    shared_ptr<ActionElement> findAction(string actionId);

    /**
     * @brief processes actions based on their type
     *
     * The UpdateActions() method represents the main event loop.
     * Based on the order and instan action queues, the method processes incoming actions
     * and pauses driving state and pauses/resumes other actions.
     *
     */
    void UpdateActions();
};

#endif