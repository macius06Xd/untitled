package org.example;

import java.io.*;
import java.net.ServerSocket;
import java.net.Socket;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.concurrent.BlockingDeque;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.locks.ReentrantLock;

 class HandleClient implements Runnable
{
    Socket socket;
    LinkedBlockingQueue<String> messageQ;
    final int buffer_size = 1024;
    ArrayList<LinkedBlockingQueue<String>> messageQList;
    String name;

    ReentrantLock lock;
    boolean flag = false;
    HandleClient(Socket socket , LinkedBlockingQueue<String> messageQ ,ArrayList<LinkedBlockingQueue<String>> messageQList , ReentrantLock lock ) throws InterruptedException {
        this.socket = socket;
        this.messageQ = messageQ;
        this.name = null;
        this.messageQList = messageQList;
        this.lock = lock;


    }

    @Override
    public void run() {
        Thread socketThread = new Thread(new SocketThread());
        Thread queueThread = new Thread(new QueueThread());
        socketThread.start();
        queueThread.start();
        try {
            socketThread.join();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        try {
            queueThread.join();
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        lock.lock();
        messageQList.remove(messageQ);
        lock.unlock();
    }

    public class SocketThread implements Runnable
    {

        @Override
        public void run() {
            System.out.println("siemanko");
            byte[] buffer = new byte[buffer_size];
            try {
                PrintWriter out = new PrintWriter(socket.getOutputStream(),true);
                BufferedReader in = new BufferedReader(new InputStreamReader(socket.getInputStream()));
                out.println("Please provide me your username");
                System.out.println("wrote");
                 name  = in.readLine();
                out.println("Welcome");
                while(true)
                {
                    String recived = in.readLine();
                    if( recived== null)
                    {
                        flag = true;
                        break;
                    }
                    StringBuilder sb = new StringBuilder();
                    sb.append(name).append(": ").append(recived).append("\n");
                    String message = sb.toString();

                    lock.lock();
                    messageQList.forEach(x -> {
                        if(x!= messageQ)
                        {
                            try {
                                x.put(message);
                            } catch (InterruptedException ignored) {
                                System.out.println("Problemy");
                            }
                        }
                    });
                    lock.unlock();
                }
            } catch (IOException ignored) {
            }
        }
    }

    public class QueueThread implements  Runnable
    {

        @Override
        public void run() {
            PrintWriter out = null;
            try {
                out = new PrintWriter(socket.getOutputStream(),true);
            } catch (IOException e) {
                throw new RuntimeException(e);
            }
            while (true) {
                try {

                    String message = messageQ.poll(200, TimeUnit.MILLISECONDS);

                    if (message != null) {
                        System.out.println("Wiadomosc");
                        System.out.println(message);
                        out.println(message);
                    }
                    if(flag) break;
                } catch (InterruptedException e) {
                    throw new RuntimeException(e);
                }
            }
        }
    }
}
public class Main {
    final int buffer_size = 1024;


    public static void main(String[] args) throws IOException, InterruptedException {
        ReentrantLock lock = new ReentrantLock();
        LinkedBlockingQueue<ByteBuffer> messageQ = new LinkedBlockingQueue<>();
        ArrayList<LinkedBlockingQueue<String>> messageQlist = new ArrayList<>();
        ServerSocket socket = new ServerSocket(5000);
        while(true) {
            Socket clientSocket = socket.accept();
            System.out.println("UserAccept");
            LinkedBlockingQueue<String> queue = new LinkedBlockingQueue<String>();
            lock.lock();
            messageQlist.add(queue);
            lock.unlock();
            System.out.println("Setup finished");
            HandleClient clientRun = new HandleClient(clientSocket,queue,messageQlist,lock);
            Thread userThread = new Thread(clientRun);
            userThread.start();
            System.out.println("UserStarted");

        }




    }
}