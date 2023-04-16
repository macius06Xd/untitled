import socket
import threading
from PyQt5.QtCore import Qt
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QTextEdit, QLineEdit, QPushButton


class ChatWidget(QWidget):
    def __init__(self):
        super().__init__()
        # Create a TCP socket
        self.username = None;
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

        # Connect to the server
        self.sock.connect(('127.0.0.1', 5000))
        # Start receive_messages thread
        recv_thread = threading.Thread(target=self.receive_messages)
        recv_thread.start()
        # Create a vertical layout
        layout = QVBoxLayout()

        # Create a QTextEdit widget to display messages
        self.text_edit = QTextEdit()
        self.text_edit.setReadOnly(True)
        layout.addWidget(self.text_edit)

        # Create a QLineEdit widget for user input
        self.line_edit = QLineEdit()
        self.line_edit.returnPressed.connect(self.send_messages)
        layout.addWidget(self.line_edit)

        # Create a QPushButton to send messages
        self.send_button = QPushButton("Send")
        self.send_button.clicked.connect(self.send_messages)
        layout.addWidget(self.send_button)

        # Set the layout
        self.setLayout(layout)

    def send_messages(self):
            message =  self.line_edit.text()
            if(len(message)!= 0):
                self.sock.sendall(message.encode())
                if(self.username is None):
                    self.username = message
                else:
                    self.text_edit.append(f"{self.username}: {message}")
                self.text_edit.verticalScrollBar().setValue(self.text_edit.verticalScrollBar().maximum())
                self.line_edit.clear()


    def receive_messages(self):
        while True:
            server_reply = self.sock.recv(2000)
            if not server_reply:
                break
            self.text_edit.append(server_reply.decode())
            self.text_edit.verticalScrollBar().setValue(self.text_edit.verticalScrollBar().maximum())

if __name__ == '__main__':

    app = QApplication([])
    widget = ChatWidget()

    widget.show()
    app.exec_()