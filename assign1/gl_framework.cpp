#include "gl_framework.hpp"
#include <fstream>
#include <sstream>

namespace cse
{
  //! Initialize GL State
  void initGL(void)
  {
    //Set framebuffer clear color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    //Set depth buffer furthest depth
    glClearDepth(1.0);
    //Set depth test to less-than
    glDepthFunc(GL_LESS);
    //Enable depth testing
    glEnable(GL_DEPTH_TEST);
    //Enable Gourard shading
    glShadeModel(GL_SMOOTH);
  }

  //!GLFW Error Callback
  void error_callback(int error, const char* description)
  {
    std::cerr<<description<<std::endl;
  }

  //!GLFW framebuffer resize callback
  void framebuffer_size_callback(GLFWwindow* window, int width, int height)
  {
    //!Resize the viewport to fit the window size - draw to entire window
    glViewport(0, 0, width, height);
  }

  //!GLFW keyboard callback
  void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
  {
    state *st;
    st = (state*)glfwGetWindowUserPointer(window);
    //!Close the window if the ESC key was pressed
    if(action == GLFW_PRESS) {
      if (key == GLFW_KEY_ESCAPE){
        // Close window
        glfwSetWindowShouldClose(window, GL_TRUE);
      }
      else if(key == GLFW_KEY_M){
        // Change to Modelling mode
        st->mode = 'M';
        std::cout << "Modelling Mode" << std::endl;
      }
      else if(key == GLFW_KEY_I){
        // Change to Inspection mode
        st->mode = 'I';
        std::cout << "Inspection Mode" << std::endl;
      }
      else if(key == GLFW_KEY_K) {
        // Save to file
        std::ofstream file;
        std::cout << "Enter the file name for writing: ";
        std::string filename;
        std::cin >> filename;
        const char* tmp = filename.c_str();
        file.open(tmp);
        for(int i=0;i<st->pts.size();i=i+3){
          file << st->pts[i] << " " << st->pts[i+1] << " " << st->pts[i+2] << " " << st->color[i] << " " << st->color[i+1] << " " << st->color[i+2] <<std::endl;
        }
        file.close();
      }
      else if(key == GLFW_KEY_L) {
        // Load from file
        std::ifstream source;
        std::cout << "Enter the file name for reading: ";
        std::string filename;
        std::cin >> filename;
        const char* tmp = filename.c_str();
        source.open(tmp);

        for(std::string line; std::getline(source, line);) {
          std::istringstream in(line);
          float x,y,z;
          in >> x >> y >> z;
          st->pts.push_back(x);
          st->pts.push_back(y);
          st->pts.push_back(z);
          float r,g,b;
          in >> r >> g >> b;
          st->color.push_back(r);
          st->color.push_back(g);
          st->color.push_back(b);

          glBufferData (GL_ARRAY_BUFFER, st->pts.size() * sizeof (float) + st->color.size() * sizeof (float), NULL, GL_STATIC_DRAW);
          glBufferSubData( GL_ARRAY_BUFFER, 0, st->pts.size() * sizeof (float), &st->pts[0] );
          glBufferSubData( GL_ARRAY_BUFFER, st->pts.size() * sizeof (float),st->color.size() * sizeof (float), &st->color[0] );
        } 
      }
      else if(key == GLFW_KEY_C) {
        // Clear everything
        while(st->pts.size() > 0){
          st->pts.pop_back();
          st->color.pop_back();
        }
      }
      else if(key == GLFW_KEY_R) {
        // recenter origin to centroid
      }
      else if(st->mode == 'I') {
        switch(key) {
          // Translation along positive Y, X, Z respectively
          case GLFW_KEY_W:  st->ytrans ++;
                            break;
          case GLFW_KEY_S:  st->ytrans --;
                            break;
          case GLFW_KEY_A:  st->xtrans ++;
                            break;
          case GLFW_KEY_D:  st->xtrans --;
                            break;
          case GLFW_KEY_Z:  st->ztrans ++;
                            break;
          case GLFW_KEY_X:  st->ztrans --;
                            break;
          // Rotation about X, Y, Z respectively
          case GLFW_KEY_UP: st->xtheta ++;
                            break;
          case GLFW_KEY_DOWN: st->xtheta --;
                            break;
          case GLFW_KEY_LEFT: st->ytheta ++;
                            break;
          case GLFW_KEY_RIGHT: st->ytheta --;
                            break;
          case GLFW_KEY_PAGE_UP: st->ztheta ++;
                            break;
          case GLFW_KEY_PAGE_DOWN: st->ztheta --;
                            break;
          // Default case
          default: std::cout << "Key not recognised in this mode." << std::endl;
        }
      }
    }
  }

  void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
  {
    state* st = (state*) glfwGetWindowUserPointer(window);

    // Shift + left click (Remove point)
    if(mods == GLFW_MOD_SHIFT && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      if(st->mode == 'I')
        return;
      if(st->pts.size() > 0) {
        for(int i = 0; i < 3; i++){
          st->pts.pop_back();
          st->color.pop_back();
        }
      }
    }
    // Left click(Add point)
    else if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
      double xpos, ypos;
      //getting cursor position
      glfwGetCursorPos(window, &xpos, &ypos);
      if(st->mode == 'I')
        return;

      // Scale click location to vertex co-ordinates
      int height = 0, width = 0;
      glfwGetWindowSize(window, &width, &height);
      st->pts.push_back(xpos*2/width - 1);
      st->pts.push_back(-(ypos*2/height - 1));
      st->pts.push_back(0.0f);
      st->color.push_back(1.0f);
      st->color.push_back(0.0f);
      st->color.push_back(0.0f);
      // std::cout<<st->color[st->color.size()-1]<<" "<<st->color[st->color.size()-2]<<" "<<st->color[st->color.size()-3]<<std::endl;
      glBufferData (GL_ARRAY_BUFFER, st->pts.size() * sizeof (float) + st->color.size() * sizeof (float), NULL, GL_STATIC_DRAW);
      glBufferSubData( GL_ARRAY_BUFFER, 0, st->pts.size() * sizeof (float), &st->pts[0] );
      glBufferSubData( GL_ARRAY_BUFFER, st->pts.size() * sizeof (float),st->color.size() * sizeof (float), &st->color[0] );
      // std::cout << "DEBUG: " << xpos << " " << ypos << std::endl;
    }
  }
};
