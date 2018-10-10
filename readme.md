## 红外目标跟踪

采用目标追踪中最简易的二帧差法，根据第一张图中的目标分析第二张图片中的对应目标及位置。程序基本采用提示中的算法，在最后的第二帧定位中稍有差别。

#### 程序流程

1. 对于题目给出的第一帧图片以及目标位置，首先读取为*img*，中值滤波后转换为RGB图*img2*，根据数据在*img2*中画出对应的红色方框。

![frame1](https://github.com/LunaElfGaming/VisualTrack/raw/master/photoshop/frame1.bmp)

2. 接下来新建*subimg*，从第一帧中裁剪出红色方框内的局部图，采用*calcHist*函数计算局部图的灰度直方图，存储在*histimg*中并归一化，为两帧之间局部图的对比做准备。

3. 读取第二帧图像*nex*，中值滤波后转换出RGB图像*nex2*

4. 通过subtract函数提取第一第二帧之间的插值，并threshold二值化，二值化结果如下
![diff](https://github.com/LunaElfGaming/VisualTrack/raw/master/photoshop/diff.bmp)

5. 对于一般的二帧差算法，差异图已经能反映出第二帧中人物的位置。为了更精确的结果，此时用findContours算法寻找图片中所有的边界（不排除有一些白色小噪点），而边界点最多者为图中白色人物的边界。针对这一边界区，找出最小的方框将其包含在内，并计算出方框的中心。

6. 以方框的中心为核心，计算不同宽高的方框，计算对应的第二帧中局部图与第一帧局部图的灰度直方图比较系数，采用Bhattacharyya 系数计算，寻找合适的宽、高，使Bhattacharyya 系数最小。

最后输出第二帧中定位框的位置，以及相应的Bhattacharyya 系数

![frame2](https://github.com/LunaElfGaming/VisualTrack/raw/master/photoshop/frame2.bmp)

#### 程序执行

程序默认打开三个窗口，分别是一二帧的帧图以及差异图，并默认保存，按任意键关闭所有窗口并结束程序。

#### 讨论

一般二帧差法在得到差异图后就可以定位，而想要具体计算一下合适的定位框就显得底蕴不足，本程序采用的方法是根据第一帧中定位框的大小，确定适当的浮动区间来在第二帧中寻找合适的定位框，即默认了两帧之间目标的大小差异不会很大。
