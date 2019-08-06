# ball_chaser
A ROS package coded as part of project requirement in Robotics Nanodegree at Udacity. 

The project objective is to program a 2-wheel virtual mobile robot so that it will drive
toward a white ball autonomously.

It contains 2 ROS nodes: drive_bot.cpp and process_image.cpp. 

The drive_bot node provides a robot-driving service by accepting linear x and angular z
velocities from a client and subsequently publishing these to the robot's motor through
"geometry_msgs/Twist" message.  This message is defined in ball_chaser/srv/DriveToTarget.srv file.

The process_image node subscribes to "/camera/rgb/image_raw" topic published by an onboard RGB camera 
through "sensor_msgs/Image" messages.  These messages are camera images. Each image is analyzed for 
the presence of a white ball by processing image pixels and detecting possible white pixels. If
the white ball is found, then the process_image node's client requests robot's velocities from
the drive_bot node.
