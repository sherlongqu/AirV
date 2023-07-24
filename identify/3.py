import paho.mqtt.client as mqtt
import json
import tkinter as tk

# 创建Tkinter窗口
window = tk.Tk()
window.title("中国象棋")
window.geometry("400x400")

# 创建一个Canvas用于绘制棋盘
canvas = tk.Canvas(window, width=400, height=400, bg="white")
canvas.pack()

# 连接到MQTT代理
client = mqtt.Client()
client.connect("47.115.214.192", 1883)  # 替换为您的MQTT代理地址

# 初始化棋局的JSON文本
chessboard = {
    "chessboard": [
        {"piece": "車", "color": "red", "row": 0, "col": 0},
        {"piece": "馬", "color": "red", "row": 0, "col": 1},
        {"piece": "相", "color": "red", "row": 0, "col": 2},
        {"piece": "仕", "color": "red", "row": 0, "col": 3},
        {"piece": "帥", "color": "red", "row": 0, "col": 4},
        {"piece": "仕", "color": "red", "row": 0, "col": 5},
        {"piece": "相", "color": "red", "row": 0, "col": 6},
        {"piece": "馬", "color": "red", "row": 0, "col": 7},
        {"piece": "車", "color": "red", "row": 0, "col": 8},
        {"piece": "砲", "color": "red", "row": 2, "col": 1},
        {"piece": "砲", "color": "red", "row": 2, "col": 7},
        {"piece": "兵", "color": "red", "row": 3, "col": 0},
        {"piece": "兵", "color": "red", "row": 3, "col": 2},
        {"piece": "兵", "color": "red", "row": 3, "col": 4},
        {"piece": "兵", "color": "red", "row": 3, "col": 6},
        {"piece": "兵", "color": "red", "row": 3, "col": 8},
        {"piece": "卒", "color": "black", "row": 6, "col": 0},
        {"piece": "卒", "color": "black", "row": 6, "col": 2},
        {"piece": "卒", "color": "black", "row": 6, "col": 4},
        {"piece": "卒", "color": "black", "row": 6, "col": 6},
        {"piece": "卒", "color": "black", "row": 6, "col": 8},
        {"piece": "車", "color": "black", "row": 9, "col": 0},
        {"piece": "馬", "color": "black", "row": 9, "col": 1},
        {"piece": "象", "color": "black", "row": 9, "col": 2},
        {"piece": "士", "color": "black", "row": 9, "col": 3},
        {"piece": "將", "color": "black", "row": 9, "col": 4},
        {"piece": "士", "color": "black", "row": 9, "col": 5},
        {"piece": "象", "color": "black", "row": 9, "col": 6},
        {"piece": "馬", "color": "black", "row": 9, "col": 7},
        {"piece": "車", "color": "black", "row": 9, "col": 8},
    ]
}
json_text = json.dumps(chessboard)

# 发布JSON文本到MQTT话题
client.publish("chessboard_topic", json_text)


# 定义收到消息的回调函数
def on_message(client, userdata, msg):
    # 解析JSON文本
    json_text = msg.payload.decode()
    chessboard_data = json.loads(json_text)

    # 清空棋盘
    canvas.delete("all")

    # 绘制棋盘网格
    for i in range(10):
        canvas.create_line(40, 40 * i + 40, 360, 40 * i + 40)
    for i in range(9):
        canvas.create_line(40 * i + 40, 40, 40 * i + 40, 400)

    # 绘制棋子
    for piece_data in chessboard_data["chessboard"]:
        piece = piece_data["piece"]
        color = piece_data["color"]
        row = piece_data["row"]
        col = piece_data["col"]

        x = col * 40 + 40
        y = row * 40 + 40

        canvas.create_text(x, y, text=piece, fill=color)


# 订阅MQTT话题
client.on_message = on_message
client.subscribe("chessboard_topic")

# 启动Tkinter事件循环
window.mainloop()
