# gamepad_io

手柄接入功能包，提供手柄数据打印和振动控制。
当前按键映射基于雷神 G30S 的接收器 XInput 模式。

## 启动方法

```bash
source ~/gamepad_ws/install/setup.bash
ros2 launch gamepad_io gamepad.launch.py
```

第一条加载工作空间，第二条启动手柄驱动、打印和振动节点。

关闭打印、保留振动：

```bash
ros2 launch gamepad_io gamepad.launch.py enable_print:=false enable_rumble:=true
```

- enable_print：是否启动打印，默认 true。
- enable_rumble：是否启动振动接口，默认 true。
- 手柄参数保存在 config/gamepad.yaml。
- device_id 是设备枚举编号，插入多个手柄后需要重新确认。

## 对外接口

| 话题 | 消息类型 | 作用 |
|---|---|---|
| /joy | sensor_msgs/msg/Joy | 输出手柄按键和轴数据 |
| /rumble_cmd | gamepad_io/msg/RumbleCommand | 接收振动指令 |

其他项目通过这两个话题接入，无需修改本包。

## 振动指令

- intensity：强度，范围 0.0～1.0。
- duration_ms：持续时间，单位毫秒。
- 强度或时间为 0：停止振动。
- 新命令立即覆盖旧命令，并重新计时。
- 非法强度会被拒绝，不改变当前任务。
- 实际持续时间存在少量调度误差。

发送一次强度 0.5、持续约 2 秒的指令：

```bash
ros2 topic pub --once /rumble_cmd gamepad_io/msg/RumbleCommand "{intensity: 0.5, duration_ms: 2000}"
```

pub 表示发布消息，--once 表示只发送一次。
发送前，当前终端需要加载本工作空间，且振动节点和手柄驱动正在运行。

## G30S 轴映射

| 下标 | 控件 | 方向或范围 |
|---|---|---|
| axes[0] | 左摇杆左右 | 左 +1，右 -1 |
| axes[1] | 左摇杆上下 | 上 +1，下 -1 |
| axes[2] | LT | 松开 +1，按到底 -1 |
| axes[3] | 右摇杆左右 | 左 +1，右 -1 |
| axes[4] | 右摇杆上下 | 上 +1，下 -1 |
| axes[5] | RT | 松开 +1，按到底 -1 |
| axes[6] | 十字键左右 | 左 +1，右 -1 |
| axes[7] | 十字键上下 | 上 +1，下 -1 |

## G30S 按键映射

buttons 中，1 表示按下，0 表示松开。

- A、B、X、Y：下标分别为 0、1、2、3。
- LB、RB：下标分别为 4、5。
- SELECT、START：下标分别为 6、7。
- 开机键：下标 8。
- 左、右摇杆按下：下标分别为 9、10。

换手柄或切换连接模式后，需要重新确认映射。
当前接口没有提供电量、充电状态和无线信号强度。