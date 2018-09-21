#include "game/InputController.h"

extern GLFWwindow* window;
extern Camera camera;
extern glm::vec3 cursorToViewportDirection;
extern Options options;
extern int scr_width;
extern int scr_height;
extern float aspect_ratio;
bool keysPressed[GLFW_KEY_LAST];
bool mouseKeysPressed[GLFW_MOUSE_BUTTON_LAST];
bool firstMouseInput = true;
float lastX, lastY;
double cursorScreenX = 0.0;
double cursorScreenY = 0.0;

void InputController::processKeyboard()
{
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GL_TRUE);

  //set FPS mode for the camera
  if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F1])
        {
          camera.switchFPSmode();
          keysPressed[GLFW_KEY_F1] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F1) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F1] = false;

  //render shadow on tree leaves
  if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F2])
        {
          options.switchOpt(RENDER_SHADOW_ON_TREES);
          keysPressed[GLFW_KEY_F2] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F2] = false;

  //animate water
  if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F3])
        {
          options.switchOpt(ANIMATE_WATER);
          keysPressed[GLFW_KEY_F3] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F3] = false;

  //render trees
  if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F4])
        {
          options.switchOpt(RENDER_TREE_MODELS);
          keysPressed[GLFW_KEY_F4] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F4) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F4] = false;

  //render debug text
  if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F5])
        {
          options.switchOpt(RENDER_DEBUG_TEXT);
          keysPressed[GLFW_KEY_F5] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F5) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F5] = false;

  //show buildable
  if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F6])
        {
          options.switchOpt(SHOW_BUILDABLE);
          keysPressed[GLFW_KEY_F6] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F6) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F6] = false;

  //models shading strategy
  if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F7])
        {
          options.switchOpt(MODELS_PHONG_SHADING);
          keysPressed[GLFW_KEY_F7] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F7) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F7] = false;

  //recreate terrain
  if (glfwGetKey(window, GLFW_KEY_F9) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F9])
        {
          options.set(RECREATE_TERRAIN_REQUEST, true);
          options.set(CREATE_SHADOW_MAP_REQUEST, true);
          keysPressed[GLFW_KEY_F9] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F9) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F9] = false;

  //save request
  if (glfwGetKey(window, GLFW_KEY_F10) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F10])
        {
          options.set(SAVE_REQUEST, true);
          keysPressed[GLFW_KEY_F10] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F10) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F10] = false;

  //load request
  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F11])
        {
          options.set(LOAD_REQUEST, true);
          options.set(CREATE_SHADOW_MAP_REQUEST, true);
          keysPressed[GLFW_KEY_F11] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F11] = false;

  //model rendering optimization
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_R])
        {
          options.switchOpt(MODELS_FC);
          options.set(CREATE_SHADOW_MAP_REQUEST, true);
          keysPressed[GLFW_KEY_R] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_R) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_R] = false;

  //hills rendering optimization
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_T])
        {
          options.switchOpt(HILLS_FC);
          keysPressed[GLFW_KEY_T] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_T] = false;

  //water rendering optimization
  if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_Y])
        {
          options.switchOpt(WATER_FC);
          keysPressed[GLFW_KEY_Y] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_Y] = false;

  //Multisampling
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_M])
        {
          options.switchOpt(MULTISAMPLE_ENABLE);
          if (options.get(MULTISAMPLE_ENABLE))
            glEnable(GL_MULTISAMPLE);
          else
            glDisable(GL_MULTISAMPLE);
          keysPressed[GLFW_KEY_M] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_M) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_M] = false;

  //Shadows
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_Q])
        {
          options.switchOpt(SHADOW_ENABLE);
          keysPressed[GLFW_KEY_Q] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_Q] = false;

  //Render flat terrain
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_F])
        {
          options.switchOpt(RENDER_FLAT_TERRAIN);
          keysPressed[GLFW_KEY_F] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_F] = false;

  //Render water
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_G])
        {
          options.switchOpt(RENDER_WATER);
          keysPressed[GLFW_KEY_G] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_G) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_G] = false;

  //models flat blending
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_B])
        {
          options.switchOpt(MODELS_FLAT_BLENDING);
          keysPressed[GLFW_KEY_B] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_B] = false;

  //set polygon draw mode
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
    {
      if (!keysPressed[GLFW_KEY_L])
        {
          options.switchOpt(POLYGON_LINE);
          keysPressed[GLFW_KEY_L] = true;
        }
    }
  if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE)
    keysPressed[GLFW_KEY_L] = false;
}

void InputController::processKeyboardCamera(float delta, std::vector<std::vector<float> > &hillsMap)
{
  /*
   * I've just made an assumption that player won't be trying
   * to move right and left (or up and down, forward and backward) simultaneously
   * that's why here are pairs of "if-elif" instead of plain "if-if",
   * in this case the CPU would not try to process both movements
   * if the 1st direction is already detected(I guess...)
   */
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    camera.processKeyboardInput(delta, FORWARD, hillsMap);
  else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    camera.processKeyboardInput(delta, BACKWARD, hillsMap);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    camera.processKeyboardInput(delta, LEFT, hillsMap);
  else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    camera.processKeyboardInput(delta, RIGHT, hillsMap);
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    camera.processKeyboardInput(delta, DOWN, hillsMap);
  else if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    camera.processKeyboardInput(delta, UP, hillsMap);
}

