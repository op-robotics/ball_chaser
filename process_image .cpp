#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{

    // Display requested velocities

    ROS_INFO_STREAM("From process_image: moving the bot");

    ROS_INFO("Send DriveToTarget request: linear_x = %.2f ; angular_z = %.2f", float(lin_x), float (ang_z));

    // TODO: Request a service and pass the velocities to it to drive the robot
    ball_chaser::DriveToTarget move_bot;
    move_bot.request.linear_x = lin_x;
    move_bot.request.angular_z = ang_z;
    
    // Inform if client call fails
    
    if (!client.call(move_bot))
        ROS_ERROR("Failed to call service /ball_chaser/command_robot");
        
}/*=== end drive_robot function ===*/

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{
    
    //--- initialize parameters for cam image analysis:

    const int left_bound = 6*img.width/15; //--- left boundary of forward moving window   
    const int right_bound = 9*img.width/15; //--- right boundary
    const int img_area = img.height * img.width; //---total amount of pixels in the image
    const int pix_sum_brake = 101000; //--- amount of white pixel to request a stop  
    const int w_byte_sum = 3*255;     
    int white_pix_pos = img.width*3;   //--- this variable holds position of
       
    int white_pix_sum = 0; //--- white pixel counter
    int three_byte_sum;
    int pix_count = 0;
    int white_bytes = 0;
    bool white_pix_found = false;//--- use this variable not to record subsequent white pixel pos
        

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function 
    // and pass velocities to it
    // Request a stop when there's no white ball seen by the camera

    //##------------ loop through every pixel, record first white pixel's position
    //##------------ calculate amount of all white pixels and use it for white ball
    //##------------ proximity detection 
 
    for (int pix = 0; pix < img_area; ++pix) {
         
         
         three_byte_sum = 0;   // reset the sum of 3 bytes to zero
 
         for (int b = 0; b < 3; ++b){ // inspect a pixel or series of 3 bytes for white
                                      // color RGB values    

               three_byte_sum += img.data[3*pix + b];
                    
             
         }/*=== end for int b loop === */
         
         if ( w_byte_sum == three_byte_sum ) {  // white pixel detected

               white_pix_pos = pix%img.width; 
               pix_count = pix; 
               break;       // count number of white pixels

         }//=== end if statement 
          
        
     }/*=== end for int pix loop */

     for (int px = pix_count; px < img_area; ++px){
     	
       three_byte_sum = 0;

       for (int b = 0; b < 3; ++b){

          three_byte_sum += img.data[3*px+b]; 

       }//--- end for int b loop
       
          if (three_byte_sum == w_byte_sum){

             ++white_pix_sum;

          }//--- end if statement

     }//--- end for int px loop

     ROS_INFO("-----White pixel position: %d", white_pix_pos);  
     ROS_INFO("-----White pixel sum: %d", white_pix_sum); 
    
     //##----------- call **drive_robot** function to move the bot
     //##----------- maneuver the bot based on the ball's position and 
     //##----------- how close the bot is to the ball:
     
     if (white_pix_pos > img.width || white_pix_sum > pix_sum_brake){
      
        drive_robot(0.0, 0.0); //--- stop the bot if there's no ball or the ball is 
                               //--- in front of the bot  

     }//--- if
           
     else { //--- drive the bot forward:
     
        if (white_pix_pos > left_bound && white_pix_pos < right_bound){
            
            if (white_pix_sum > 12400){ //--- slow the bot down as it is approaching the ball
           
                 drive_robot(0.07, 0.0);      

            }

            else if (white_pix_sum > 4000){
            
                drive_robot(0.16, 0.0); //--- slower forward linear velocity

            }
          
            else {
              
                drive_robot(0.35, 0.0); //--- default x-axis velocity
            }
                       
        }//---if

        else if (white_pix_pos >= right_bound){

           drive_robot(0.012, -0.035); //--- turn the bot clockwise
           
        }
        
        else {

           drive_robot(0.012, 0.035); //--- turn the bot counter-clockwise
           
        }
     
     }//--- else   
            
}//=== end process_image_callback function ===

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle pr_im_n_h;

    // Define a client service capable of requesting services from command_robot
    client = pr_im_n_h.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");
    
    
    // Subscribe to /camera/rgb/image_raw topic to read the image data 
    // inside the process_image_callback function
    ros::Subscriber cam_sub = pr_im_n_h.subscribe("/camera/rgb/image_raw", 10, process_image_callback);
      

    // Handle ROS communication events
    ros::spin();

    return 0;
}

