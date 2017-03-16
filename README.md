# Zbuffer

### 实验要求
实现Zbuffer扫描线算法，要求可以导入obj场景，并可以描绘1000+面片

### 实验环境
win7，64bit
C++， Visual Studio 2015
想要下载我代码的小伙伴不用担心，只要是使用VS2015+且为64位系统，就可以直接在VS中选择x64编译生成可执行文件，我已将OpenGL的header,lib和dll库都配好。不过强烈安利使用[该方法(戳这里)](http://www.cs.uregina.ca/Links/class-info/315/WWW/Lab1/GLUT/windows.html)给自己的电脑配环境，这样就不用以后每写一个程序就配一次环境了~

### 使用方法
该程序没有什么酷炫的界面也没有特别的功能，就老老实实地实现了Zbuffer算法。在使用时把想要显示的.obj文件放入model文件夹下，当提示输入文件名称时将相应的文件名输入。如果没有输入（如空格，回车），则打开默认的文件 obj_camper_sample.obj。我在当前model文件夹下有如下三个文件。
model/　　
&#8195;cubex6.obj　　
&#8195;obj_camper_sample.obj　　
&#8195;obj_ford_sample.obj　　
One more thing，本程序中使用的.obj格式较为简易：
以v开头的是顶点，后面的数字标识了其坐标。
以f开头的是面，后面的数字标识了这个面是由哪几个点构成的。e.g.，f 1 2 3代表是第1-3个点构成的。
[点击这里查看更标准的.obj文件格式](http://www.opengl-tutorial.org/beginners-tutorials/tutorial-7-model-loading/)

### 算法实现
前文已承认仅仅是最普通的Zbuffer，那你还有什么其他好期待的嘛\~好好研究一下经典课件上的Zbuffer算法，我保证你就能看懂代码了。什么？你说我写的太烂太难懂？那，怪我咯\~\~\~

不过我还是觉得有三件事还是要提及一下。　　
**第一**，无论是我们构建的分类边表，分类多边形表，活性边表还是活性多边形表，读取ymax后存储至相应分类中，ymax取整数更易理解。且我们在屏幕上绘画时其实也不过是整数像素点为单位级，所以我们获取屏幕坐标进行上面四种表的创建更为方便。　　
上面说的有（不）点（像）抽（人）象（话），举个栗子。以下图中的分类边表为例（图来自Professor Feng的课件，侵删）。　　
![Classified edge Table](/image/classified_edge_table.png)　　
每条分类边将其放入ymax中分类边表中。如果本来这两个三角形其实很小，最大和最小的y在[-1,1]这个区间，那么大多数点都是小数，那么分类（链表前的数字，竖起来的一列）怎么做呢？也是变成小数方式？那怎么取这个区间才合适呢？把[-1,1]分成100份，还是分成356份比较好？　　
那么我们将所有的点，通过坐标系转换得到其在屏幕上的坐标，很幸运的是我们得到的屏幕坐标都是整数（小数的话屏幕也没法画啊，是不是）。然后再按照步骤进行四种表的构建。　　
 对每个点的处理代码：
```
//  get three cooridnate parameters
GLint viewport[4];
GLdouble modelview[16];
GLdouble projection[16];
glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
glGetDoublev(GL_PROJECTION_MATRIX, projection);
glGetIntegerv(GL_VIEWPORT, viewport);

// get the vertex position on the screen
GLdouble x,y,z;
gluProject(iter->x, iter->y, iter->z,
modelview, projection, viewport,
&x, &y, &z);
ivec3 screen_pos(x, y, iter->z);
```

不过这还没完，当我们使用下面代码画线时，是要使用局部坐标。
```
glBegin(GL_LINES);
glVertex2f(GLfloat(start.x), GLfloat(start.y));
glVertex2f(GLfloat(end.x), GLfloat(end.y));
glEnd();
```
因此我们再使用逆过程（三个坐标系参数和上面的一样）得到局部坐标再进行画线。
```
gluUnProject(screen_pos.x, screen_pos.y, 0,
                      modelview, projection, viewport,
                      &x, &y, &tmpZ);
```

**第二**，细心的你一定发现我竟然是画的二维的点且gluUnProject中的z竟然用的等于零，没错（此处脑补柯南推理的BGM）。因为我们是Zbuffer算法，在绘画时本来就不用考虑深度。我们留下的要画的就是离观察点最近的面片。我也是使用的二维正交投影。
```
gluOrtho2D(-Ortho*scale, Ortho*scale, -Ortho, Ortho);
```
把这里的scale就理解成一个为了解决因屏幕拉伸而导致物体形变的参数就好了。如果使用三维投影也是可以的，改成相应三维画线的方法就行。

**第三**，构建活性边表时，相对于某一值为y时，其实我们需要从对应于的多边形中找到边的最左一个和最右一个再填入活性边中。那就遇到一个问题，对于一个多边形而言，怎么判断一个边是最右边和最左边的呢？
当两个边的最左边端点x不相等时，最左边端点x值小的为左边
![lefter_general](/image/lefter_general.png)
 
当两个边的最左边端点x相等时（又体会到整数的好，否则浮点数比较起来肯定会出问题的嘛），我们比较分类边的dx（dx= -1/k）。
在下图中如箭头方向运动，则dx的变化从-∞ ~ 0 ~ +∞。而平行于x轴的直线我们是无法区分直线的方向的（图中向左和向右的箭头），所以更确切的变化方式是
-∞ ~ 0 ~ +∞<sup>-</sup>
![lefter_same x-coordinate](/image/lefter_samePos.png)
 那么再分成两个情况讨论：
- 两条线中**一条**平行于x轴，则其dx=-∞。则无论另一个边的dx为何值，另一条边是**左边**。
- 两条线的dx都不平行于x轴，那么dx值小的边为**左边**。
为什么不讨论两条边的dx都为-∞的情况呢？因为机智如我已经把垂直于视线的边提前去除了~

求最右边的边就是最左边的逆过程，你懂得~

### 结果截图
我给每个面片随机生成了颜色，且使用线的方式画图的，即对同一扫描线中，记下同一面片的起点和终点，然后进行线的绘制。
![ford](/image/ford_zbuffer.png)
（文件obj_ford_sample.obj，19921 faces）
![clip](/image/clip_zbuffer.png)
（文件obj_camper_sample.obj，4719 faces）

### 总结
虽然这个算法理解起来很容易，但其实我也是花了很久的时间才完成了该作业。而且总体的构架也不并满意，因为当出现bug时需要全局修改，即牵一发而动全身。而且我也是闭门造车的状态，相信一定有比使用坐标变换更高效的方法。我想有时间真的应该好好读一下设计模式，并将其使用在我的代码中。
> 毕竟不停地走出自己的舒适区，人才能成长嘛~
