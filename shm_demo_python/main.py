import cv2

from image_receiver import ImageReceiver

if __name__ == '__main__':
    image_receiver = ImageReceiver("SharedDataDemo")
    img_data = image_receiver.receive_image()
    if img_data is not None:
        # 保存图片
        cv2.imwrite("result.jpg", img_data)
        received = True
