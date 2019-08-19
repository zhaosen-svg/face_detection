//
#include "ros/ros.h"
#include "std_msgs/Float64.h"
#include <iostream>
#include "face_tracker_pkg/centroid.h"

//以下为串口通讯需要的头文件
#include <string>        
#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <math.h>
#include "serial/serial.h"

using std::string;
using std::exception;
using std::cout;
using std::cerr;
using std::endl;
using std::vector;

unsigned char data_terminal0=0x0d;  //“/r"字符
unsigned char data_terminal1=0x0a;  //“/n"字符
unsigned char speed_data[10]={0};   //要发给串口的数据

union floatData //union的作用为实现char数组和float之间的转换
{
    int d;
    unsigned char data[4];
}x_data,y_data;
//Callback of the topic /numbers
void face_callback(const face_tracker_pkg::centroid::ConstPtr& msg)
{
    string port("/dev/ttyUSB0");    //串口号
    unsigned long baud = 115200;    //串口波特率
    serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000)); //配置串口

	x_data.d = msg->x;
	y_data.d = msg->y;
	for(int i=0;i<4;i++)    //将速度存入数组中发送给串口
    {
        speed_data[i]=x_data.data[i];
        speed_data[i+4]=y_data.data[i];
    }
    //在写入串口的左右轮速度数据后加入”/r/n“
    speed_data[8]=data_terminal0;
    speed_data[9]=data_terminal1;
	ROS_INFO("Recieved X = [%d], Y = [%d]",x_data.d ,y_data.d);
    //写入数据到串口
    my_serial.write(speed_data,10);
	//Calling track face functi
}
int main(int argc, char **argv)
{
    string port("/dev/ttyUSB0");//小车串口号
    unsigned long baud = 115200;//小车串口波特率
    serial::Serial my_serial(port, baud, serial::Timeout::simpleTimeout(1000));//配置串
	ros::init(argc, argv,"face_tracker_controller");
	//Created a nodehandle object
	ros::NodeHandle node_obj;
	//Create a publisher object
	ros::Subscriber number_subscriber = node_obj.subscribe("/face_centroid",10,face_callback);	
	//Spinning the node
	ros::spin();
	return 0;
}


