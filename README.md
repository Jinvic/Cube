# Cube

计算机图像学结课作业
预计做一个魔方模拟器，先做一个立方体。

## 参考

### 关于计算机图形学

**计算机图形学———基于MFC三维图形开发 孔令德著（第一版）** [链接](https://u.xueshu86.com/13429290.html)

本来教材是第二版，没找到电子书就用第一版凑合下。

- **基础MFC绘图方法** 见第二章 MFC绘图基础。
- **三维图形变换、透视投影** 见第六章 三维变换与投影。
- **消隐算法** 见第八章 建模与消隐。

### 关于VC++

- **消息映射机制** [链接](https://blog.csdn.net/qq_44974888/article/details/124355801)
- **鼠标响应** [链接](https://blog.csdn.net/Eastmount/article/details/53192634)

### 关于计算几何

- 向量的**数量积**、**向量积**、**夹角**求取

### API（MSDN）

- **CFrameWndEx类** [链接](https://learn.microsoft.com/zh-cn/cpp/mfc/reference/cframewndex-class?view=msvc-170) 关于`CFrameWndEx::OnLButtonDown`、`CFrameWndEx::OnLButtonUp`、`CFrameWndEx::OnMouseMove`等方法。
- **鼠标输入通知** [链接](https://learn.microsoft.com/zh-cn/windows/win32/inputdev/mouse-input-notifications) 关于`WM_LBUTTONDOWN`、`WM_LBUTTONUP`、`WM_MOUSEMOVE`等消息。

## 说明

关于立方体的定义和操作整合在文件`Cube.cpp`内。
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
 Matrix CP_st;//按下鼠标时的初始状态，用于鼠标移动时实时更新后还原
 bool lm_clicked;//左键是否按下
```

具体实现请在`CubeView.cpp`中查看。
阅读代码只需要看上述三个文件，其他都是创建项目时VS自动生成，未进行修改。

## TODO List

- [x] 以立方体中心为原点建立三维坐标系
- [x] 实现投影映射
- [x] 实现消隐算法
- [x] 实现鼠标事件映射
- [x] 实现鼠标坐标变换到立方体旋转角度的转换
- [x] 实现立方体旋转功能
- [x] 由8点6面的立方体细化为216点54面的魔方
- [ ] 鼠标落点检测，判断鼠标具体落在哪个面
- [ ] 实现面到层的定位
- [ ] 实现鼠标坐标变换到层选择以及旋转角度的转换
- [ ] 实现层旋转功能
- [x] 添加双缓冲机制
