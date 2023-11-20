# Cube

计算机图像学结课作业

预计做一个魔方模拟器。

[项目地址](https://github.com/Jinvic/Cube)

分支cube为仅立方体功能的存档。

## 参考

### 关于计算机图形学

**计算机图形学———基于MFC三维图形开发 孔令德著（第一版）** [链接](https://u.xueshu86.com/13429290.html)

本来教材是第二版，没找到电子书就用第一版凑合下。

- **基础MFC绘图方法** 见第二章 MFC绘图基础。
- **三维图形变换、透视投影** 见第六章 三维变换与投影。
- **消隐算法** 见第八章 建模与消隐。
- **双缓冲机制** 见第二章 MFC绘图基础。

### 关于VC++

- **消息映射机制** [链接](https://blog.csdn.net/qq_44974888/article/details/124355801)
- **鼠标响应** [链接](https://blog.csdn.net/Eastmount/article/details/53192634)

### 关于计算几何

- 向量的**数量积**、**向量积**、**夹角**求取
- 判断点**是否在线段上**，**是否在凸包内**（不含边）

### API（MSDN）

- **CFrameWndEx类** [链接](https://learn.microsoft.com/zh-cn/cpp/mfc/reference/cframewndex-class?view=msvc-170) 关于`CFrameWndEx::OnLButtonDown`、`CFrameWndEx::OnLButtonUp`、`CFrameWndEx::OnMouseMove`等方法。
- **鼠标输入通知** [链接](https://learn.microsoft.com/zh-cn/windows/win32/inputdev/mouse-input-notifications) 关于`WM_LBUTTONDOWN`、`WM_LBUTTONUP`、`WM_MOUSEMOVE`等消息。
- **bitBlt 函数** (wingdi.h) [链接](https://learn.microsoft.com/zh-cn/windows/win32/api/wingdi/nf-wingdi-bitblt) 该函数的各参数意义以及使用示例。

## 说明

关于立方体的定义和操作整合在文件`Cube.hpp`内。

关于计算几何的结构定义和算法实现整合在文件`ComputationGeometry.hpp`内。

MFC功能主要在CCubeView类中实现。

添加的一些成员函数和变量可在`CubeView.h`中查看，如下：

```C++
public:
 void DrawCube(void);// 绘图函数,绘制立方体当前状态
 afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
 afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
 afx_msg void OnMouseMove(UINT nFlags, CPoint point);
private:
 Cube cub;//立方体
 CPoint m_st_pos;//按下鼠标的起始位置
 Matrix CP_st;//按下鼠标时的初始状态，用于实时更新鼠标移动后进行还原
 bool lm_clicked;//左键是否按下
 int Fs_idx;//鼠标按下时落在哪个面上。若为Cube::OutFace，即-1，则表示未落在面上
```

具体实现请在`CubeView.cpp`中查看。
阅读代码只需要看上述四个文件，其他都是创建项目时VS自动生成，未进行修改。

## 关于调试

在`Cube.hpp`的头提供了宏`debug`和枚举变量`debug_flags`作为debug选项。具体定义如下：

```C++
const enum debug_flags {...};
#define debug 【上面定义的选项】
#undef debug
```

只需要注释掉`#undef debug`，并将debug宏定义为上一行中的选项即可启用调试。

目前定义的debug选项有：

- `real_time_visibility`：实时显示鼠标移动时各色面的可见度。指标为面的外法向量与视向量的夹角θ与π的比值。当值小于0.5时，夹角为锐角，该面可见，且值越小可见面积越大。当值大于0.5时，夹角为钝角，该面不可见。
- `mouse_LBdown_onWhichFace`：按下鼠标左键时鼠标的位置以及落在哪个小面上，使用计算几何中的凸包相关算法判断。

## TODO List

- [x] 以立方体中心为原点建立三维坐标系
- [x] 实现投影映射
- [x] 实现消隐算法
- [x] 实现鼠标事件映射
- [x] 实现鼠标坐标变换到立方体旋转角度的转换
- [x] 实现立方体旋转功能
- [x] 由8点6面的立方体细化为152点54面的魔方
- [x] 鼠标落点检测，判断鼠标具体落在哪个面
- [x] 实现面到层的连接
- [x] 实现层的定义和旋转
- [ ] 向层中添加所含点
- [x] 添加双缓冲机制