void InputController::cursorCallback(GLFWwindow *, double x, double y)
{
  if (options.get(SHOW_CURSOR))
    {
      lastX = x;
      lastY = y;
      glfwGetCursorPos(window, &cursorScreenX, &cursorScreenY);
      glm::vec3 view = camera.getDirection();
      view = glm::normalize(view);
      glm::vec3 h = camera.getRight();
      h = glm::normalize(h);
      glm::vec3 v = camera.getUp();
      v = glm::normalize(v);
      float fovRad = glm::radians(camera.getZoom());
      float vLength = std::tan(fovRad / 2) * NEAR_PLANE;
      float hLength = vLength * aspect_ratio;
      h *= hLength;
      v *= vLength;
      cursorScreenX -= scr_width / 2;
      cursorScreenY -= scr_height / 2;
      cursorScreenX /= (scr_width / 2);
      cursorScreenY /= (scr_height / 2);
      glm::vec3 newPos = camera.getPosition() + view * NEAR_PLANE + h * (float)cursorScreenX - v * (float)cursorScreenY;
      glm::vec3 newDir = newPos - camera.getPosition();
      cursorToViewportDirection = newDir;
      return;
    }
  if (firstMouseInput)
    {
      lastX = x;
      lastY = y;
      firstMouseInput = false;
    }
  float dx = lastX - x;
  float dy = y - lastY;
  lastX = x;
  lastY = y;
  camera.processMouseCursor(dx, dy);
}

void InputController::cursorClickCallback(GLFWwindow *window, int button, int action, int)
{
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
      if (!mouseKeysPressed[GLFW_MOUSE_BUTTON_RIGHT])
        {
          options.switchOpt(SHOW_CURSOR);
          glfwSetInputMode(window, GLFW_CURSOR, options.get(SHOW_CURSOR) ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
          glfwSetCursorPos(window, scr_width / 2.0f, scr_height / 2.0f);
          lastX = scr_width / 2.0f;
          lastY = scr_height / 2.0f;
          mouseKeysPressed[GLFW_MOUSE_BUTTON_RIGHT] = true;
        }
    }
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    mouseKeysPressed[GLFW_MOUSE_BUTTON_RIGHT] = false;
}

void InputController::updateCursorMappingCoordinates(Camera &camera,
                                        BaseMapGenerator *baseMapGenerator,
                                        HillsMapGenerator *hillMapGenerator,
                                        BuildableMapGenerator *buildableMapGenerator)
{
  if (options.get(SHOW_CURSOR) && cursorToViewportDirection.y < 0.0f)
    {
      float ratio = camera.getPosition().y / (-cursorToViewportDirection.y);
      bool cursorOutOfMap = false;
      cursorOnMapX = glm::clamp((cursorToViewportDirection.x * ratio) + camera.getPosition().x, -(float)HALF_TILES_WIDTH, (float)HALF_TILES_WIDTH);
      cursorOnMapZ = glm::clamp((cursorToViewportDirection.z * ratio) + camera.getPosition().z, -(float)HALF_TILES_HEIGHT, (float)HALF_TILES_HEIGHT);
      if (cursorOnMapX == -HALF_TILES_WIDTH || cursorOnMapX == HALF_TILES_WIDTH ||
          cursorOnMapZ == -HALF_TILES_HEIGHT || cursorOnMapZ == HALF_TILES_HEIGHT)
        cursorOutOfMap = true;
      if (cursorOutOfMap)
        {
          cursorTileName = "out of map";
          return;
        }
      cursorOnMapCoordX = (int)(TILES_WIDTH + cursorOnMapX) - HALF_TILES_WIDTH;
      cursorOnMapCoordX = glm::clamp(cursorOnMapCoordX, 1, TILES_WIDTH - 2);
      cursorOnMapCoordZ = (int)(TILES_HEIGHT + cursorOnMapZ) - HALF_TILES_HEIGHT + 1;
      cursorOnMapCoordZ = glm::clamp(cursorOnMapCoordZ, 1, TILES_HEIGHT - 1);
      if (buildableMapGenerator->getMap()[cursorOnMapCoordZ][cursorOnMapCoordX] != 0)
        cursorTileName = "Flat";
      else if (hillMapGenerator->getMap()[cursorOnMapCoordZ][cursorOnMapCoordX] != 0 ||
               hillMapGenerator->getMap()[cursorOnMapCoordZ-1][cursorOnMapCoordX] != 0 ||
               hillMapGenerator->getMap()[cursorOnMapCoordZ-1][cursorOnMapCoordX+1] != 0 ||
               hillMapGenerator->getMap()[cursorOnMapCoordZ][cursorOnMapCoordX+1] != 0)
        cursorTileName = "Hills";
      else
        {
          if (baseMapGenerator->getMap()[cursorOnMapCoordZ][cursorOnMapCoordX] == DENY_TILE_RENDER_VALUE ||
              baseMapGenerator->getMap()[cursorOnMapCoordZ-1][cursorOnMapCoordX] == DENY_TILE_RENDER_VALUE ||
              baseMapGenerator->getMap()[cursorOnMapCoordZ-1][cursorOnMapCoordX+1] == DENY_TILE_RENDER_VALUE ||
              baseMapGenerator->getMap()[cursorOnMapCoordZ][cursorOnMapCoordX+1] == DENY_TILE_RENDER_VALUE)
            cursorTileName = "Water";
          else
            cursorTileName = "Shore";
        }
    }
  else
    cursorTileName = "out of map";
}

int InputController::getCursorMapX() const
{
  return cursorOnMapCoordX;
}

int InputController::getCursorMapZ() const
{
  return cursorOnMapCoordZ;
}

const std::string &InputController::getCursorTileName() const
{
  return cursorTileName;
}
