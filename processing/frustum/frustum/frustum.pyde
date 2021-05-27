import math
from maths import *

#                 UP    DOWN   LEFT  RIGHT  SPACE    0      1      2
pressed_keys = [False, False, False, False, False, False, False, False]

def drawCircle(center, radius):
  ellipseMode(CENTER)
  ellipseMode(RADIUS)
  ellipse(center.x, center.y, radius, radius)

class Camera:
    
    def __init__(self, pos, dir, fov):
        self.pos = pos;
        self.dir = dir.normalized()
        self.fov = fov * PI / 180.0
    
    def draw(self):
        radius = 5.0
        # noStroke()
        # fill(CAMERA_COLOR)
        # drawCircle(self.pos, radius/2.0)
        stroke(color(255, 0, 0))
        strokeWeight(2)
        noFill()
        drawCircle(self.pos, radius)
        strokeWeight(1)
    
        left_dir = RotateMatrix(self.fov/2) * self.dir
        
        strokeWeight(1)
        line(self.pos.x, self.pos.y, self.pos.x + left_dir.x * 2500, self.pos.y + left_dir.y * 2500)
        
        right_dir = RotateMatrix(-self.fov/2) * self.dir
        
        strokeWeight(1)
        line(self.pos.x, self.pos.y, self.pos.x + right_dir.x * 2500, self.pos.y + right_dir.y * 2500)
        
    def leftDir(self):
        left_dir = RotateMatrix(self.fov/2) * self.dir
        return left_dir
    
    def rightDir(self):
        right_dir = RotateMatrix(-self.fov/2) * self.dir
        return right_dir

class GRM:
    
    def __init__(self, pos, dir, vertical=None):
        
        self.pos = pos
        self.dir_normalized = dir.normalized()
        
        if vertical is None:
            self.vertical = abs(self.dir_normalized.x) > cos(PI/4)
        else:
            self.vertical = vertical
        
        if self.vertical:
            self.dir = self.dir_normalized / abs(self.dir_normalized.x)
        else:
            self.dir = self.dir_normalized / abs(self.dir_normalized.y)
            
        self.tan_alpha = (self.dir_normalized.y / self.dir_normalized.x) if self.vertical else (self.dir_normalized.x / self.dir_normalized.y)
        
    def firstStep(self):
        if self.vertical:
            right = self.dir_normalized.x > 0
            target = ceil(self.pos.x) if right else floor(self.pos.x)
            dx = target - self.pos.x
            dy = dx * self.tan_alpha
            self.pos = self.pos + Vector2(dx, dy)
            self.pos.x = round(self.pos.x)
        else:
            up = self.dir_normalized.y > 0
            target = ceil(self.pos.y) if up else floor(self.pos.y)
            dy = target - self.pos.y
            dx = dy * self.tan_alpha
            self.pos = self.pos + Vector2(dx, dy)
            self.pos.y = round(self.pos.y)
            
    def stepOne(self):
        self.pos = self.pos + self.dir
    
    def getId(self):
        res = Vector2(0, 0)
        
        if self.vertical:
            res.x = int(self.pos.x + 0.5)
            res.y = int(self.pos.y)
            right = self.dir_normalized.x > 0
            if right:
                res.x = res.x - 1;
        else:
            res.y = int(self.pos.y + 0.5)
            res.x = int(self.pos.x)
            up = self.dir_normalized.y > 0
            if up:
                res.y = res.y - 1;
        return res


global cam

def setup():
    global cam
    cam = Camera(Vector2(500, 500), Vector2(1, 0), 70)
    
    size(1000, 1000)
    
    
def drawSquare(id):
    base = id * 100
    rect(base.x, base.y, 100, 100)

def draw():
    global cam
    background(200)
    
    move = Vector2(0, 0)
    if pressed_keys[0]:
        move.y = move.y - 1.0
    if pressed_keys[1]:
        move.y = move.y + 1.0
    if pressed_keys[2]:
        move.x = move.x - 1.0
    if pressed_keys[3]:
        move.x = move.x + 1.0
    move = move * 5
    cam.pos = cam.pos + move


    if not pressed_keys[4]: # Hold the mouse pos
        mouse_pos = Vector2(mouseX, mouseY)
        cam.dir = (mouse_pos - cam.pos)
        if cam.dir.norm2() != 0:
            cam.dir = cam.dir.normalized()
    
    
    
    
    
    strokeWeight(1)
    stroke(0);
    for i in range(11):
        line(i*100, 0, i*100, 1000)
        line(0, i*100, 1000, i*100)
    
    
    cd = cam.dir
    ld = cam.leftDir()
    rd = cam.rightDir()
    
    grid_pos = cam.pos / Vector2(100, 100)
    
    cg = GRM(grid_pos, cd)
    lg = GRM(grid_pos, ld, cg.vertical)
    rg = GRM(grid_pos, rd, cg.vertical)
    
    lg.firstStep()
    rg.firstStep()
    
    strokeWeight(1)
    stroke(0);
    
    
    fill(0, 200, 0)
    drawSquare(lg.getId())
    fill(0, 0, 200)
    drawSquare(rg.getId())
    
    
    cam.draw()
    
def keyPressed():
    global pressed_keys
    if key == 'z':
        pressed_keys[0] = True
    elif key == 's':
        pressed_keys[1] = True
    elif key == 'q':
        pressed_keys[2] = True
    elif key == 'd':
        pressed_keys[3] = True
    elif key == ' ':
        pressed_keys[4] = True
                        
def keyReleased():
    global pressed_keys
    if key == 'z':
        pressed_keys[0] = False
    elif key == 's':
        pressed_keys[1] = False
    elif key == 'q':
        pressed_keys[2] = False
    elif key == 'd':
        pressed_keys[3] = False
    elif key == ' ':
        pressed_keys[4] = False
    
    
    
