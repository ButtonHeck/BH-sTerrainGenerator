#include "input/KeyboardManager.h"

extern float debug_sunSpeed;

KeyboardManager::KeyboardManager(GLFWwindow *window, Camera &camera, Options &options, TheSunFacade &sun)
  :
    window(window),
    camera(camera),
    options(options),
    sun(sun)
{}

void KeyboardManager::processInput()
{
  BENCHMARK("KeyboardManager: processInput", true);
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  processKey(GLFW_KEY_F1, [&](){camera.switchFPSmode();});
  processKey(GLFW_KEY_F3, OPT_ANIMATE_WATER);
  processKey(GLFW_KEY_F4, OPT_DRAW_TREES);
  processKey(GLFW_KEY_F5, OPT_DRAW_DEBUG_TEXT);
  processKey(GLFW_KEY_F6, OPT_DRAW_BUILDABLE);
  processKey(GLFW_KEY_F7, OPT_MODELS_PHONG_SHADING);
  processKey(GLFW_KEY_F9, [&]()
  {
      options[OPT_RECREATE_TERRAIN_REQUEST] = true;
    });
  processKey(GLFW_KEY_F10, [&](){options[OPT_SAVE_REQUEST] = true;});
  processKey(GLFW_KEY_F11, [&]()
  {
      options[OPT_LOAD_REQUEST] = true;
    });
  processKey(GLFW_KEY_T, OPT_HILLS_CULLING);
  processKey(GLFW_KEY_Y, OPT_WATER_CULLING);
  processKey(GLFW_KEY_M, [&]()
  {
      options.toggle(OPT_USE_MULTISAMPLING);
      if (options[OPT_USE_MULTISAMPLING])
        glEnable(GL_MULTISAMPLE);
      else
        glDisable(GL_MULTISAMPLE);
    });
  processKey(GLFW_KEY_Q, OPT_USE_SHADOWS);
  processKey(GLFW_KEY_F, OPT_DRAW_LAND);
  processKey(GLFW_KEY_G, OPT_DRAW_WATER);
  processKey(GLFW_KEY_B, OPT_MODELS_LAND_BLENDING);
  processKey(GLFW_KEY_L, OPT_POLYGON_LINE);
  processKey(GLFW_KEY_P, OPT_DEBUG_RENDER);

  processKey(GLFW_KEY_KP_8, [&]()
  {
      camera.setYaw(-90.0f);
    });
  processKey(GLFW_KEY_KP_6, [&]()
  {
      camera.setYaw(0.0f);
    });
  processKey(GLFW_KEY_KP_2, [&]()
  {
      camera.setYaw(90.0f);
    });
  processKey(GLFW_KEY_KP_4, [&]()
  {
      camera.setYaw(180.0f);
    });
  processKey(GLFW_KEY_KP_5, [&]()
  {
      camera.setPitch(-90.0f);
    });
  processKey(GLFW_KEY_E, [&]()
  {
      camera.switchAcceleration();
    });
  processKey(GLFW_KEY_UP, []()
  {
      debug_sunSpeed += 0.5f;
    });
  processKey(GLFW_KEY_DOWN, []()
  {
      debug_sunSpeed -= 0.5f;
    });
  processKey(GLFW_KEY_KP_0, []()
  {
      debug_sunSpeed = 0.0f;
    });

  processKey(GLFW_KEY_EQUAL, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(0.0f);
    });
  processKey(GLFW_KEY_1, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 1);
    });
  processKey(GLFW_KEY_2, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 2);
    });
  processKey(GLFW_KEY_3, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 3);
    });
  processKey(GLFW_KEY_4, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 4);
    });
  processKey(GLFW_KEY_5, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 5);
    });
  processKey(GLFW_KEY_6, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 6);
    });
  processKey(GLFW_KEY_7, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 7);
    });
  processKey(GLFW_KEY_8, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 8);
    });
  processKey(GLFW_KEY_9, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 9);
    });
  processKey(GLFW_KEY_0, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 10);
    });
  processKey(GLFW_KEY_MINUS, [&]()
  {
      debug_sunSpeed = 0.0f;
      sun.setPos(360.0f / 12 * 11);
    });

  //process camera
  camera.disableMoveAcceleration();
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_S) != GLFW_PRESS)
    camera.updateAccelerations(FORWARD);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_W) != GLFW_PRESS)
    camera.updateAccelerations(BACKWARD);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_D) != GLFW_PRESS)
    camera.updateAccelerations(LEFT);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_A) != GLFW_PRESS)
    camera.updateAccelerations(RIGHT);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS)
    camera.updateAccelerations(DOWN);
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS &&
      glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
    camera.updateAccelerations(UP);
}

void KeyboardManager::processKey(int keyCode, OPTION option)
{
  if (glfwGetKey(window, keyCode) == GLFW_PRESS)
    {
      if (!keysPressed[keyCode])
        {
          options.toggle(option);
          keysPressed[keyCode] = true;
        }
    }
  if (glfwGetKey(window, keyCode) == GLFW_RELEASE)
    keysPressed[keyCode] = false;
}

void KeyboardManager::processKey(int keyCode, std::function<void ()> function)
{
  if (glfwGetKey(window, keyCode) == GLFW_PRESS)
    {
      if (!keysPressed[keyCode])
        {
          function();
          keysPressed[keyCode] = true;
        }
    }
  if (glfwGetKey(window, keyCode) == GLFW_RELEASE)
    keysPressed[keyCode] = false;
}
